<?php
   
   include_once(__DIR__ . "/../ViewMindDataContainer.php");

   /**
    * @ Generates the standard CSV for a Go No-Go Study study. Returns the error message or an empty string if all went well. 
    */   

    function gonogoCSV(ViewMindDataContainer $input, $output_file){

      // Retrieving the output file from the associative array
      $output_file = $output_file[Study::GONOGO];

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
      "trial_type",
      "target_visualization_delay"];

      // The order in which data sets need to be processed. 
      $data_set_type = DataSetType::UNIQUE;

      $csv_rows_l = array();
      $csv_rows_r = array();


      // Mapping the trial descriptions back to numbers
      $map_trial_desc_to_values["R<-"] = 0; 
      $map_trial_desc_to_values["G<-"] = 1;
      $map_trial_desc_to_values["R->"] = 2;
      $map_trial_desc_to_values["G->"] = 3;      

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
      $subject_age      = $input->getSubjectDataValue(SubjectField::AGE);

      $n_trials = $input->getNumberOfTrials();

      $total_exp_time = $input->getStudyField(StudyField::DURATION);
      if ($total_exp_time == NULL){
         return "Could not get response time. Error: " . $input->getError();
      }      

      

      for ($t = 0; $t < $n_trials; $t++) {

         $trial = $input->getTrial($t);
         if ($input->getError() != "") {
             return "Error getting trial $t: " . $input->getError();
         }

         $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_L];
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();
         }

         // Error check is NOT necessary here as the check in the previous get includes the same thing checked here.
         $data_set_computed_values = $trial[TrialField::DATA][$data_set_type][DataSetField::DATA_SET_VALUES];

         // Getting the start of trial as the time stamp of the first raw eye_data. 
         if (count($trial[TrialField::DATA][$data_set_type][DataSetField::RAW_DATA]) > 0)
            $trial_start_time = $trial[TrialField::DATA][$data_set_type][DataSetField::RAW_DATA][0][DataVectorField::TIMESTAMP];
         else // This check is simply to avoid the warning, as if there is no raw data there won't be any fixations and hence no rows in the final CSV. 
            $trial_start_time = 0;

         // The number of fixations in the data set is a value.
         $nfl = count($fix);

         $arrow_type = "R";
         if (str_contains($trial[TrialField::TRIAL_TYPE],"G")) $arrow_type = "G";

         $trial_type = $map_trial_desc_to_values[$trial[TrialField::TRIAL_TYPE]];

         for ($i = 0; $i < $nfl; $i++){
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = intval($trial[TrialField::ID]);
            $row[] = $fix[$i][FixationVectorField::IS_IN];
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_left_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_L];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::RESPONSE_TIME_L]; 
            $row[] = $fix[$i][FixationVectorField::PUPIL];
            $row[] = $nfl;
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_L];
            $row[] = $data_set_computed_values[DataSetComputedValues::FIX_IN_CENTER_L]; 
            $row[] = $total_exp_time;
            $row[] = $arrow_type;
            $row[] = $trial_type;

            $target_visualization_delay = 0;
            if ($fix[$i][FixationVectorField::IS_IN] == 1){ // This fixation is inside the desired target. 
               $target_visualization_delay = $fix[$i][FixationVectorField::START_TIME] - $trial_start_time;
            }
            $row[] = $target_visualization_delay;

            $csv_rows_l[] = implode(",",$row);
         }

         $fix = $trial[TrialField::DATA][$data_set_type][DataSetField::FIXATIONS_R];
         if ($input->getError() != "") {
             return "Error getting fixation list from trial $t: " . $input->getError();             
         }

         $nfr = count($fix);
         for ($i = 0; $i < $nfr; $i++){
            $row = array();
            $row[] = $subject_idenfier;
            $row[] = intval($trial[TrialField::ID]);
            $row[] = $fix[$i][FixationVectorField::IS_IN];
            $row[] = $fix[$i][FixationVectorField::DURATION];
            $row[] = $eye_right_csv;
            $row[] = $data_set_computed_values[DataSetComputedValues::SAC_LAT_R];
            $row[] = $fix[$i][FixationVectorField::SAC_AMP];
            $row[] = $data_set_computed_values[DataSetComputedValues::RESPONSE_TIME_R]; 
            $row[] = $fix[$i][FixationVectorField::PUPIL];
            $row[] = $nfr;
            $row[] = $data_set_computed_values[DataSetComputedValues::GAZE_R];
            $row[] = $data_set_computed_values[DataSetComputedValues::FIX_IN_CENTER_R]; 
            $row[] = $total_exp_time;
            $row[] = $arrow_type;
            $row[] = $trial_type;
            
            $target_visualization_delay = 0;
            if ($fix[$i][FixationVectorField::IS_IN] == 1){ // This fixation is inside the desired target. 
               $target_visualization_delay = $fix[$i][FixationVectorField::START_TIME] - $trial_start_time;
            }
            $row[] = $target_visualization_delay;

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


?>