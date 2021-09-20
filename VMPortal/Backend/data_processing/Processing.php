<?php

include_once (__DIR__ . "/../common/named_constants.php");
include_once ("FixationDataGenerator.php");
include_once ("TargetHitSearcher.php");

class Processing
{
   // Maps the trial type description to the right answer on a GoNoGo Trial (In GoNoGo DataSet = Trial)
   const GONOGO_ANSWER_MAP = [
      "R<-" => GoNoGoTargetBoxes::RIGHT,   // The right box
      "G<-" => GoNoGoTargetBoxes::LEFT,    // The left box. 
      "R->" => GoNoGoTargetBoxes::LEFT,
      "G->" => GoNoGoTargetBoxes::RIGHT         
   ];

   const CSV_FUNCTION_MAP = [
      Study::GONOGO => "gonogoCSV",
      Study::NBACKRT => "nbackrtCSV",
      Study::READING => "readingCSV",
      Study::BINDING_UC => "bindingCSV",
      Study::BINDING_UC => "bindingCSV"
   ];

   private $error;
   private $processing_parameters;
   private FixationDataGenerator $fdg;
   private $study_values;

   // The function will require several processing parameters, so they are the argument for it's constructor.  
   function __construct($pp){
      $this->processing_parameters = $pp;

      $this->error = "";

      $this->fdg = new FixationDataGenerator();

      if (!$this->fdg->setProcessingParameters($pp)){
         $this->error =  "Setting processing parameters in the fixation data generator provided the followig error: " . $this->fdg->getError() . "\n";
      }      
   }

   function getError(){
      return $this->error;
   }

   function getStudyComputedValues() {
      return $this->study_values;
   }

  /**
   * Function that computes all necessary values at a data set level.
   * A dataset will have the raw data and fixation fields which are all lists of associative arrays.
   *
   * The study is the name of the study being processed.
   */

   private function processDataSet(&$dataset, $study, $trial_description)
   {

      if ($this->error != "") return false;

      $this->error = "";
      $target_hit_box = array();

      $dataset[DataSetField::DATA_SET_VALUES] = array();

      // Doing a complete verification of the structure would be too long and complicated. And this should have been done in the client.
      // Hence we only check the main fields. This is more a just in case, than anything else.
      $missing_fields = DataSetField::listMissingFields(array_keys($dataset));
      if (!empty($missing_fields)){
         $this->error = "DataSet has missing field: " . implode(",",$missing_fields);
         return false;
      }

      $raw_data = $dataset[DataSetField::RAW_DATA];
      $fix_l    = $dataset[DataSetField::FIXATIONS_L];
      $fix_r    = $dataset[DataSetField::FIXATIONS_R];
      $start_time = $raw_data[0][DataVectorField::TIMESTAMP];

      // Specific Study Processing. 
      if ($study == Study::GONOGO) {

         $hit_boxes = $this->processing_parameters[ProcessingParameter::GONOGO_HITBOXES];
         $centerX = $this->processing_parameters[ProcessingParameter::RESOLUTION_WIDTH]/2;

         // So the right target is pointed by the value of the trial description in the map (there should be 3, but the first one is the arrow, center box)
         if (!array_key_exists($trial_description, self::GONOGO_ANSWER_MAP)) {
             $this->error = "Invalid Go No Go Trial description found: $trial_description\n";
             return false;
         }
         $target_hit_box_index = self::GONOGO_ANSWER_MAP[$trial_description];
         $target_hit_box = $hit_boxes[$target_hit_box_index];
     
         // Computing the go no go  hit logic fo each fixations.
         TargetHitSearcher::computeFixationHitsGoNoGo($hit_boxes, $fix_l, $target_hit_box_index, $centerX);
         TargetHitSearcher::computeFixationHitsGoNoGo($hit_boxes, $fix_r, $target_hit_box_index, $centerX);
      }

      // General value computations. 

      // Computing values which are for the data set. 
      $data_set_values = $this->fdg->computeDataSetValues($raw_data, $fix_l, $fix_r, $target_hit_box);

      // Computing values for each fixation. 
      $this->fdg->computeFixationDependantValues($fix_l, $start_time);
      $this->fdg->computeFixationDependantValues($fix_r, $start_time);

      // Storing the dataset values and the new computed fixations.
      $dataset[DataSetField::DATA_SET_VALUES] = $data_set_values;
      $dataset[DataSetField::FIXATIONS_L] = $fix_l;
      $dataset[DataSetField::FIXATIONS_R] = $fix_r;

      return true;
   }

