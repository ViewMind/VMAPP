<?php

include_once(__DIR__."/../fixation_analysis/online_fixations.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/Processing.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/ViewMindDataContainer.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/CSVGeneration/CSVGoNoGo.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/CSVGeneration/CSVReading.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/CSVGeneration/CSVBinding.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/CSVGeneration/CSVNBackRT.php");
include_once(__DIR__."/../../../VMPortal/Backend/common/log_manager.php");


const MD_FROM_SAMPLE_FREQUENCY = [
   150 => ["max_dispersion_window" => 15, "pp_key" => "gazepoint"],      // Assuming sample frequency of 150 Hz is GazePoint
   120 => ["max_dispersion_window" => 3,  "pp_key" => "htcviveeyepro"],    // Assuming sample frequency of 120 Hz is HTC. 
   250 => ["max_dispersion_window" => 3,  "pp_key" => "smi"]
];

const MINIMUM_FIXATION_LENGTH = 50; // This is pretty much universal. 

const LATENCY_ESCAPE_RADIOUS  = 80; // This has never changed, but it should. 

// The possible names for the eye rep gen conf. 
const EYE_REP_GEN_CONF = ["eye_rep_gen_conf.","eye_rep_gen_conf"];

// These 3 arrays should be in all data sets, even if empty. 
const ARRAYS_IN_DATASET = ["raw_data","fixations_l","fixations_r"];

// Used for the storing in the JSON as place holder for moth the medic and the evaluator. 
const APPLICATION_USER = [
   "email" =>  "ariel.arelovich@viewmind.ai",
   "lastname" =>  "Arelovich",
   "local_id" =>  "",
   "name" =>  "Ariel",
   "viewmind_id" =>  ""
];

const INSTITUTION_NAMES = [
   1145868706 => "Axis",
   1369462188 => "ViewMind",
   1105849094 => "Antioquia",
   989302458  => "Novartis",
   1242673082 => "Strathclyde"
];

/**
 * Attempts to find one of thte institution ID's in the path. 
 */
function getInstituionNameFromPath($path){
   $ret["id"]  = -1;
   $ret["name"] = "";
   foreach (INSTITUTION_NAMES as $id => $name){
      if (str_contains($path,$id)){
         $ret["id"] = $id;
         $ret["name"] = $name;
         break;         
      }
   }
   return $ret;
}

/**
 * Function that extracts the study description and the resolution from the a legacy raw data file. 
 */

function separateLegacyRawDataFile($file,$HEADER, $study_version_pos = 1){

   $lines = explode("\n",file_get_contents($file));

   // Filtering the study description.
   $headercounter = 0;
   $study_description = [];
   while ($headercounter < 2){
      if (str_contains($lines[0], $HEADER)) {
         $headercounter++;
         if ($headercounter == 1){
            // This line contains the study version.
            $parts = explode(" ",$lines[0]);
            $ret["version"] = $parts[$study_version_pos];
         }
      }
      else{
         $study_description[] = $lines[0];
      }
      array_shift($lines);
      if (empty($lines)){
         $ret["error"] = "Found end of file $file but could not find the the two headers of $HEADER";
         return $ret;
      }
   }

   $study_description = implode("\n",$study_description);
      
   // Storing resolution, overwritting preset value. 
   $res = $lines[0];

   // Removing the resolution line. 
   array_shift($lines);

   $res = explode(" ",$res);
   if (count($res) != 2){
      $ret["error"] = "Resolution should be a two valued array, but it has " . count($res) . " in file $file\n";
      return $ret;
   }

   $resolution =  [$res[0], $res[1]];

   if (!is_numeric($resolution[0])){
      $ret["error"] = "Resolution width is not a number: " . $resolution[0] . " in file $file\n";
      return $ret;
   }

   if (!is_numeric($resolution[1])){
      $ret["error"] = "Resolution height is not a number: " . $resolution[1] . " in file $file\n";
      return $ret;
   }

   $reference_for_md = max($resolution);

   $ret["lines"] = $lines;
   $ret["resolution"] = $resolution;
   $ret["reference_for_md"] = $reference_for_md;
   $ret["experiment_description"] = $study_description;
   $ret["error"] = "";
   return $ret;
      
}

/**
 * Functiont that extracts basic informationf rom the eye rep gen conf. 
 */
