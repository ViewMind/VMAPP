<?php

include_once("FixationDataGenerator.php");
include_once("RawDataInterpreter.php");
include_once("CSVGenerator.php");
include_once("TargetHitSearcher.php");

$raw_data_input = "/home/ariel/repos/viewmind_projects/VMTools/RawJSONDataDev/bin/test.json";
$use_old_hitboxes = true; // Eventually this can be inferred from the study date. 

$rdi = new RawDataInterpreter();
$pdc = new ProcessedDataContainer();

$rdi->loadFromFile($raw_data_input);

if ($rdi->getError() != ""){
   echo "Error loading raw data: " . $rdi->getError();
   exit();
}

$studies = $rdi->getAvailableStudies();
if (empty($studies)){
   echo "No studies detected in loaded data\n";
   exit();
}
$study = $studies[0];

if (!StudyTypes::validate($study)){
   echo "Invalid study type $study\n";
   var_dump($studies);
   exit();
}

echo "Selected study is $study\n";

$is_reading = ($study == StudyTypes::READING);

if ($study != StudyTypes::BINDING){
   $trial_list_type = TrialListTypes::UNIQUE;
}
else{
   //$trial_list_type = [TrialListTypes::BOUND, TrialListTypes::UNBOUND];
   //$trial_list_type = TrialListTypes::BOUND; 
   $trial_list_type = TrialListTypes::UNBOUND;

   if ($study == StudyTypes::BINDING){
      $binding_score[BindingScore::TEST_CORRECT] = 0;
      $binding_score[BindingScore::TEST_WRONG] = 0;
      $binding_score[BindingScore::CORRECT] = 0;
      $binding_score[BindingScore::WRONG] = 0;
   }   

}

$subject_age = $rdi->getSubjectDataValue(SubjectFields::AGE);
if ($subject_age == ""){
   echo "Subject age retrieval error: " . $rdi->getError();
   exit();
}

$pp_key = $rdi->getMetaDataField(MetadaFields::PROCESSING_PARAMETER_KEY);
if ($pp_key == ""){
   echo "PP Key retrieval error: " . $rdi->getError();
   exit();
}

if ($pp_key == 0) {
   // // This should be obtained from a Database Request based on the $pp_key
   // GazePoint Parameters
   $db_params[ProcessingParameter::SAMPLE_FREQUENCY        ] = 150;
   $db_params[ProcessingParameter::MAX_DISPERSION_WINDOW   ] = 197;
   $db_params[ProcessingParameter::MINIMUM_FIXATION_LENGTH ] = 50;
   $db_params[ProcessingParameter::LATENCY_ESCAPE_RADIOUS  ] = 80;

   // HTC Vive Paramters. 
   // This should be obtained from a Database Request based on the $pp_key
   // $db_params[ProcessingParameter::SAMPLE_FREQUENCY        ] = 120;
   // $db_params[ProcessingParameter::MAX_DISPERSION_WINDOW   ] = 105;
   // $db_params[ProcessingParameter::MINIMUM_FIXATION_LENGTH ] = 50;
   // $db_params[ProcessingParameter::LATENCY_ESCAPE_RADIOUS  ] = 80;

}
else{
   echo "Unknown processing paramter key: $pp_key\n";
   exit();
}

$processing_parameters = $rdi->getProcessingParameters();
//var_dump($processing_parameters);
$processing_parameters = array_merge($processing_parameters,$db_params);
//var_dump($processing_parameters);

echo "Setting path for $study with trial list type $trial_list_type\n";

if (!$rdi->setRawDataAccessPathForTrialList($study,$trial_list_type)){
   echo "Setting access path for study: " . $rdi->getError() . "\n";
   exit();   
}


$trial_count = $rdi->getNumberOfTrials();;
echo "Number of trials: $trial_count\n"; 

$fdg = new FixationDataGenerator();

if (!$fdg->setProcessingParameters($processing_parameters)){
   echo "Setting pp error is " . $fdg->getError() . "\n";
   exit();
}

$valid_eye = $rdi->getValidEyeForCurrentStudy();
if ($valid_eye == ""){
   echo "Error getting valid eye: " . $rdi->getError() . "\n";
   exit();
}

$fix_l = array();
$fix_r = array();

$pdc->setSubjectData("",$subject_age);

// Creating the output structure.
if (!$pdc->createNewStudy($study,$trial_list_type,$valid_eye)){
   echo "Creating new study in output " . $pdc->getError() . "\n";
   exit();
}

if (!$pdc->setActiveStudyAndTrialListType($study,$trial_list_type)){
   echo "Setting active study in output " . $pdc->getError() . "\n";
   exit();
}

// Processing each trial. 
$data_set_type_list = $rdi->getAvailableDataSetsForSelectedStudy();

