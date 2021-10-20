<?php

class QCComputations {

   private $vmdata;
   private $output_file;
   private $min_diff;
   private $max_diff;

   function __construct($input_file){
      $this->vmdata = json_decode(file_get_contents($input_file),true);


      // Getting parameters for glitch computation. 
      $this->max_diff = $this->vmdata["qc_parameters"]["max_successive_timestamp_difference"];
      $this->min_diff = $this->vmdata["qc_parameters"]["min_successive_timestamp_difference"];
  

      // Generating the output file name
      $pathinfo = pathinfo($input_file);
      $dir = $pathinfo["dirname"];
      $basename = $pathinfo["filename"];
      $this->output_file = "$dir/$basename" . "_qc_corrected.json";

   }

   function qcCompute(){
      
      // Iterating over each study, then over the trial list of the study then over each dataset within each trial. 

      // Getting the target sampling frequency
      $sampling_frequency = $this->vmdata["processing_parameters"]["sample_frequency"];

      foreach ($this->vmdata["studies"] as $study_name => $study_data){

         $avg_f_per_trial = array();
         $fix_per_trial = array();
         $glitches_per_trial = array();
         $points_per_trial = array();
         $eye = $study_data["study_configuration"]["valid_eye"];

         $original_qc = $study_data["quality_control"];

         $qc_params = $this->vmdata["qc_parameters"];
         $qc_study_params = $qc_params[$study_name];

         $min_points_per_trial = $qc_study_params["min_points_per_trial"];
         $threshold_dp         = $qc_study_params["threshold_valid_num_datapoints"];
         $min_fix_per_trial    = $qc_study_params["min_fixations_per_trial"];
         $threshold_fix        = $qc_study_params["threshold_valid_num_fixations"];         
         $over_frequency       = $sampling_frequency*(1+$qc_params["allowed_f_plus_variation"]/100);
         $under_frequency      = $sampling_frequency*(1-$qc_params["allowed_f_minus_variation"]/100);         
         $threshold_f          = $qc_params["threshold_trials_with_valid_f"];

         $qc_datapoint_index = 0;
         $qc_fixation_index  = 0;
         $qc_f_index = 0;
         $num_trials = 0;

         foreach ($study_data["trial_list"] as $trial){         

            $avg_f_acc = 0;
            $avg_f_counter = 0;
            $fix_counter = 0;
            $glitch_counter = 0;
            $point_counter = 0;
            $num_trials++;

            foreach ($trial["data"] as $dataset_name => $dataset){
               $raw_data = $dataset["raw_data"];
               
               if ($eye == "left"){
                  $fix_counter = $fix_counter + count($dataset["fixations_l"]);
               }
               else{
                  $fix_counter = $fix_counter + count($dataset["fixations_r"]);
               }

               $point_counter = $point_counter + count($raw_data);
               
               $ts = $raw_data[0]["ts"];               
               for ($i = 1; $i < count($raw_data); $i++){

                  $diff = $raw_data[$i]["ts"] - $ts;
                  $avg_f_acc = $avg_f_acc + $diff;
                  $avg_f_counter++;
                  $ts = $raw_data[$i]["ts"];

                  if (($diff > $this->max_diff) || ($diff < $this->min_diff)){
                     // This is a glitch.
                     $glitch_counter++;
                  }                  

               }

            }

            $avg_f = 1000*$avg_f_counter/$avg_f_acc;
            $avg_f_per_trial[] = $avg_f;
            $fix_per_trial[] = $fix_counter;
            $glitches_per_trial[] = $glitch_counter;
            $points_per_trial[] = $point_counter;

            if ($point_counter >= $min_points_per_trial ){
               $qc_datapoint_index++;
            }

            if ($fix_counter >= $min_fix_per_trial){
               $qc_fixation_index++;
            }

            if (($avg_f <= $over_frequency) && ($avg_f >= $under_frequency)){
               $qc_f_index++;
            }
            
         }

         $qc_datapoint_index = $qc_datapoint_index*100/$num_trials;
         $qc_fixation_index = $qc_fixation_index*100/$num_trials;
         $qc_f_index = $qc_f_index*100/$num_trials;

         $qc["avg_freq_per_trial"] = $avg_f_per_trial;
         $qc["fixations_per_trial"] = $fix_per_trial;         
         $qc["glitches_per_trial"] = $glitches_per_trial;
         $qc["number_of_datasets"] = $original_qc["number_of_datasets"];
         $qc["points_per_trial"]   = $points_per_trial;

         $qc["qc_data_point_index"] = $qc_datapoint_index;
         $qc["qc_data_point_index_ok"] = ($qc_datapoint_index > $threshold_dp);
         $qc["qc_fix_index"] = $qc_fixation_index;
         $qc["qc_fix_index_ok"] = ($qc_fixation_index > $threshold_fix);
         $qc["qc_freq_index"] =  $qc_f_index;
         $qc["qc_freq_index_ok"] = ($qc_f_index > $threshold_f);

         $study_data["quality_control"] = $qc;                  
         $this->vmdata["studies"][$study_name] = $study_data;

      }

      // Storing the results in the output file
      $fid = fopen($this->output_file,"w");
      fwrite($fid,json_encode($this->vmdata,JSON_PRETTY_PRINT));
      fclose($fid);

      return $this->output_file;

   }





}

?>