function getBasicInformationFromEyeRepGenConf($eye_rep_gen_conf){
   $config = parse_ini_file($eye_rep_gen_conf);
   //var_dump($config);
   //echo "Parse file |$eye_rep_gen_conf|\n";
   if (array_key_exists("sample_frequency",$config)){
      
      $f = $config["sample_frequency"];
      
      if (array_key_exists($f,MD_FROM_SAMPLE_FREQUENCY)){
         $ret["md"] = MD_FROM_SAMPLE_FREQUENCY[$f];
         $ret["sample_frequency"] = $f;
         $ret["error"] = "";
         $ret["age"] = 0;
         $ret["protocol"] = "";
         $ret["x_px_mm"] = 0.25;
         $ret["y_px_mm"] = 0.25;
         $ret["valid_eye"] = 2;

         // The age is added if it exists. 
         if (array_key_exists("patient_age",$config)){
            $ret["age"] = $config["patient_age"];
         }

         // the protocol name, if it exists. 
         if (array_key_exists("protocol_name",$config)){
            $ret["protocol"] = $config["protocol_name"];
         }

         // The x/y px to mm ratio. Pretty sure it was never changed to anything other than 0.25
         if (array_key_exists("x_px_mm", $config)) {
            $ret["x_px_mm"] = $config["x_px_mm"];               
         }

         if (array_key_exists("y_px_mm", $config)) {
            $ret["y_px_mm"] = $config["y_px_mm"];            
         }

         if (array_key_exists("valid_eye",$config)){
            $ret["valid_eye"] = $config["valid_eye"];
         }

         return $ret;

      }
      else{
         $ret["error"] = "Unrecognized sample frequency $f in $eye_rep_gen_conf";
         return $ret;   
      }
   }
   else{
      $ret["error"] = "Could not find sample frequency in $eye_rep_gen_conf";
      return $ret;
   }
   
}

/**
 * Wrapper for processJSONFileFull when the VMDC and $csv_file array are not of interes. 
 */

function processJSONFile($json_filename){
   $vmdc = new ViewMindDataContainer();
   $csv_array = [];
   return processJSONFileFull($json_filename,$csv_array,$vmdc);
}

/**
 * Function that copies the flow (and error reporting) that goes from loading a JSON file to obtaining the CSV associated with it. 
 */
function processJSONFileFull($json_filename,&$csv_file,&$vmdc){
   
   $path_info = pathinfo($json_filename);
   $base_file_name = $path_info["filename"];
   $workdir = $path_info["dirname"];
   

   $vmdc = new ViewMindDataContainer();
   if (!$vmdc->loadFromFile($json_filename)){
      return "Failed loading $json_filename. Reason: " . $vmdc->getError();
   }
   
   $JSONProcessor = new Processing($vmdc->getProcessingParameters());

   // Processing all studies. 
   $all_studies = $vmdc->getAvailableStudies();
   if (empty($all_studies)){
      return "No studies were found in file " . $json_filename;
   }

   $csv_file = array();

   foreach ($all_studies as $study){
      
      // Setting the study. 
      if ($vmdc->setRawDataAccessPathForStudy($study) === false){
         return "Could not set access to study $study. Reason: " . $vmdc->getError();
      }

      $trial_list = $vmdc->getTrialList();

      if (!$JSONProcessor->processStudy($trial_list,$study)){
         return "Failed to process study $study. Reason: " . $JSONProcessor->getError();
      }

      $vmdc->setTrialList($trial_list);

      if (!$vmdc->setStudyComputedValues($JSONProcessor->getStudyComputedValues())){
         return "Failed setting the computed values for $study. Reason: " . $vmdc->getError();
      }

      // Saving the progress so far ...
      if (!$vmdc->save()){
         return "Failed saving after study $study, from $json_filename";
      }

      $csv_file[$study] = "$workdir/" . strtolower(str_replace(" ","_",$study)) . "_$base_file_name.csv";

   }

   // Generating the CSV
   $csv_generating_function = Processing::CSV_FUNCTION_MAP[$study];
  
   $csv_generating_error = call_user_func($csv_generating_function, $vmdc, $csv_file);

   if ($csv_generating_error != ""){
      return "Error generating CSV for study $study of file $json_filename. Reason: $csv_generating_error";
   }   
   
   return "";
}

/**
 * Function that returns a list of the legacy studies present in a directory. 
 * The object contains keys which describe the studies present while the values are arrays which indicate if a json and csv matching are present as well. 
 */

function getStudyList($directory){

   $list = scandir($directory);
   $tocheck = ["reading", "binding", "gonogo", "nbackrt"];
   $ans = array();
   foreach ($list as $file){
      if (!is_file("$directory/$file")) continue;
      if (!(str_ends_with($file,".dat") || str_ends_with($file,".datf"))) continue;
      foreach ($tocheck as $study_name_file_prefix){
         if (str_starts_with($file,$study_name_file_prefix)){
            //$ans[] = $study_name_file_prefix;
            // If the study is present we check if there is a JSON and CSV present as well. 
            $rec["json"] = false;
            $rec["csv"]  = false;
            foreach ($list as $processed_file){
               if (str_contains($processed_file,$study_name_file_prefix) && (str_ends_with($processed_file,".csv"))) $rec["csv"] = true;
               if (str_contains($processed_file,$study_name_file_prefix) && (str_ends_with($processed_file,".json"))) $rec["json"] = true;
            }
            $ans[$study_name_file_prefix] = $rec;
         }
      }
   }
   return $ans;

}

?>