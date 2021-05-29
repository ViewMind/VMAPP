<?php

include_once ("ProcessedDataContainer.php");

abstract class CSVGenerator{


   /**
    * @ Generates the standard CSV for a Reading study. Returns the error message or an empty string if all went well. 
    */

   static function readingCSV(ProcessedDataContainer $input, $output_file){
      
      $headers = [    
      "placeholder_id",
      "age",
      "suj",
      "sujnum",
      "oracion",
      "trial",
      "fixn",
      "screenpos",
      "wn",
      "pos",
      "dur",
      "longoracion",
      "ojoDI",
      "pupila",
      "blink",
      "amp_sac",
      "gaze",
      "nf",
      "fixY" ];
      
      $csv_rows = array();      
      $csv_rows[] = implode(",",$headers);

      $data_set_type = DataSetTypes::UNIQUE;      
      $trial_list_type = TrialListTypes::UNIQUE;

      if (!$input->setActiveStudyAndTrialListType(StudyTypes::READING, $trial_list_type)){
         return "Error setting the active study in the procssed data container: " . $input->getError();
      }

      $valid_eye = $input->getValidEyeForStudy();
      if ($valid_eye == ""){
         return "Error getting valid eye: " . $input->getError();
      }

      if ($valid_eye == EyeType::LEFT){
         $eye_left_csv  = "1";
         $eye_right_csv = "0";
      }
      else{
         $eye_left_csv  = "0";
         $eye_right_csv = "1";
      }

      $patient_age = $input->getSubjectAge();
      $subject_idenfier = $input->getSubjectID();

      $n_trials = $input->getNumberOfTrials();

      for ($t = 0; $t < $n_trials; $t++){

         $trial = $input->getTrial($t);
         if ($input->getError() != "") {
            return "Error getting trial $t: " . $input->getError();
         }

         $sentence = explode(" ",$trial[TrialFields::SENTENCE]);
         $sentence_length = 0;
         foreach ($sentence as $token){
            $token = trim($token);
            if ($token != "") $sentence_length++;
         }

         $fix = $input->getFixationListsFromTrial($trial,$data_set_type,EyeType::LEFT);
         if ($input->getError() != "") {
            return "Error getting fixation list from trial $t: " . $input->getError();
         }

         // Error check is NOT necessary here as the check in the previous get includes the same thing checked here. 
         $data_set_computed_values = $input->getValuesForDataSetFromTrial($trial,$data_set_type);

         // The number of fixations in the data set is a value. 
         $nfl = count($fix);

         for ($i = 0; $i < $nfl; $i++){
            $row = array();
            $row[] =  "N/A";
            $row[] = $patient_age;                
            $row[] = $subject_idenfier;           
            $row[] = "1";
            $row[] = $trial[TrialFields::ID];
            $row[] = $trial[TrialFields::ID];
            $row[] = $i+1;
            $row[] = $fix[$i][FixationVectorFields::X];
            $row[] = $fix[$i][FixationVectorFields::WORD];
            $row[] = $fix[$i][FixationVectorFields::CHAR];
            $row[] = $fix[$i][FixationVectorFields::DURATION];
            $row[] = $sentence_length;
            $row[] = $eye_left_csv;   
            $row[] = $fix[$i][FixationVectorFields::PUPIL];
            $row[] = $fix[$i][FixationVectorFields::ZERO_PUPIL];
            $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
            $row[] = $nfl;                       
            $row[] = $fix[$i][FixationVectorFields::Y];
            $csv_rows[] = implode(",",$row);
         }

         //////////////////////// Now for the rigth side
         $fix = $input->getFixationListsFromTrial($trial,$data_set_type,EyeType::RIGHT);
         if ($input->getError() != "") {
            return "Error getting fixation list from trial $t: " . $input->getError();
         }

         // The number of fixations in the data set is a value. 
         $nfR = count($fix);

         for ($i = 0; $i < $nfR; $i++){
            $row = [];
            $row[] =  "N/A";
            $row[] = $patient_age;                
            $row[] = $subject_idenfier;           
            $row[] = "1";
            $row[] = $trial[TrialFields::ID];
            $row[] = $trial[TrialFields::ID];
            $row[] = $i+1;
            $row[] = $fix[$i][FixationVectorFields::X];
            $row[] = $fix[$i][FixationVectorFields::WORD];
            $row[] = $fix[$i][FixationVectorFields::CHAR];
            $row[] = $fix[$i][FixationVectorFields::DURATION];
            $row[] = $sentence_length;
            $row[] = $eye_right_csv;   
            $row[] = $fix[$i][FixationVectorFields::PUPIL];
            $row[] = $fix[$i][FixationVectorFields::ZERO_PUPIL];
            $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
            $row[] = $nfR;                       
            $row[] = $fix[$i][FixationVectorFields::Y];
            $csv_rows[] = implode(",",$row);
         }         


      }


      $fid = fopen($output_file,"w+");
      fwrite($fid,implode("\n",$csv_rows));
      fclose($fid);

      return "";

   }