// Check for NBack family of studies. 
if (($study == StudyTypes::NBACKRT) || ($study == StudyTypes::NBACKRT) || ($study == StudyTypes::NBACKRT)){
   $hit_boxes = $processing_parameters[ProcessingParameter::NBACK_HITBOXES];
   if ($use_old_hitboxes) {
      $hit_boxes = TargetHitSearcher::nonAdjustedTargetBoxes($processing_parameters[ProcessingParameter::NBACK_HITBOXES]);
   }
}

// Check for GoNoGo
if ($study == StudyTypes::GONOGO){
   
   $hit_boxes = $processing_parameters[ProcessingParameter::GONOGO_HITBOXES];

   //var_dump(array_keys($processing_parameters));

   // Map the trial description to the index of the hitbox in the processing parameters. 
   $map_trial_desc_to_targe_box_index["R<-"] = GoNoGoTargetBoxes::RIGHT;   // The right box
   $map_trial_desc_to_targe_box_index["G<-"] = GoNoGoTargetBoxes::LEFT;    // The left box. 
   $map_trial_desc_to_targe_box_index["R->"] = GoNoGoTargetBoxes::LEFT;
   $map_trial_desc_to_targe_box_index["G->"] = GoNoGoTargetBoxes::RIGHT;      

}

$study_start_timestamp = -1;
$study_end_timestamp = -1;

for ($i = 0; $i < $trial_count; $i++){

   $all_data_set_values = array();
   $all_fix_l = array();
   $all_fix_r = array();

   // Getting the original trial structure
   $trial = $rdi->getTrial($i);
   if ($rdi->getError() != ""){
      echo "Getting trial $i: " . $rdi->getError() . "\n";
      exit();
   }   

   foreach ($data_set_type_list as $data_set_type) {
      $dataset = $rdi->getDataSetTypeFromTrial($data_set_type, $i);
      if ($rdi->getError() != "") {
         echo "Getting data set: " . $rdi->getError() . "\n";
         exit();
      }

      if (count($dataset) == 0){
         echo "Empty data set for trial $i and $data_set_type data set type\n";
         exit();
      }

      // Getting the smallest and minimum timestamps. 
      if (($study_start_timestamp == -1) || ($study_start_timestamp > $dataset[0][RawDataVectorFields::TIMESTAMP])){
         $study_start_timestamp = $dataset[0][RawDataVectorFields::TIMESTAMP];
      }

      $last = count($dataset)-1;

      if (($study_end_timestamp == -1) || ($study_end_timestamp < $dataset[$last][RawDataVectorFields::TIMESTAMP])){
         $study_end_timestamp = $dataset[$last][RawDataVectorFields::TIMESTAMP];
      }

      // Computing the fixations. 
      $fix_l = $fdg->computeFixationsForDataSet($dataset, EyeType::LEFT, $is_reading);
      $fix_r = $fdg->computeFixationsForDataSet($dataset, EyeType::RIGHT, $is_reading);

      // Used to compute timeline values
      $start_time = $dataset[0][RawDataVectorFields::TIMESTAMP];

      // Go Go No Go requires a hitbox for its data set values. 
      $target_hit_box = array();
      if ($study = StudyTypes::GONOGO) {
         // So the right target is pointed by the value of the trial description in the map (there should be 3, but the first one is the arrow, center box)
         $trial_description = trim($trial[TrialFields::TRIAL_TYPE]);
         if (!array_key_exists($trial_description,$map_trial_desc_to_targe_box_index)){
            echo "Invalid Go No Go Trial description found: $trial_description\n";
            exit();
         }
         $target_hit_box_index = $map_trial_desc_to_targe_box_index[$trial_description];
         $target_hit_box = $hit_boxes[$target_hit_box_index];     
         
         // Computing the go no go  hit logic fo each fixations. 
         TargetHitSearcher::computeFixationHitsGoNoGo($hit_boxes,$fix_l,$target_hit_box_index,$fdg->getScreenCenter()[0]);  
         TargetHitSearcher::computeFixationHitsGoNoGo($hit_boxes,$fix_r,$target_hit_box_index,$fdg->getScreenCenter()[0]);           
      }

      // Computing values which are for the data set. 
      $data_set_values = $fdg->computeDataSetValues($dataset, $fix_l, $fix_r, $target_hit_box);

      // Computing values for each fixation. 
      $fdg->computeFixationDependantValues($fix_l, $start_time);
      $fdg->computeFixationDependantValues($fix_r, $start_time);

      $all_data_set_values[$data_set_type] = $data_set_values;
      $all_fix_l[$data_set_type] = $fix_l;
      $all_fix_r[$data_set_type] = $fix_r;
   }

   // To be filled if necessary, according to each study. 
   $trial_values = array();

   // Specific NBack computations
   if (($study == StudyTypes::NBACKRT) || ($study == StudyTypes::NBACKMS)){

      //echo "L Fixations Trial ID: " . $trial[TrialFields::ID] . "\n";
      $result = TargetHitSearcher::computeFixationBasedNBackValues($all_fix_l,$hit_boxes ,$trial[TrialFields::TRIAL_TYPE],$study);
      if ($result[TargetHitSearcher::COMPUTE_RET_ERROR] != ""){
         echo "Error while computing NBack Fixation L Values: " . $result[TargetHitSearcher::COMPUTE_RET_ERROR] . "\n";
         exit();
      }

      //var_dump($result);
      $trial_values[TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_L] = $result[TargetHitSearcher::COMPUTE_RET_SEQ_COMPLETE];

      //echo "R Fixations Trial ID: " . $trial[TrialFields::ID] . "\n";
      $result = TargetHitSearcher::computeFixationBasedNBackValues($all_fix_r,$hit_boxes, $trial[TrialFields::TRIAL_TYPE],$study);
      if ($result[TargetHitSearcher::COMPUTE_RET_ERROR] != "") {
         echo "Error while computing NBack Fixation R Values: " . $result[TargetHitSearcher::COMPUTE_RET_ERROR] . "\n";
         exit();
      }

      $trial_values[TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_R] = $result[TargetHitSearcher::COMPUTE_RET_SEQ_COMPLETE];
   }
   
   // Specific Binding computations
   else if ($study == StudyTypes::BINDING){

      //echo "TrialID: " . $trial[TrialFields::ID] . ". Trial Type |" . $trial[TrialFields::TRIAL_TYPE] . "|. Response: |" . $trial[TrialFields::RESPONSE] . "|\n";

      $trial_type = trim($trial[TrialFields::TRIAL_TYPE]);
      $response = trim($trial[TrialFields::RESPONSE]);

      if (str_contains($trial[TrialFields::ID],"test")){
         if ($trial_type == $response) $binding_score[BindingScore::TEST_CORRECT]++;
         else $binding_score[BindingScore::TEST_WRONG]++;
      }
      else{
         if ($trial_type == $response) $binding_score[BindingScore::CORRECT]++;
         else $binding_score[BindingScore::WRONG]++;
      }

   }

   // Creating the new trial 
   $trial_id = $pdc->addTrial($trial,$trial_values);
   if ($trial_id == -1){      
      echo "Error adding a new Trial To the Processed Data Structure " . $pdc->getError() . "\n";
      var_dump($trial);
      exit();      
   }

   // Filing the trial structure withe the computed data and storing said data in the processed structure. 
   foreach ($data_set_type_list as $data_set_type) {
      if (!$pdc->addDataSetDataToTrial($trial_id,$all_data_set_values[$data_set_type], $all_fix_l[$data_set_type], $all_fix_r[$data_set_type], $data_set_type)){
         echo "Adding data set $data_set_type to trial struct $trial_id: " . $pdc->getError() . "\n";
         exit();
      }
   }
}

