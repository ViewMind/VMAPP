<?php


include_once (__DIR__ . "/../ViewMindDataContainer.php");


/**
 * @ Generates the standard CSV for a Reading study. Returns the error message or an empty string if all went well. 
 */
function readingCSV(ViewMindDataContainer $input, $output_file){

   $output_file = $output_file[Study::READING];
   
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
   $data_set_type = DataSetType::UNIQUE;      

   if (!$input->setRawDataAccessPathForStudy(Study::GONOGO)){
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
   $patient_age      = $input->getSubjectDataValue(SubjectField::AGE);

   $n_trials = $input->getNumberOfTrials();

   for ($t = 0; $t < $n_trials; $t++){

      $trial = $input->getTrial($t);

      if ($input->getError() != "") {
         return "Error getting trial $t: " . $input->getError();
      }

      $sentence = explode(" ",$trial[TrialField::TRIAL_TYPE]);
      $sentence_length = 0;

      foreach ($sentence as $token){
         $token = trim($token);
         if ($token != "") $sentence_length++;
      }
      
      $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_L];

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
         $row[] = $trial[TrialField::ID];
         $row[] = $trial[TrialField::ID];
         $row[] = $i+1;
         $row[] = $fix[$i][FixationVectorField::X];
         $row[] = $fix[$i][FixationVectorField::WORD];
         $row[] = $fix[$i][FixationVectorField::CHAR];
         $row[] = $fix[$i][FixationVectorField::DURATION];
         $row[] = $sentence_length;
         $row[] = $eye_left_csv;   
         $row[] = $fix[$i][FixationVectorField::PUPIL];
         $row[] = $fix[$i][FixationVectorField::ZERO_PUPIL];
         $row[] = $fix[$i][FixationVectorField::SAC_AMP];
         $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
         $row[] = $nfl;                       
         $row[] = $fix[$i][FixationVectorField::Y];
         $csv_rows[] = implode(",",$row);
      }
      //////////////////////// Now for the rigth side
      $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_R];
      
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
         $row[] = $trial[TrialField::ID];
         $row[] = $trial[TrialField::ID];
         $row[] = $i+1;
         $row[] = $fix[$i][FixationVectorField::X];
         $row[] = $fix[$i][FixationVectorField::WORD];
         $row[] = $fix[$i][FixationVectorField::CHAR];
         $row[] = $fix[$i][FixationVectorField::DURATION];
         $row[] = $sentence_length;
         $row[] = $eye_right_csv;   
         $row[] = $fix[$i][FixationVectorField::PUPIL];
         $row[] = $fix[$i][FixationVectorField::ZERO_PUPIL];
         $row[] = $fix[$i][FixationVectorField::SAC_AMP];
         $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
         $row[] = $nfR;                       
         $row[] = $fix[$i][FixationVectorField::Y];
         $csv_rows[] = implode(",",$row);
      }         
   }


   $fid = fopen($output_file,"w+");
   fwrite($fid,implode("\n",$csv_rows));
   fclose($fid);
   return "";
}
?>