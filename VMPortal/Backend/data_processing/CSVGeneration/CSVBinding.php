<?php

include_once(__DIR__ . "/../ViewMindDataContainer.php");

/**
 * @ Generates the standard CSV for a Binding Study study. Returns the error message or an empty string if all went well. 
 */   
function bindingCSV(ViewMindDataContainer $input, $output_file){
   // Binding is a multi study file so it's necessary to call it for both parts. 
   foreach ($output_file as $study => $csv_name){
      $ans = bindingSingleCSV($input,$csv_name,$study);
      if ($ans != "") return $ans;
   }
}

function bindingSingleCSV(ViewMindDataContainer $input, $output_file, $study){

   $headers = [
   "placeholder_id",
   "age",
   "suj",
   "trial_id",
   "is_trial",
   "trial_name",
   "trial_type",
   "response",
   "dur",
   "ojoDI",
   "blink",
   "amp_sacada",
   "pupila",
   "gaze",
   "nf",
   "fixX",
   "fixY",
   "targetSide",
   "tX1",
   "tY1",
   "tX2",
   "tY2",
   "tX3",
   "tY3"           ,
   "fixStart",
   "fixEnd",
   "fixMid",
   "timeline",
   "score"];

   // Creating an image number map to reproduce datasets just as in legacy CSV. 
   $is_trial_map[DataSetType::ENCODING_1]   = "0";
   $is_trial_map[DataSetType::RETRIEVAL_1]  = "1";

   // Map of strings for compatibility's sake. 
   $type_str_map["S"] = "same";
   $type_str_map["D"] = "different";

   // The order in which data sets need to be processed. 
   $data_set_order = array_keys($is_trial_map);
   $csv_rows = array();
   $csv_rows[] = implode(",",$headers);

   if (!$input->setRawDataAccessPathForStudy($study)){
      return "Error setting the active study in the procssed data container: " . $input->getError();
   }
   
   $valid_eye = $input->getValidEyeForCurrentStudy();

   if ($valid_eye == ""){
      return "Error getting valid eye: " . $input->getError();
   }
   if ($valid_eye == Eye::LEFT){
      $eye_left_csv  = "1";
      $eye_right_csv = "0";
   }
   else{
      $eye_left_csv  = "0";
      $eye_right_csv = "1";
   }
   
   $subject_idenfier = $input->getSubjectDataValue(SubjectField::LOCAL_ID);
   $subject_age      = $input->getSubjectDataValue(SubjectField::AGE);
   
   $n_trials = $input->getNumberOfTrials();
   
   $binding_score  = $input->getStudyField(StudyField::BINDING_SCORE);
      
   if ($input->getError() != ""){
      return "Could not get binding score. Error: " . $input->getError();
   }

   //var_dump($binding_score);
   $score = $binding_score[BindingScore::CORRECT];

   for ($t = 0; $t < $n_trials; $t++) {
   
      $trial = $input->getTrial($t);
   
      if ($input->getError() != "") {
          return "Error getting trial $t: " . $input->getError();
      }
   
      foreach ($data_set_order as $data_set_type) {
         
         $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_L];
         
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }
         
         // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
         $data_set_computed_values = $trial[TrialField::DATA][$data_set_type][DataSetField::DATA_SET_VALUES];
         
         // The number of fixations in the data set is a value.
         $nfl = count($fix);
         
         // The trial type: same or differnt
         $trial_type = "N/A";
         
         if ($data_set_type == DataSetType::RETRIEVAL_1){
            $trial_type = $type_str_map[$trial[TrialField::TRIAL_TYPE]];
         }
         
         // Is Trial: if it's show/retrieval or encoding.
         $is_trial = $is_trial_map[$data_set_type];
         
         // The trial number
         $parts = explode("-",$trial[TrialField::ID]);
         
         $trial_number_from_name = intval($parts[0]);
         
         if ($data_set_type == DataSetType::RETRIEVAL_1) $response = $trial[TrialField::RESPONSE];
         else $response = "N/A";
         
         for ($i = 0; $i < $nfl; $i++) {
            $row = array();
            $row[] = "N/A";
            $row[] = $subject_age;
            $row[] = $subject_idenfier;
            $row[] = $trial_number_from_name;
            $row[] = $is_trial;
            $row[] = $trial[TrialField::ID];
            $row[] = $trial_type;
            $row[] = $response;
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_left_csv;
            $row[] = $fix[$i][FixationVectorField::ZERO_PUPIL];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            $row[] = $fix[$i][FixationVectorField::PUPIL];
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
            $row[] = $nfl;
            $row[] = $fix[$i][FixationVectorField::X];
            $row[] = $fix[$i][FixationVectorField::Y];
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = $fix[$i][FixationVectorField::START_TIME];
            $row[] = $fix[$i][FixationVectorField::END_TIME];
            $row[] = $fix[$i][FixationVectorField::TIME];
            $row[] = $fix[$i][FixationVectorField::TIMELINE];
            $row[] = $score;
            $csv_rows[] = implode(",",$row);
         }

         $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_R];
         
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }
         
         // The number of fixations in the data set is a value.
         $nfR = count($fix);
         
         for ($i = 0; $i < $nfR; $i++) {
            $row = array();
            $row[] = "N/A";
            $row[] = $subject_age;
            $row[] = $subject_idenfier;
            $row[] = $trial_number_from_name;
            $row[] = $is_trial;
            $row[] = $trial[TrialField::ID];
            $row[] = $trial_type;
            $row[] = $response;
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_right_csv;
            $row[] = $fix[$i][FixationVectorField::ZERO_PUPIL];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            $row[] = $fix[$i][FixationVectorField::PUPIL];
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
            $row[] = $nfR;
            $row[] = $fix[$i][FixationVectorField::X];
            $row[] = $fix[$i][FixationVectorField::Y];
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = 0;
            $row[] = $fix[$i][FixationVectorField::START_TIME];
            $row[] = $fix[$i][FixationVectorField::END_TIME];
            $row[] = $fix[$i][FixationVectorField::TIME];
            $row[] = $fix[$i][FixationVectorField::TIMELINE];
            $row[] = $score;
            $csv_rows[] = implode(",",$row);
         }           
      }
   }
   $fid = fopen($output_file,"w+");
   fwrite($fid,implode("\n",$csv_rows));
   fclose($fid);
   return "";
   
}    

?>