<?php

   /**
    * This will reprocess all sub directories on based directory using the specificied online fixation computation 
    * Using the new % of Resolution as maximum dispersion methodology.
    * It will create a complient JSON File and will then use CSVBinding to generate the respecitve CSV File.
    * 
    */


   include_once("processing_common.php");

   function getStudyConfigurationFromBindingFile($file){
      
      $ret["error"] = "";
      $path_info = pathinfo($file);
      $fname = $path_info["basename"];
      $fname_parts = explode(".",$fname);
      $fname_parts = explode("_",$fname_parts[0]);

      $n = count($fname_parts);
      
      if (($n != 10) && ($n != 7)){
         $ret["error"] = "Unknown file name format with $n parts: $fname";
         return $ret;
      }

      $ret["number_targets"] = $fname_parts[2];
      if ($fname_parts[3] == "l") {
          $ret["target_size"] = "large";
      } elseif ($fname_parts[3] == "s") {
          $ret["target_size"] = "small";
      } else {
          $ret["error"] = "Unrecognized target size: " . $fname_parts[3] . " from file $fname";
          return $ret;
      }      
      
      if ($n == 10) {

         if ($fname_parts[4] == 2) {
             $ret["valid_eye"] = "both";
         } elseif ($fname_parts[4] == 0) {
             $ret["valid_eye"] = "left";
         } elseif ($fname_parts[4] == 1) {
             $ret["valid_eye"] = "right";
         } else {
             $ret["error"] = "Unrecognized eye code: " . $fname_parts[4] . " from file $fname";
             return $ret;
         }

         //binding_uc_3_l_2_2019_06_07_14_46.dat
         $ret["date"] = $fname_parts[5] . "_" . $fname_parts[6] . "_" . $fname_parts[7];
         $ret["hour"] = $fname_parts[8] . ":" . $fname_parts[9] . ":00";
         $ret["code"] = $ret["number_targets"] . substr(strtoupper(($ret["target_size"])), 0, 1);
         $ret["output_name"] = "binding_" . $ret["date"] . "_" . $fname_parts[8] . "_" . $fname_parts[9] . ".json";
      }
      else if ($n == 7){
         // binding_bc_2_l_2018_09_04.dat
         $ret["date"] = $fname_parts[4] . "_" . $fname_parts[5] . "_" . $fname_parts[6];
         $ret["hour"] = "00:00:00";
         $ret["code"] = $ret["number_targets"] . substr(strtoupper(($ret["target_size"])), 0, 1);
         $ret["output_name"] = "binding_" . $ret["date"] . "_00_00.json";         
      }

      return $ret;

   }

   function generateTrialListForBindingFile($file,$md_percent,$sampling_frequency){
      $HEADER = "#IMAGE";

      $data = separateLegacyRawDataFile($file,$HEADER);
      if ($data["error"] != ""){
         $ret["error"] = "Failed separating obtaining experiment description, resolution and raw data. Reaso: " . $data["error"];
         return $ret;
      }

      // $ret["lines"] = $lines;
      // $ret["resolution"] = $resolution;
      // $ret["reference_for_md"] = $reference_for_md;
      // $ret["experiment_description"] = $study_description;
      // $ret["error"] = "";      

      $lines = $data["lines"];
      unset($data["lines"]);
      $maximum_dispersion = round($data["reference_for_md"]*$md_percent/100);
      $data["max_dispersion_in_px"] = $maximum_dispersion;

      // Setting up the online fixation algorithms. 
      $onlineFixR = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);
      $onlineFixL = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);

      $trial_list = array();
      $trial = array();
      $current_dataset = "";
   
     
      for ($i = 0; $i < count($lines); $i++) {
      
         $line = trim($lines[$i]);

         if (str_contains($line,"->")){
            // Finalizing the current dataset AND the trial.  

            $parts = explode("->",$line);

            $fix = $onlineFixR->finalizeComputation();         
            if ($fix->valid)  $trial["data"][$current_dataset]["fixations_r"][] = $fix->toStdJSON();
            $fix = $onlineFixL->finalizeComputation();               
            if ($fix->valid)  $trial["data"][$current_dataset]["fixations_l"][] = $fix->toStdJSON();
            
            // Storing the user provided response. 
            $trial["response"] = $parts[1];
            $trial_list[] = $trial;
            continue;
         }
   
         $parts = explode(" ",$line);
   
         if (count($parts) == 2){
            // This is a trial name and 0/1 for encoding/retrieval.            
            if ($parts[1] == "0"){
               // Enconding means a new trial. 
               $trial = array();
               $id = $parts[0];

               // Setting the trial type, response and correct response. Actual response is at the end. 
               $trial["ID"] = $id;
               $correct_response = "D";
               if (str_contains($id,"same")) $correct_response = "S";
               $trial["correct_response"] = $correct_response;
               $trial["response"] = "N/A";
               $trial["trial_type"] = $correct_response;
               $trial["data"] = array();

               // Creating the data sets.
               $trial["data"]["encoding_1"] = array();
               $trial["data"]["retrieval_1"] = array();

               // Creating the arrays inside each data set. 
               foreach (array_keys($trial["data"]) as $dataset_name){
                  foreach (ARRAYS_IN_DATASET as $name) {
                     $trial["data"][$dataset_name][$name] = array();
                  }
               }

               $current_dataset = "encoding_1";

            }
            else if ($parts[1] == "1"){

               // Finalizing the current dataset. 
               $fix = $onlineFixR->finalizeComputation();         
               if ($fix->valid)  $trial["data"][$current_dataset]["fixations_r"][] = $fix->toStdJSON();
               $fix = $onlineFixL->finalizeComputation();               
               if ($fix->valid)  $trial["data"][$current_dataset]["fixations_l"][] = $fix->toStdJSON();
               
               $current_dataset = "retrieval_1";

            }
            else{
               $data["error"] = "Could not determine encoding/retrieval on line: $line. Exploded into parts " . implode("|",$parts); 
               return $data;
            }
         }
         else if (count($parts) == 7){
            // This is actual data raw data. 
            $v["ts"]   = $parts[0];
            $v["xr"]   = $parts[1];
            $v["yr"]   = $parts[2];
            $v["xl"]   = $parts[3];
            $v["yl"]   = $parts[4];
            $v["pl"]   = $parts[6];
            $v["pr"]   = $parts[5];
            $trial["data"][$current_dataset]["raw_data"][] = $v;

            $fix = $onlineFixR->computeFixationsOnline($v["xr"], $v["yr"], $v["ts"],$v["pr"]);
            if ($fix->valid)  $trial["data"][$current_dataset]["fixations_r"][] = $fix->toStdJSON();
            $fix = $onlineFixL->computeFixationsOnline($v["xl"], $v["yl"], $v["ts"],$v["pl"]);
            if ($fix->valid)  $trial["data"][$current_dataset]["fixations_l"][] = $fix->toStdJSON();

         }
   
      } 

      $data["trial_list"] = $trial_list;

      return $data;

   }

   function processDirectoryForBinding($input_directory){

      // Getting the information from the path. The last part of the path is expected to be the date code directory. 
      $path_info = pathinfo($input_directory);

      // Gettign the institution name and id
      $ret = getInstituionNameFromPath($input_directory);
      $institution_id = $ret["id"];
      $institution_name = $ret["name"];
      
      // The date code is required as the subject identifer of the old CSV files. 
      $subject_identifier = $path_info["basename"];
      $parts = explode("/",$path_info["dirname"]);
      $n = count($parts);
      $vmid = $parts[$n-1];

      // Finding the required files: eyerepgenconf, and binding bc and uc.
      $binding_bc = "";
      $binding_uc = "";
      $eye_rep_gen_conf = "";

      $all_data = scandir($input_directory);
      foreach ($all_data as $filename){         
         // Filtering any file not ending in .dat or .datf
         if (!is_file("$input_directory/$filename")) continue;
   
         if (!(str_ends_with($filename,".dat") || str_ends_with($filename,".datf"))){
            // Only not dat or datf files of interest in the eye rep gen conf. 
            if (in_array($filename,EYE_REP_GEN_CONF)){
               $eye_rep_gen_conf = "$input_directory/$filename";               
            }
            continue;
         }

         if (str_starts_with($filename,"binding_bc")){
            $binding_bc = "$input_directory/$filename";
         }
         else if (str_starts_with($filename,"binding_uc")){
            $binding_uc = "$input_directory/$filename";
         }
      }

      if ($binding_uc == ""){
         return "Could not find UC File";
      }
      else if ($binding_bc == ""){
         return "Could not find BC File";
      }
      else if ($eye_rep_gen_conf == ""){
         return "Could not find Eye Rep Gen Conf";
      }


      // Now we parse the file name to find the EyeCode, Number of Targets and Size, date, hour. 
      $study_info = getStudyConfigurationFromBindingFile($binding_bc); // Will be the same from binding file. 
      if ($study_info["error"] != ""){
         return "Failed in parsing study configuration from file name of file $binding_bc. Reason: " . $study_info["error"];         
      }

      // Getting the information from the eye rep gen conf. 
      $conf = getBasicInformationFromEyeRepGenConf($eye_rep_gen_conf);
      if ($conf["error"] != ""){
         return "Failed in getting basic information from EyeRepGenConf. Reason: " . $conf["error"] . "\n";         
      }

      // Parsing the actual raw data file to generate the JSON Structure and 
      $md_percent = $conf["md"]["max_dispersion_window"];
      $bc = generateTrialListForBindingFile($binding_bc,$md_percent,$conf["sample_frequency"]);
      if ($bc["error"] != ""){
         return "Failed generating trial list for BC File. Reason:  " . $bc["error"] . "\n";
      }
      if (count($bc["trial_list"]) == 0){
         return "BC File found but contains no data";
      }

      $uc = generateTrialListForBindingFile($binding_uc,$md_percent,$conf["sample_frequency"]);
      if ($uc["error"] != ""){
         return "Failed generating trial list for UC File. Reason:  " . $bc["error"] . "\n";
      }
      if (count($uc["trial_list"]) == 0){
         return "UC File found but contains no data";
      }

      // Now that we have all pieces the final JSON is created and the saved. 
      $json = array();
      // Placeholder. 
      $json["application_user"]["evaluator"] = APPLICATION_USER;
      $json["application_user"]["medic"]     = APPLICATION_USER;
      $json["hash"] = ""; // Hash is empty as it is not used in processing. 

      // Metadata
      $json["metadata"]["date"] =  $study_info["date"];
      $json["metadata"]["hour"] = $study_info["hour"];
      $json["metadata"]["institution_id"] =  $institution_id;
      $json["metadata"]["institution_instance"] =  "0";
      $json["metadata"]["institution_name" ] =  $institution_name;
      $json["metadata"]["mouse_used" ] =  "false";
      $json["metadata"]["processing_parameters_key" ] =  $conf["md"]["pp_key"];
      $json["metadata"]["protocol" ] =  $conf["protocol"];
      $json["metadata"]["status" ] =  "finalized";
      $json["metadata"]["version" ] =  "1";

      // Processing parameters
      $json["processing_parameters"]["latency_escape_radious"] = LATENCY_ESCAPE_RADIOUS; // Since the beginning of time this values has not changed. 
      $json["processing_parameters"]["max_dispersion_window"]  = $md_percent;
      $json["processing_parameters"]["max_disp_window_in_px"]  = $uc["max_dispersion_in_px"];
      $json["processing_parameters"]["minimum_fixation_length"] = MINIMUM_FIXATION_LENGTH;
      $json["processing_parameters"]["resolution_height"] = $bc["resolution"][1];
      $json["processing_parameters"]["resolution_width"] = $bc["resolution"][0];
      $json["processing_parameters"]["sample_frequency"] = $conf["sample_frequency"];

      // QC Paraemters are not really improtant for processing. An empty array is left instead.
      $json["qc_parameters"] = array();

      // The actual studies
      $json["studies"]["Binding BC"]["abbreviation"] = "bc";
      $json["studies"]["Binding BC"]["config_code"] = "B" . $study_info["code"];
      $json["studies"]["Binding BC"]["date"] = $study_info["date"];
      $json["studies"]["Binding BC"]["experiment_description"] = $bc["experiment_description"];
      $json["studies"]["Binding BC"]["hour"] = $study_info["hour"];
      $json["studies"]["Binding BC"]["quality_control"] = array();
      $json["studies"]["Binding BC"]["status"] = "finalized";
      $json["studies"]["Binding BC"]["study_configuration"]["number_targets"] = $study_info["number_targets"];
      $json["studies"]["Binding BC"]["study_configuration"]["target_size"] = $study_info["target_size"];
      $json["studies"]["Binding BC"]["study_configuration"]["valid_eye"] = $conf["valid_eye"];
      $json["studies"]["Binding BC"]["trial_list"] = $bc["trial_list"];
      $json["studies"]["Binding BC"]["version"] = $bc["version"];

      $json["studies"]["Binding UC"]["abbreviation"] = "uc";
      $json["studies"]["Binding UC"]["config_code"] = "B" . $study_info["code"];
      $json["studies"]["Binding UC"]["date"] = $study_info["date"];
      $json["studies"]["Binding UC"]["experiment_description"] = $uc["experiment_description"];
      $json["studies"]["Binding UC"]["hour"] = $study_info["hour"];
      $json["studies"]["Binding UC"]["quality_control"] = array();
      $json["studies"]["Binding UC"]["status"] = "finalized";
      $json["studies"]["Binding UC"]["study_configuration"]["number_targets"] = $study_info["number_targets"];
      $json["studies"]["Binding UC"]["study_configuration"]["target_size"] = $study_info["target_size"];
      $json["studies"]["Binding UC"]["study_configuration"]["valid_eye"] = $conf["valid_eye"];
      $json["studies"]["Binding UC"]["trial_list"] = $uc["trial_list"];
      $json["studies"]["Binding UC"]["version"] = $uc["version"];      

      // The very little subject data we might have
      $json["subject"]["age"] = $conf["age"];
      $json["subject"]["birth_country"] = "";
      $json["subject"]["birth_date"] = "";
      $json["subject"]["gender"] = "";
      $json["subject"]["institution_provided_id"] = "";
      $json["subject"]["lastname"] = "";
      $json["subject"]["local_id"] = $subject_identifier;
      $json["subject"]["name"] = $vmid;
      $json["subject"]["years_formation"] = 0;

      // Saving the file. 
      $json_file = $input_directory . "/" . $study_info["output_name"];
      $fid = fopen($json_file,"w");
      fwrite($fid,json_encode($json,JSON_PRETTY_PRINT));
      fclose($fid);

      // Finally processing the JSON File
      return processJSONFile($json_file);

   }

   $ans = processDirectoryForBinding("work/1242673082/186/2019_04_17_13_33_45");
   if ($ans != "") echo "ERROR $ans\n";
   else echo "All Good\n";


?>