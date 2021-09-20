<?php

   /**
    * This will reprocess all sub directories on based directory using the specificied online fixation computation 
    * Using the new % of Resolution as maximum dispersion methodology.
    * It will create a complient JSON File and will then use CSVNBackRT to generate the respecitve CSV File.
    * 
    */

   include_once("processing_common.php");

// Constants for drawing the squares, cross and target on the screen for fielding
// These values are the physical dimensions of the targets in mm.
// These are part of the parser in order to have access to the drawing dimension logic when processing data.
// The following ascii art enumerates the squares.
//               -------                -------
//               |  0  |                |  1  |
//               |     |                |     |
//               -------                -------
//               
//      -------                 |                 -------
//      |  5  |               -- --               |  2  |
//      |     |                 |                 |     |
//      -------                                   -------
//               
//               -------                -------
//               |  4  |                |  3  |
//               |     |                |     |
//               -------                -------
             
   function computeNBackRTTargetBoxes($x_px_2_mm,$y_px_2_mm,$resolutionWidth,$resolutionHeight){
      
      // These were taken straight out of the Legacy Application. 
      $RECT_WIDTH              =   163/4;
      $RECT_HEIGHT             =   155/4;
      $TARGET_R                =   42/4;
      $TARGET_OFFSET_X         =   39/4;
      $TARGET_OFFSET_Y         =   35/4;
      
      $TARGET_BOX_0          =   0;
      $TARGET_BOX_1            =   1;
      $TARGET_BOX_2            =   2;
      $TARGET_BOX_3            =   3;
      $TARGET_BOX_4            =   4;
      $TARGET_BOX_5            =   5;
      
      $K_HORIZONAL_MARGIN    =   0.06;
      $K_SPACE_BETWEEN_BOXES   =   0.09;
      $K_VERTICAL_MARGIN       =   0.06;
      
      $K_CROSS_LINE_LENGTH   =   0.05;
      
      $TARGET_BOX_EX_W       = 1.5/6.0;
      $TARGET_BOX_EX_H       = 1.5/5.6;  
      
      // Computing the drawn target box locations and the hit target boxes
      $kx = $x_px_2_mm;
      $ky = $y_px_2_mm;

      $targetBoxWidth = ($RECT_WIDTH/$kx);
      $targetBoxHeight = ($RECT_HEIGHT/$ky);

      // Computing the boxes's coordinates.
      $x5 = $resolutionWidth*$K_HORIZONAL_MARGIN;
      $x2 = $resolutionWidth*(1- $K_HORIZONAL_MARGIN) - $targetBoxWidth;
      $x0 = $x5 + $targetBoxWidth + $resolutionWidth*$K_SPACE_BETWEEN_BOXES;
      $x4 = $x0;
      $x1 = $x2 - $targetBoxWidth - $resolutionWidth*$K_SPACE_BETWEEN_BOXES;
      $x3 = $x1;
      $y2 = $resolutionHeight/2.0 - $targetBoxWidth/2.0;
      $y5 = $y2;
      $y0 = $resolutionHeight*$K_VERTICAL_MARGIN;
      $y1 = $y0;
      $y3 = $resolutionHeight*(1-$K_VERTICAL_MARGIN) - $targetBoxHeight;
      $y4 = $y3;

      $targetBoxIncreasedMarginWidth = $targetBoxWidth*$TARGET_BOX_EX_W;
      $targetBoxIncreasedMarginHeight = $targetBoxHeight*$TARGET_BOX_EX_H;
      $targetBoxWidthEX = $targetBoxWidth + 2*$targetBoxIncreasedMarginWidth;
      $targetBoxHeightEX = $targetBoxHeight + 2*$targetBoxIncreasedMarginHeight;   
      
      $hitboxes = array();
      $leeway = $targetBoxHeightEX/2;
      // When the studies where done, all boxes except 0 and 5 would consider half the height more and less when checkin for target hit. Since target hit is straight forward in processing, then we need to include the leeway here. 
      // Target Box 0
      $hitboxes[] = [$x0 - $targetBoxIncreasedMarginWidth, $y0-$targetBoxIncreasedMarginHeight - $leeway, $targetBoxWidthEX, $targetBoxHeightEX + $leeway*2]; 
      // Target Box 1
      $hitboxes[] = [$x1 - $targetBoxIncreasedMarginWidth, $y1-$targetBoxIncreasedMarginHeight - $leeway, $targetBoxWidthEX, $targetBoxHeightEX + $leeway*2]; 
      // Target Box 2 - Normal
      $hitboxes[] = [$x2 - $targetBoxIncreasedMarginWidth, $y2-$targetBoxIncreasedMarginHeight, $targetBoxWidthEX, $targetBoxHeightEX]; 
      // Target Box 3
      $hitboxes[] = [$x3 - $targetBoxIncreasedMarginWidth, $y3-$targetBoxIncreasedMarginHeight - $leeway, $targetBoxWidthEX, $targetBoxHeightEX + $leeway*2]; 
      // Target Box 4
      $hitboxes[] = [$x4 - $targetBoxIncreasedMarginWidth, $y4-$targetBoxIncreasedMarginHeight - $leeway, $targetBoxWidthEX, $targetBoxHeightEX + $leeway*2]; 
      // Target Box 5 - Normal
      $hitboxes[] = [$x5 - $targetBoxIncreasedMarginWidth, $y5-$targetBoxIncreasedMarginHeight, $targetBoxWidthEX, $targetBoxHeightEX]; 

      return $hitboxes;

      
   }

   function getStudyConfigurationFromNbackFile($file){
      
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
      $ret["output_name"] = "nbackrt_" . $ret["date"] . "_" . $fname_parts[5] . "_" . $fname_parts[6] . ".json";

      return $ret;

   }

   function generateTrialListForNBackRTFile($file,$md_percent,$sampling_frequency){
      $HEADER = "#NBACKRT";

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
      $name_map = array();
      $slines = explode("\n",$data["experiment_description"]);
      foreach ($slines as $line){
         $parts = explode(" ",$line);
         $n = count($parts);
         // There are two version of the Nback Experimetn description. One describes 3 targets and the other 6. The first value is the trial ID always. 
         if (($n == 4) || ($n == 7)){
            $id = array_shift($parts);
            $name_map[$id] = implode(" ",$parts);
         }
      }


      // Setting up the online fixation algorithms. 
      $onlineFixR = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);
      $onlineFixL = new OnlineMovingWindowAlgorithm($maximum_dispersion,$sampling_frequency,MINIMUM_FIXATION_LENGTH);

      $trial_list = array();
      $trial = array();
      $current_dataset = "";

      $code_to_dataset_name = [
         "1" => "encoding_1",
         "2" => "encoding_2",
         "3" => "encoding_3",
         "4" => "retrieval_1"
      ];
   
      // Adding this line in order to force finalization of the final dataset and foce adding the trial to the trial list. 
      $lines[] = "000 1"; 
     
      for ($i = 0; $i < count($lines); $i++) {
      
         $line = trim($lines[$i]);
   
         $parts = explode(" ",$line);

         if (count($parts) == 2){
            // Header line, trial name + code.
            $trial_id = $parts[0];
            $dataset = $code_to_dataset_name[$parts[1]];
            $new_trial = false;
            
            if ($dataset != $current_dataset){               
               
               if ($current_dataset != "") {
                  // Finalizing the current dataset.
                  $fix = $onlineFixR->finalizeComputation();
                  if ($fix->valid) {
                      $trial["data"][$current_dataset]["fixations_r"][] = $fix->toStdJSON();
                  }
                  $fix = $onlineFixL->finalizeComputation();
                  if ($fix->valid) {
                      $trial["data"][$current_dataset]["fixations_l"][] = $fix->toStdJSON();
                  }
               }

               if ($dataset == "encoding_1"){
                  // This means this a new trial needs to be created and the old one saved. 
                  $new_trial = true;
                  if ($current_dataset != ""){
                     // If this a new trial and NOT the first one, the data is stored. 
                     $trial_list[] = $trial;
                  }
               }

               $current_dataset = $dataset;               

            }

            if ($new_trial){
               
               $trial = array();               

               // Setting the trial type, response and correct response. Actual response is at the end. 
               $trial["ID"] = $trial_id;
               $trial["response"] = "";
               $trial["trial_type"] = $name_map[$trial_id];
               $trial["data"] = array();

               // Creating the data sets.
               $trial["data"]["encoding_1"] = array();
               $trial["data"]["encoding_2"] = array();
               $trial["data"]["encoding_3"] = array();
               $trial["data"]["retrieval_1"] = array();

               // Creating the arrays inside each data set. 
               foreach (array_keys($trial["data"]) as $dataset_name){
                  foreach (ARRAYS_IN_DATASET as $name) {
                     $trial["data"][$dataset_name][$name] = array();
                  }
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

   function processDirectoryForNBackRT($input_directory){

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
      $nback = "";
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

         if (str_starts_with($filename,"nbackrt")){
            $nback = "$input_directory/$filename";
         }
      }

      if ($nback == ""){
         return "Could not find NBackRT File\n";
      }
      else if ($eye_rep_gen_conf == ""){
         return "Could not find Eye Rep Gen Conf";
      }


      // Now we parse the file name to find the EyeCode, date and hour. 
      $study_info = getStudyConfigurationFromNbackFile($nback); // Will be the same from binding file. 
      if ($study_info["error"] != ""){
         return "Failed in parsing study configuration from file name of file $nback. Reason: " . $study_info["error"];         
      }

      // Getting the information from the eye rep gen conf. 
      $conf = getBasicInformationFromEyeRepGenConf($eye_rep_gen_conf);
      if ($conf["error"] != ""){
         return "Failed in getting basic information from EyeRepGenConf. Reason: " . $conf["error"] . "\n";         
      }

      // Parsing the actual raw data file to generate the JSON Structure and 
      //var_dump($conf);
      $md_percent = $conf["md"]["max_dispersion_window"];
      $trial_list = generateTrialListForNBackRTFile($nback,$md_percent,$conf["sample_frequency"]);
      if ($trial_list["error"] != ""){
         return "Failed generating trial list for BC File. Reason:  " . $trial_list["error"] . "\n";
      }

      // Computing the target boxes
      $hitboxes = computeNBackRTTargetBoxes($conf["x_px_mm"],$conf["y_px_mm"],$trial_list["resolution"][0],$trial_list["resolution"][1]);

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
      $json["processing_parameters"]["nback_hitboxes"] = $hitboxes;

      // QC Paraemters are not really improtant for processing. An empty array is left instead.
      $json["qc_parameters"] = array();

      // The actual studies
      $json["studies"]["NBack RT"]["abbreviation"] = "bc";
      $json["studies"]["NBack RT"]["config_code"] = "B" . $study_info["code"];
      $json["studies"]["NBack RT"]["date"] = $study_info["date"];
      $json["studies"]["NBack RT"]["experiment_description"] = $trial_list["experiment_description"];
      $json["studies"]["NBack RT"]["hour"] = $study_info["hour"];
      $json["studies"]["NBack RT"]["quality_control"] = array();
      $json["studies"]["NBack RT"]["status"] = "finalized";
      $json["studies"]["NBack RT"]["study_configuration"]["valid_eye"] = $study_info["valid_eye"];
      $json["studies"]["NBack RT"]["trial_list"] = $trial_list["trial_list"];
      $json["studies"]["NBack RT"]["version"] = $trial_list["version"];

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

   // $ans = processDirectoryForNBackRT("work/1369462188/440/2020_06_17_17_09_03/");
   // if ($ans != "") echo "ERROR $ans\n";
   // else echo "All Good\n";


?>