// Setting the Trial list values. 
$trial_list_values[TrialListTypeDataField::DURATION] = $study_end_timestamp - $study_start_timestamp;
if ($study == StudyTypes::BINDING) {
   $trial_list_values[TrialListTypeDataField::BINDING_SCORE] = $binding_score;
}
if (!$pdc->setTrialListData($trial_list_values)){
   echo "Error setting the trial list data: " . $pdc->getError();
   exit();
}

// echo "Count total fixations for left: "  . count($fix_l) . "\n";
// echo "Count total fixations for right: " . count($fix_r) . "\n";

if ($study == StudyTypes::READING) {
   $ans = CSVGenerator::readingCSV($pdc, "test_reading.csv");
   if ($ans == "") {
      echo "CSV Reading Finished OK\n";
   } else {
      echo "ERROR generating CSV: $ans\n";
   }
}
else if ($study == StudyTypes::NBACKRT){
   $ans = CSVGenerator::nbackrtCSV($pdc, "test_nbackrt.csv");
   if ($ans == "") {
      echo "CSV NBack RT Finished OK\n";
   } else {
      echo "ERROR generating CSV: $ans\n";
   }
}
else if ($study == StudyTypes::BINDING){
   if ($trial_list_type == TrialListTypes::BOUND) $code = "bc";
   else if ($trial_list_type == TrialListTypes::UNBOUND) $code = "uc";
   else {
      echo "Unknown trial list type to generate the CSV: $trial_list_type\n";
      exit();
   }
   $ans = CSVGenerator::bindingCSV($pdc, "test_binding$code.csv",$trial_list_type);
   if ($ans == "") {
      echo "CSV Binding Finished OK\n";
   } else {
      echo "ERROR generating CSV: $ans\n";
   }
}
else  if ($study == StudyTypes::GONOGO) {
   $ans = CSVGenerator::gonogoCSV($pdc, "test_gonogo.csv");
   if ($ans == "") {
      echo "CSV GoNoGo Finished OK\n";
   } else {
      echo "ERROR generating CSV: $ans\n";
   }   
}
else{
   echo "No CSV Generator for Study $study\n";
}


?> 
