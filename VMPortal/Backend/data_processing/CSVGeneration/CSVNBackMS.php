<?php

include_once(__DIR__ . "/../ViewMindDataContainer.php");

/**
 * @ Generates the standard CSV for a NBackMS study. Returns the error message or an empty string if all went well. 
 */
function nbackmsCSV(ViewMindDataContainer $input, $output_file){

   $output_file = $output_file[Study::NBACKMS];
   
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
   $img_num_map[DataSetType::ENCODING_1]  = "1";
   $img_num_map[DataSetType::ENCODING_2]  = "2";
   $img_num_map[DataSetType::ENCODING_3]  = "3";
   $img_num_map[DataSetType::RETRIEVAL_1] = "4";
   $img_num_map[DataSetType::RETRIEVAL_2] = "4";
   $img_num_map[DataSetType::RETRIEVAL_3] = "4";
   
   // The order in which data sets need to be processed. 
   $data_set_order = array_keys($img_num_map);
   $csv_rows_l = array();      
   $csv_rows_r = array();      

   
   if (!$input->setRawDataAccessPathForStudy(Study::NBACKMS)){
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

   $n_trials = $input->getNumberOfTrials();
   
   
   for ($t = 0; $t < $n_trials; $t++) {
      
      $trial = $input->getTrial($t);
      
      if ($input->getError() != "") {
          return "Error getting trial $t: " . $input->getError();
      }
      
      $trial_sequence = $trial[TrialField::TRIAL_TYPE];
      $parts = explode(" ",$trial_sequence);
      $trial_sequence = implode("|",$parts);
      $l_fix_seq_completed = $trial[TrialField::TRIAL_VALUES][TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_L];
      $r_fix_seq_completed = $trial[TrialField::TRIAL_VALUES][TrialComputedValueTypes::COMPLETE_SEQUENCE_FIXATION_R];
      
      
      foreach ($data_set_order as $data_set_type) {
      
         $img_num = $img_num_map[$data_set_type];
      
         $fix =  $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_L];
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }
      
         // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
         $data_set_computed_values = $trial[TrialField::DATA][$data_set_type][DataSetField::DATA_SET_VALUES];
      
         // The number of fixations in the data set is a value.
      
         $nfl = count($fix);
         for ($i = 0; $i < $nfl; $i++) {
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = $trial[TrialField::ID];
            $row[] = $img_num;
            $row[] = $fix[$i][FixationVectorField::TARGET_HIT];
            $row[] = $fix[$i][FixationVectorField::IS_IN];
            $row[] = $fix[$i][FixationVectorField::NBACK];
            $row[] = $trial_sequence;
            if ($l_fix_seq_completed == $i) $row[] = 1;
            else $row[] = 0;
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_left_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_L];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            if ($data_set_type == DataSetType::RETRIEVAL_1) {
               // Response time is only used for retrival data set. 
               $row[] = $data_set_computed_values[DataSetComputedValues::DURATION];
            }
            else $row[] = "N/A";
            $csv_rows_l[] = implode(",",$row);
         }
         
         
         $fix =  $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_R];
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }
         
         // The number of fixations in the data set is a value.
         $nfR = count($fix);
         for ($i = 0; $i < $nfR; $i++) {
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = $trial[TrialField::ID];
            $row[] = $img_num;
            $row[] = $fix[$i][FixationVectorField::TARGET_HIT];
            $row[] = $fix[$i][FixationVectorField::IS_IN];
            $row[] = $fix[$i][FixationVectorField::NBACK];
            $row[] = $trial_sequence;
            if ($r_fix_seq_completed == $i) $row[] = 1;
            else $row[] = 0;
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_right_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_R];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            if ($data_set_type == DataSetType::RETRIEVAL_1) {
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


?>