   /**
    * @ Generates the standard CSV for a NBackRT study. Returns the error message or an empty string if all went well. 
    */
   static function nbackrtCSV(ProcessedDataContainer $input, $output_file){

      $headers = [
         "suj"        ,  
         "idtrial"    ,  
         "imgnum"     ,  
         "target"     ,  
         "target_hit" ,  
         "nback"      ,  
         "trial_seq"  ,  
         "seq_comp"   ,  
         "dur"        ,  
         "ojoDI"      ,  
         "latSac"     ,  
         "amp_sacada" ,  
         "resp_time" ,            
      ];

      // Creating an image number map to reproduce datasets just as in legacy CSV. 
      $img_num_map[DataSetTypes::ENCODING_1]  = "1";
      $img_num_map[DataSetTypes::ENCODING_2]  = "2";
      $img_num_map[DataSetTypes::ENCODING_3]  = "3";
      $img_num_map[DataSetTypes::RETRIEVAL_1] = "4";

      // The order in which data sets need to be processed. 
      $data_set_order = array_keys($img_num_map);

      $csv_rows_l = array();      
      $csv_rows_r = array();      

      $trial_list_type = TrialListTypes::UNIQUE;

      if (!$input->setActiveStudyAndTrialListType(StudyTypes::NBACKRT, $trial_list_type)){
         return "Error setting the active study in the procssed data container: " . $input->getError();
      }

      $valid_eye = $input->getValidEyeForStudy();
      if ($valid_eye == ""){
         return "Error getting valid eye: " . $input->getError();
      }

      if ($valid_eye == EyeType::LEFT){
         $eye_left_csv  = "1";
         $eye_right_csv = "0";
      }
      else{
         $eye_left_csv  = "0";
         $eye_right_csv = "1";
      }

      $subject_idenfier = $input->getSubjectID();

      $n_trials = $input->getNumberOfTrials();

      for ($t = 0; $t < $n_trials; $t++) {

         $trial = $input->getTrial($t);
         if ($input->getError() != "") {
             return "Error getting trial $t: " . $input->getError();
         }

         $trial_sequence = $trial[TrialFields::TRIAL_TYPE];
         $parts = explode(" ",$trial_sequence);
         $trial_sequence = implode("|",$parts);

         $l_fix_seq_completed = $trial[TrialFields::TRIAL_VALUES][TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_L];
         $r_fix_seq_completed = $trial[TrialFields::TRIAL_VALUES][TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_R];

         foreach ($data_set_order as $data_set_type) {

            $img_num = $img_num_map[$data_set_type];

            $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::LEFT);
            if ($input->getError() != "") {
                return "Error getting fixation list from trial $t: " . $input->getError();
            }

            // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
            $data_set_computed_values = $input->getValuesForDataSetFromTrial($trial, $data_set_type);

            // The number of fixations in the data set is a value.
            $nfl = count($fix);

            for ($i = 0; $i < $nfl; $i++) {
               $row = array();
               $row[] = $subject_idenfier;
               $row[] = $trial[TrialFields::ID];
               $row[] = $img_num;
               $row[] = $fix[$i][FixationVectorFields::TARGET_HIT];
               $row[] = $fix[$i][FixationVectorFields::IS_IN];
               $row[] = $fix[$i][FixationVectorFields::NBACK];
               $row[] = $trial_sequence;
               if ($l_fix_seq_completed == $i) $row[] = 1;
               else $row[] = 0;
               $row[] = $fix[$i][FixationVectorFields::DURATION];
               $row[] = $eye_left_csv;
               $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_L];
               $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
               if ($data_set_type == DataSetTypes::RETRIEVAL_1) {
                  // Response time is only used for retrival data set. 
                  $row[] = $data_set_computed_values[DataSetComputedValues::DURATION];
               }
               else $row[] = "N/A";
               $csv_rows_l[] = implode(",",$row);
            }

            $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::RIGHT);
            if ($input->getError() != "") {
                return "Error getting fixation list from trial $t: " . $input->getError();
            }

