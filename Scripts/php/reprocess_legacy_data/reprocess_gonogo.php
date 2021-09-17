<?php

   /**
    * This will reprocess all sub directories on based directory using the specificied online fixation computation 
    * Using the new % of Resolution as maximum dispersion methodology.
    * It will create a complient JSON File and will then use CSVGoNoGo to generate the respecitve CSV File.
    * 
    */

   include_once("processing_common.php");
             
   function computeGoNoGoTargetBoxes($resolutionWidth,$resolutionHeight){
      
      // #define  GONOGO_TRIAL_TYPE_RLEFT   0
      // #define  GONOGO_TRIAL_TYPE_GLEFT   1
      // #define  GONOGO_TRIAL_TYPE_RRIGHT  2
      // #define  GONOGO_TRIAL_TYPE_GRIGHT  3
      
      // Draw constants need to be here for computations when manager is not available.
      $GONOGO_SIDE_MARGIN               = 0.05;   // Percent of resolution width
      $GONOGO_TARGET_SIZE               = 0.03;   // Percent of resolution width
      $GONOGO_TARGET_TOL                = 0.20;   // Percent of resolution width
      $GONOGO_ARROW_LENGTH              = 0.1 ;   // Percent of resolution width
      $GONOGO_ARROW_INDICATOR_LENGTH    = 0.4 ;   // Percent of arrow length
      $GONOGO_CROSS_LINE_LENGTH         = 0.1 ;   // Percent of resolution width/height.
      $GONOGO_TARGET_LINE_WIDTH         = 0.1 ;   // Percent of the diameter.
      $GONOGO_LINE_WIDTH                = 0.1 ;   // Percent of the line length.
      $GONOGO_INDICATOR_LINE_LENGTH     = 0.3 ;   // Percent of the line length
      $GONOGO_ARROW_TARGET_BOX_WMARGIN  = 2.2 ;   // Multiplier to the arrow width
      $GONOGO_ARROW_TARGET_BOX_HMARGIN  = 4.5 ;   // Multiplier to the arrow height
      
      $diameter = ($resolutionWidth*$GONOGO_TARGET_TOL);
      $targetDiamteter = ($resolutionWidth*$GONOGO_TARGET_SIZE);
      $r = $diameter/2;
      $cY = $resolutionHeight/2;
      $xOffset = ($diameter - $targetDiamteter)/2;
      $left_target = [$resolutionWidth*$GONOGO_SIDE_MARGIN - $xOffset,$cY-$r,$diameter,$diameter];
      $right_target = [$resolutionWidth*(1-$GONOGO_SIDE_MARGIN) - $diameter + $xOffset,$cY-$r,$diameter,$diameter];
      
      $centerX = $resolutionWidth/2;
      $centerY = $resolutionHeight/2;
      $line_length = $resolutionWidth*$GONOGO_CROSS_LINE_LENGTH;
      $indicator_line_length = $GONOGO_INDICATOR_LINE_LENGTH*$line_length;
      
      // Arrow x coordinates of the main trunk
      $left_x0 = $centerX-$line_length/2;
      $right_x0 = $centerX+$line_length/2;
      
      // Offset to build indicators (45 degree line of the arrow ends)
      $ka = sqrt(2)*$indicator_line_length/2;
      
      // Using information above to actually compute the arrow target box.
      $arrowWidth = ($right_x0 - $left_x0);
      $arrowHeight = 2*$ka;
      $arrowTargetBoxW = $arrowWidth*$GONOGO_ARROW_TARGET_BOX_WMARGIN;
      $arrowTargetBoxH = $arrowHeight*$GONOGO_ARROW_TARGET_BOX_HMARGIN;
      $arrowTargetBox = [
         $left_x0 - (($arrowTargetBoxW - $arrowWidth)/2),
         $centerY - $ka - (($arrowTargetBoxH - $arrowHeight)/2),
         $arrowTargetBoxW,
         $arrowTargetBoxH
      ];

      return [$arrowTargetBox, $left_target, $right_target];

      
   }

   function getStudyConfigurationFromGoNoGoFile($file){
      
      $ret["error"] = "";
      $path_info = pathinfo($file);
      $fname = $path_info["basename"];
      $fname_parts = explode(".",$fname);
      $fname_parts = explode("_",$fname_parts[0]);

      $n = count($fname_parts);
      
      if ($n != 7){
         $ret["error"] = "Unknown file name format with $n parts: $fname";
         return $ret;
      }

      if ($fname_parts[1] == 2) $ret["valid_eye"] = "both";
      else if ($fname_parts[1] == 0) $ret["valid_eye"] = "left";
      else if ($fname_parts[1] == 1) $ret["valid_eye"] = "right";
      else{
         $ret["error"] = "Unrecognized eye code: " . $fname_parts[4] . " from file $fname";
         return $ret;
      }

      
      $ret["date"] = $fname_parts[2] . "_" . $fname_parts[3] . "_" . $fname_parts[4];
      $ret["hour"] = $fname_parts[5] . ":" . $fname_parts[6] . ":00";
      $ret["code"] = "B";
      $ret["output_name"] = "gonogo_" . $ret["date"] . "_" . $fname_parts[5] . "_" . $fname_parts[6] . ".json";

      return $ret;

   }

   function generateTrialListForGoNoGoFile($file,$md_percent,$sampling_frequency){
      $HEADER = "#GONOGO";

      $data = separateLegacyRawDataFile($file,$HEADER);
      if ($data["error"] != ""){
         $ret["error"] = "Failed separating obtaining experiment description, resolution and raw data. Reaso: " . $data["error"];
         return $ret;
      } 

      $lines = $data["lines"];
      unset($data["lines"]);
      $maximum_dispersion = round($data["reference_for_md"]*$md_percent/100);
      $data["max_dispersion_in_px"] = $maximum_dispersion;

      // Creating a map between the trial name and description. For tat wee need a quick parse of the experiment description
      $name_map = [
         0 => "R<-",
         1 => "G<-",
         2 => "R->",
         3 => "G->"
      ];

      // Setting up the online fixation algorithms. 
      $onlineFixR = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);
      $onlineFixL = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);

      $trial_list = array();
      $trial = array();
      $current_trial_id = "";
      $current_dataset = "unique"; // This is the only data set for this file.

   
      // Adding this line in order to force finalization of the final dataset and foce adding the trial to the trial list. 
      $lines[] = "000 1"; 
     
      for ($i = 0; $i < count($lines); $i++) {
      
         $line = trim($lines[$i]);
   
         $parts = explode(" ",$line);

         if (count($parts) == 2){
            
            // Header line, trial name + code. This will ALWAYS be a new dataset/trial. 
            $trial_id = $parts[0];
            $trial_type = $name_map[$parts[1]];

            // check to avoid adding an empty trial at the very beginning. 
            if ($current_trial_id != "") {
               // Finalizing the current dataset.
               $fix = $onlineFixR->finalizeComputation();
               if ($fix->valid) {
                   $trial["data"][$current_dataset]["fixations_r"][] = $fix->toStdJSON();
               }
               $fix = $onlineFixL->finalizeComputation();
               if ($fix->valid) {
                   $trial["data"][$current_dataset]["fixations_l"][] = $fix->toStdJSON();
               }

               // Storing the previous trial. 
               $trial_list[] = $trial;

            }

            $current_trial_id = $trial_id;
            
            $trial = array();               

            // Setting the trial type, response and correct response. Actual response is at the end. 
            $trial["ID"] = $trial_id;
            $trial["response"] = "";
            $trial["correct_response"] = "";
            $trial["trial_type"] = $trial_type;
            $trial["data"] = array();

            // Creating the data sets.
            $trial["data"][$current_dataset] = array();

            // Creating the arrays inside each data set. 
            foreach (array_keys($trial["data"]) as $dataset_name){
               foreach (ARRAYS_IN_DATASET as $name) {
                  $trial["data"][$dataset_name][$name] = array();
               }
            }


         }
         else if (count($parts) == 7){
            // Nomal data line 
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

   function processDirectoryForGoNoGo($input_directory){

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
      $gonogo = "";
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

         if (str_starts_with($filename,"gonogo")){
            $gonogo = "$input_directory/$filename";
         }
      }

      if ($gonogo == ""){
         return "Could not find GoNoGo File\n";
      }
      else if ($eye_rep_gen_conf == ""){
         return "Could not find Eye Rep Gen Conf";
      }


      // Now we parse the file name to find the EyeCode, date and hour. 
      $study_info = getStudyConfigurationFromGoNoGoFile($gonogo); // Will be the same from binding file. 
      if ($study_info["error"] != ""){
         return "Failed in parsing study configuration from file name of file $gonogo. Reason: " . $study_info["error"];         
      }

      // Getting the information from the eye rep gen conf. 
      $conf = getBasicInformationFromEyeRepGenConf($eye_rep_gen_conf);
      if ($conf["error"] != ""){
         return "Failed in getting basic information from EyeRepGenConf. Reason: " . $conf["error"] . "\n";         
      }

      // Parsing the actual raw data file to generate the JSON Structure and 
      //var_dump($conf);
      $md_percent = $conf["md"]["max_dispersion_window"];
      $trial_list = generateTrialListForGoNoGoFile($gonogo,$md_percent,$conf["sample_frequency"]);
      if ($trial_list["error"] != ""){
         return "Failed generating trial list for BC File. Reason:  " . $trial_list["error"] . "\n";
      }

      // Computing the target boxes
      $hitboxes = computeGoNoGoTargetBoxes($trial_list["resolution"][0],$trial_list["resolution"][1]);

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
      $json["processing_parameters"]["max_disp_window_in_px"]  = $trial_list["max_dispersion_in_px"];
      $json["processing_parameters"]["minimum_fixation_length"] = MINIMUM_FIXATION_LENGTH;
      $json["processing_parameters"]["resolution_height"] = $trial_list["resolution"][1];
      $json["processing_parameters"]["resolution_width"] = $trial_list["resolution"][0];
      $json["processing_parameters"]["sample_frequency"] = $conf["sample_frequency"];
      $json["processing_parameters"]["go_no_go_hitboxes"] = $hitboxes;

      // QC Paraemters are not really improtant for processing. An empty array is left instead.
      $json["qc_parameters"] = array();

      // The actual studies
      $json["studies"]["Go No-Go"]["abbreviation"] = "bc";
      $json["studies"]["Go No-Go"]["config_code"] = "B" . $study_info["code"];
      $json["studies"]["Go No-Go"]["date"] = $study_info["date"];
      $json["studies"]["Go No-Go"]["experiment_description"] = $trial_list["experiment_description"];
      $json["studies"]["Go No-Go"]["hour"] = $study_info["hour"];
      $json["studies"]["Go No-Go"]["quality_control"] = array();
      $json["studies"]["Go No-Go"]["status"] = "finalized";
      $json["studies"]["Go No-Go"]["study_configuration"]["valid_eye"] = $study_info["valid_eye"];
      $json["studies"]["Go No-Go"]["trial_list"] = $trial_list["trial_list"];
      $json["studies"]["Go No-Go"]["version"] = $trial_list["version"];

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

   // $ans = processDirectoryForGoNoGo("work/1369462188/445/2021_09_02_15_00_31/");
   // if ($ans != "") echo "ERROR $ans\n";
   // else echo "All Good\n";


?>