  /**
   * Function that computes all necessary values at a trial level
   * It also calls the process Data Set function for each of it's datasets. 
   *
   * The study is the name of the study being processed.
   */
   private function processTrial(&$trial, $study){

      if ($this->error != "") return false;

      $trial[TrialField::TRIAL_VALUES] = array();

      // Doing a complete verification of the structure would be too long and complicated. And this should have been done in the client.
      // Hence we only check the main fields. This is more a just in case, than anything else.
      $missing_fields = TrialField::listMissingFields(array_keys($trial));
      if (!empty($missing_fields)){
         $this->error = "Trial has missing field: " . implode(",",$missing_fields);
         return false;
      }            

      $trial_type = $trial[TrialField::TRIAL_TYPE];

      foreach ($trial[TrialField::DATA] as $data_set_type => $dataset){
         if (!$this->processDataSet($dataset,$study,$trial_type)){
            $this->error = "Trial " . $trial[TrialField::ID] . " had an error while processing dataset $data_set_type: " . $this->error;
            return false;
         }

         // Storing the dataset with the new information. 
         $trial[TrialField::DATA][$data_set_type] = $dataset;
      }

      // Specific NBack computations
      if (($study == Study::NBACKRT) || ($study == Study::NBACKMS)){

         $hit_boxes = $this->processing_parameters[ProcessingParameter::NBACK_HITBOXES];
   
         //echo "L Fixations Trial ID: " . $trial[TrialFields::ID] . "\n";
         $result = TargetHitSearcher::computeFixationBasedNBackValues($trial,$hit_boxes ,$trial_type,$study);
         if ($result[TargetHitSearcher::COMPUTE_RET_ERROR] != ""){
            $this->error = "While computing NBack Fixation Values: " . $result[TargetHitSearcher::COMPUTE_RET_ERROR] . "\n";
            return false;
         }
   
         //var_dump($result);
         $trial_values[TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_L] = $result[DataSetField::FIXATIONS_L][TargetHitSearcher::COMPUTE_RET_SEQ_COMPLETE];   
         $trial_values[TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_R] = $result[DataSetField::FIXATIONS_R][TargetHitSearcher::COMPUTE_RET_SEQ_COMPLETE];
         $trial[TrialField::TRIAL_VALUES] = $trial_values;
      }
      
      // // Specific Binding computations
      // else if (($study == Study::BINDING_BC) || ($study == Study::BINDING_UC)){
   
      //    //echo "TrialID: " . $trial[TrialFields::ID] . ". Trial Type |" . $trial_type . "|. Response: |" . $trial[TrialFields::RESPONSE] . "|\n";
      //    $binding_score = array();
      //    $response = trim($trial[TrialField::RESPONSE]);
   
      //    if (str_contains($trial[TrialField::ID],"test")){
      //       if ($trial_type == $response) $binding_score[BindingScore::TEST_CORRECT]++;
      //       else $binding_score[BindingScore::TEST_WRONG]++;
      //    }
      //    else{
      //       if ($trial_type == $response) $binding_score[BindingScore::CORRECT]++;
      //       else $binding_score[BindingScore::WRONG]++;
      //    }
   
      // }      
      return true;

   }

   /**
    * The overarching function that calls all others and computes all study, trial and data set related values.
    * The expcted input is th trial_list field of a given study as well as the standarized study name. 
    */

   public function processStudy(&$trial_list, $study){

      if ($this->error != "") return false;

      if (count($trial_list) == 0){
         $this->error = "Study $study has an empty trial list";
         return false;
      }

      // Processing each trial. 
      for ($i = 0; $i < count($trial_list); $i++) {
         $trial = $trial_list[$i];
         if (!$this->processTrial($trial,$study)){
            $this->error = "While processing trial $i for study $study: " . $this->error;
            return false;
         }
         $trial_list[$i] = $trial;
      }

      // We now compute the duration of the trial.
      $study_start_timestamp = -1;
      $end_study_timestamp = -1;
      
      // Values computed at a study level. 
      $this->study_values = array();

      $first_trial = $trial_list[0];
      $last_trial  = $trial_list[count($trial_list)-1];      
      $datasets = array_keys($first_trial[TrialField::DATA]);
      
      // Getting the smallest timestamp. 
      foreach ($datasets as $dataset){         
         if (count($first_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA]) == 0)  continue;
         
         if (($study_start_timestamp == -1) || ($study_start_timestamp > $first_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA][0][DataVectorField::TIMESTAMP])) {
            $study_start_timestamp = $first_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA][0][DataVectorField::TIMESTAMP];
         }
      }

      // Getting the largest time stamp
      foreach ($datasets as $dataset){         
         $cnt = count($last_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA]);
         
         if ($cnt == 0)  continue;

         if (($end_study_timestamp == -1) || ($end_study_timestamp < $last_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA][$cnt-1][DataVectorField::TIMESTAMP])) {
            $end_study_timestamp = $last_trial[TrialField::DATA][$dataset][DataSetField::RAW_DATA][$cnt-1][DataVectorField::TIMESTAMP];
         }
      }

      $this->study_values[StudyField::DURATION] = $end_study_timestamp - $study_start_timestamp;

      // Specific Binding computations
      if (($study == Study::BINDING_BC) || ($study == Study::BINDING_UC)){
   
         //echo "TrialID: " . $trial[TrialFields::ID] . ". Trial Type |" . $trial_type . "|. Response: |" . $trial[TrialFields::RESPONSE] . "|\n";
         $binding_score = array();
         $binding_score[BindingScore::TEST_CORRECT] = 0;
         $binding_score[BindingScore::TEST_WRONG] = 0;
         $binding_score[BindingScore::CORRECT] = 0;
         $binding_score[BindingScore::WRONG] = 0;

         for ($i = 0; $i < count($trial_list); $i++) {

            $trial = $trial_list[$i];
            $trial_type = $trial[TrialField::TRIAL_TYPE];
            $response = trim($trial[TrialField::RESPONSE]);
   
            if (str_contains($trial[TrialField::ID], "test")) {
                if ($trial_type == $response) {
                    $binding_score[BindingScore::TEST_CORRECT]++;
                } else {
                    $binding_score[BindingScore::TEST_WRONG]++;
                }
            } else {
                if ($trial_type == $response) {
                    $binding_score[BindingScore::CORRECT]++;
                } else {
                    $binding_score[BindingScore::WRONG]++;
                }
            }
         }

         $this->study_values[StudyField::BINDING_SCORE] = $binding_score;
   
      }      

      return true;
   }

}



?>