            // The number of fixations in the data set is a value.
            $nfR = count($fix);
            for ($i = 0; $i < $nfR; $i++) {
               $row = array();
               $row[] = $subject_idenfier;
               $row[] = $trial[TrialFields::ID];
               $row[] = $img_num;
               $row[] = $fix[$i][FixationVectorFields::TARGET_HIT];
               $row[] = $fix[$i][FixationVectorFields::IS_IN];
               $row[] = $fix[$i][FixationVectorFields::NBACK];
               $row[] = $trial_sequence;
               if ($r_fix_seq_completed == $i) $row[] = 1;
               else $row[] = 0;
               $row[] = $fix[$i][FixationVectorFields::DURATION];
               $row[] = $eye_right_csv;
               $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_R];
               $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
               if ($data_set_type == DataSetTypes::RETRIEVAL_1) {
                  // Response time is only used for retrival data set. 
                  $row[] = $data_set_computed_values[DataSetComputedValues::DURATION];
               }
               else $row[] = "N/A";
               $csv_rows_r[] = implode(",",$row);
            }            


         }


      }

      $fid = fopen($output_file,"w+");
      fwrite($fid,implode(",",$headers) . "\n");
      fwrite($fid,implode("\n",$csv_rows_r) . "\n");
      fwrite($fid,implode("\n",$csv_rows_l));
      fclose($fid);

      return "";
      
   }

   /**
    * @ Generates the standard CSV for a Binding Study study. Returns the error message or an empty string if all went well. 
    */   

   static function bindingCSV(ProcessedDataContainer $input, $output_file, $trial_list_type){

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
      $is_trial_map[DataSetTypes::ENCODING_1]   = "0";
      $is_trial_map[DataSetTypes::RETRIEVAL_1]  = "1";

      // Map of strings for compatibility's sake. 
      $type_str_map["S"] = "same";
      $type_str_map["D"] = "different";

      // The order in which data sets need to be processed. 
      $data_set_order = array_keys($is_trial_map);

      $csv_rows = array();
      $csv_rows[] = implode(",",$headers);

      if (!$input->setActiveStudyAndTrialListType(StudyTypes::BINDING, $trial_list_type)){
         return "Error setting the active study in the procssed data container: " . $input->getError();
      }

      $valid_eye = $input->getValidEyeForStudy();
      if ($valid_eye == ""){
         return "Error getting valid eye: " . $input->getError();
      }

      if ($valid_eye == EyeType::LEFT){
         $eye_left_csv  = "1";
         $eye_right_csv = "0";
      }
      else{
         $eye_left_csv  = "0";
         $eye_right_csv = "1";
      }

      $subject_idenfier = $input->getSubjectID();
      $subject_age      = $input->getSubjectAge();

      $n_trials = $input->getNumberOfTrials();

      $binding_score = $input->getTrialListData(TrialListTypeDataField::BINDING_SCORE);
      if ($binding_score == NULL){
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

            $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::LEFT);
            if ($input->getError() != "") {
                return "Error getting fixation list from trial $t: " . $input->getError();
            }

            // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
            $data_set_computed_values = $input->getValuesForDataSetFromTrial($trial, $data_set_type);

            // The number of fixations in the data set is a value.
            $nfl = count($fix);

            // The trial type: same or differnt
            $trial_type = "N/A";
            if ($data_set_type == DataSetTypes::RETRIEVAL_1){
               $trial_type = $type_str_map[$trial[TrialFields::TRIAL_TYPE]];
            }

            // Is Trial: if it's show/retrieval or encoding.
            $is_trial = $is_trial_map[$data_set_type];

            // The trial number
            $parts = explode("-",$trial[TrialFields::ID]);
            $trial_number_from_name = intval($parts[0]);

            if ($data_set_type == DataSetTypes::RETRIEVAL_1) $response = $trial[TrialFields::RESPONSE];
            else $response = "N/A";

            for ($i = 0; $i < $nfl; $i++) {
               $row = array();
               $row[] = "N/A";
               $row[] = $subject_age;
               $row[] = $subject_idenfier;
               $row[] = $trial_number_from_name;
               $row[] = $is_trial;
               $row[] = $trial[TrialFields::ID];
               $row[] = $trial_type;
               $row[] = $response;
               $row[] = $fix[$i][FixationVectorFields::DURATION];
               $row[] = $eye_left_csv;
               $row[] = $fix[$i][FixationVectorFields::ZERO_PUPIL];
               $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
               $row[] = $fix[$i][FixationVectorFields::PUPIL];
               $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
               $row[] = $nfl;
               $row[] = $fix[$i][FixationVectorFields::X];
               $row[] = $fix[$i][FixationVectorFields::Y];
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = $fix[$i][FixationVectorFields::START_TIME];
               $row[] = $fix[$i][FixationVectorFields::END_TIME];
               $row[] = $fix[$i][FixationVectorFields::TIME];
               $row[] = $fix[$i][FixationVectorFields::TIMELINE];
               $row[] = $score;
               $csv_rows[] = implode(",",$row);
            }

            $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::RIGHT);
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
               $row[] = $trial[TrialFields::ID];
               $row[] = $trial_type;
               $row[] = $response;
               $row[] = $fix[$i][FixationVectorFields::DURATION];
               $row[] = $eye_right_csv;
               $row[] = $fix[$i][FixationVectorFields::ZERO_PUPIL];
               $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
               $row[] = $fix[$i][FixationVectorFields::PUPIL];
               $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
               $row[] = $nfR;
               $row[] = $fix[$i][FixationVectorFields::X];
               $row[] = $fix[$i][FixationVectorFields::Y];
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = 0;
               $row[] = $fix[$i][FixationVectorFields::START_TIME];
               $row[] = $fix[$i][FixationVectorFields::END_TIME];
               $row[] = $fix[$i][FixationVectorFields::TIME];
               $row[] = $fix[$i][FixationVectorFields::TIMELINE];
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

   /**
    * @ Generates the standard CSV for a Go No-Go Study study. Returns the error message or an empty string if all went well. 
    */   

   static function gonogoCSV(ProcessedDataContainer $input, $output_file){

      $headers = ["suj",
      "idtrial",
      "target_hit",
      "dur",
      "ojoDI",
      "latSac",
      "amp_sacada",
      "resp_time",
      "pupil_size",
      "num_trial_fix",
      "gazing",
      "num_center_fix",
      "total_study_time",
      "arrow_type",
      "trial_type"];

      // The order in which data sets need to be processed. 
      $data_set_type = DataSetTypes::UNIQUE;
      $trial_list_type = TrialListTypes::UNIQUE;

      $csv_rows_l = array();
      $csv_rows_r = array();


      // Mapping the trial descriptions back to numbers
      $map_trial_desc_to_values["R<-"] = 0; 
      $map_trial_desc_to_values["G<-"] = 1;
      $map_trial_desc_to_values["R->"] = 2;
      $map_trial_desc_to_values["G->"] = 3;      

      if (!$input->setActiveStudyAndTrialListType(StudyTypes::GONOGO, $trial_list_type)){
         return "Error setting the active study in the procssed data container: " . $input->getError();
      }

      $valid_eye = $input->getValidEyeForStudy();
      if ($valid_eye == ""){
         return "Error getting valid eye: " . $input->getError();
      }

      if ($valid_eye == EyeType::LEFT){
         $eye_left_csv  = "1";
         $eye_right_csv = "0";
      }
      else{
         $eye_left_csv  = "0";
         $eye_right_csv = "1";
      }

      $subject_idenfier = $input->getSubjectID();
      $subject_age      = $input->getSubjectAge();

      $n_trials = $input->getNumberOfTrials();

      $total_exp_time = $input->getTrialListData(TrialListTypeDataField::DURATION);
      if ($total_exp_time == NULL){
         return "Could not get response time. Error: " . $input->getError();
      }      

      for ($t = 0; $t < $n_trials; $t++) {

         $trial = $input->getTrial($t);
         if ($input->getError() != "") {
             return "Error getting trial $t: " . $input->getError();
         }

         $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::LEFT);
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }

         // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
         $data_set_computed_values = $input->getValuesForDataSetFromTrial($trial, $data_set_type);

         // The number of fixations in the data set is a value.
         $nfl = count($fix);

         $arrow_type = "R";
         if (str_contains($trial[TrialFields::TRIAL_TYPE],"G")) $arrow_type = "G";

         $trial_type = $map_trial_desc_to_values[$trial[TrialFields::TRIAL_TYPE]];

         for ($i = 0; $i < $nfl; $i++){
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = intval($trial[TrialFields::ID]);
            $row[] = $fix[$i][FixationVectorFields::IS_IN];
            $row[] = $fix[$i][FixationVectorFields::DURATION];
            $row[] = $eye_left_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_L];
            $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::RESPONSE_TIME_L]; 
            $row[] = $fix[$i][FixationVectorFields::PUPIL];
            $row[] = $nfl;
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
            $row[] = $data_set_computed_values[DataSetComputedValues::FIX_IN_CENTER_L]; 
            $row[] = $total_exp_time;
            $row[] = $arrow_type;
            $row[] = $trial_type;
            $csv_rows_l[] = implode(",",$row);
         }

         $fix = $input->getFixationListsFromTrial($trial, $data_set_type, EyeType::RIGHT);
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();             
         }

         $nfr = count($fix);
         for ($i = 0; $i < $nfr; $i++){
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = intval($trial[TrialFields::ID]);
            $row[] = $fix[$i][FixationVectorFields::IS_IN];
            $row[] = $fix[$i][FixationVectorFields::DURATION];
            $row[] = $eye_right_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_R];
            $row[] = $fix[$i][FixationVectorFields::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::RESPONSE_TIME_R]; 
            $row[] = $fix[$i][FixationVectorFields::PUPIL];
            $row[] = $nfr;
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
            $row[] = $data_set_computed_values[DataSetComputedValues::FIX_IN_CENTER_R]; 
            $row[] = $total_exp_time;
            $row[] = $arrow_type;
            $row[] = $trial_type;
            $csv_rows_r[] = implode(",",$row);
         }

      }

      $fid = fopen($output_file,"w+");
      fwrite($fid,implode(",",$headers) . "\n");
      fwrite($fid,implode("\n",$csv_rows_r) . "\n");
      fwrite($fid,implode("\n",$csv_rows_l));
      fclose($fid);
      return "";      

   }

}



?>