<?php

include_once("log_manager.php");
include_once("online_fixations.php");


///////////////////////////// FUNCTIONS
function hieararchyGet($array, $hieararchy){
   $res["value"] = array();
   $res["error"] = "";
   foreach ($hieararchy as $key){
      if (!is_array($array)){
         $res["error"] = "When about to test for $key in hiearchy " . implode(",",$hieararchy) . " found that object is not an array";
         return $res;
      }
      if (array_key_exists($key,$array)){
         $array = $array[$key];
      }
      
      else{
         $res["error"] = "Could not find $key in hiearchy " . implode(",",$hieararchy);
         return $res;
      }
   }
   $res["value"] = $array;
   return $res;
}

/**
 * Computed fixations will be an array of Fixatation class while stored will be a simple object array. 
 * We will assume that the computed fixationn is the reference. 
 */

function compareComputedVsStoredFixations($computed, $stored, $logger, $tab){
   $N = count($computed);
   $M = count($stored);
   $S = max([$N,$M]);

   $tol = 0.001;
   $match = 0;

   for ($i = 0; $i < $S; $i++){

      if ($i < $N){
         $cfix = $computed[$i];
      }
      else{
         $sfix = $stored[$i];
         $logger->logProgress($tab. "STORED FIXATION NOT COMPUTED: " . $sfix["x"] . ", " . $sfix["y"] . ", lasted: " . $sfix["duration"] . " @ " . $sfix["time"]);
         continue;
         //
      }
      
      if ($i < $M){
         $sfix = $stored[$i];
      }
      else{
         //echo "===============\n";
         $cfix = $computed[$i];
         //var_dump($cfix);
         $logger->logProgress($tab . "COMPUTED FIXATION NOT IN STORE: " . $cfix["x"] . ", " . $cfix["y"] . ", lasted: " . $cfix["duration"] . " @ " . $cfix["time"] );
         //echo "===============\n";
         continue;
      }

      if ((abs($cfix["x"] - $sfix["x"]) > $tol) || (abs($cfix["y"] - $sfix["y"]) > $tol)){
         $logger->logProgress($tab . "FIXATION MISMATCH ORDER $i:");
         $logger->logProgress($tab . "   COMPUTED: " . $cfix["x"] . ", " . $cfix["y"] . ", lasted: " . $cfix["duration"] . " @ " . $cfix["time"]);
         $logger->logProgress($tab . "   STORED  : " . $sfix["x"] . ", " . $sfix["y"] . ", lasted: " . $sfix["duration"] . " @ " . $sfix["time"]);
      }
      else{
         $match++;         
      }

   }

   $logger->logProgress($tab . "FIX MATCH: Computed: $N. Stored: $M. Iterated over $S. Matched: $match");

}


/////////////////////////// LOG MANAGER DEFINITION
$logname = "validation_log.txt";
shell_exec("rm -f $logname");
$logger = new LogManager($logname);
$logger->enableOutput();

/////////////////////////// FILE TO VALIDATE
$input_file_to_validate = "gonogo/htc_files/gonogo_2021_09_02_18_23.json";
$input_file_to_validate = "binding/hp_files/binding_2021_09_03_07_46.json";
$input_file_to_validate = "gonogo/hp_files/gonogo_2021_09_06_07_33.json";
$input_file_to_validate = "binding/hp_files/binding_2021_09_06_07_46.json";

/////////////////////////// START
$logger->logProgress("Validating File: $input_file_to_validate");

if (!is_file($input_file_to_validate)){
   $logger->logError("Invalid input file $input_file_to_validate");
   exit();
}

// Loading the data. 
$json = json_decode(file_get_contents($input_file_to_validate),true);

// Hiararchy path or (hp) for everything.
// Where to get processcing parameters. 
$md_hp = ["processing_parameters","max_dispersion_window"];
$sample_frequency_hp = ["processing_parameters","sample_frequency"];
$minimum_fixation_window_hp = ["processing_parameters","minimum_fixation_length"];

$study_keys = array_keys($json["studies"]);

$res = hieararchyGet($json,$md_hp);
if ($res["error"] != ""){
   $logger->logError("Failed getting MD: " . $res["error"]);
   exit();
}
$md = $res["value"];
  
$res = hieararchyGet($json,$sample_frequency_hp);
if ($res["error"] != ""){
   $logger->logError("Failed getting Sample Frequency: " . $res["error"]);
   exit();
}
$sample_frequency = $res["value"];

$res = hieararchyGet($json,$minimum_fixation_window_hp);
if ($res["error"] != ""){
   $logger->logError("Failed getting Minimum Fixation Window: " . $res["error"]);
   exit();
}
$minimal_fixation_window = $res["value"];

$logger->logProgress("Processing Parameters: MD: $md, F: $sample_frequency, Minimum Fixation Size: $minimal_fixation_window");
$logger->logProgress("Studies Found: " . implode(",",$study_keys));

/////////////////////////// PROCESSING EACH STUDY

$onlineFixL = new OnlineMovingWindowAlgorithm($md,$sample_frequency,$minimal_fixation_window);
$onlineFixR = new OnlineMovingWindowAlgorithm($md,$sample_frequency,$minimal_fixation_window);

foreach ($study_keys as $study_name){
   
   $trial_liist = $json["studies"][$study_name]["trial_list"];
   $logger->logProgress("   Verifying $study_name. Found " . count($trial_liist)  . " trials");

   foreach ($trial_liist as $trial){
      $data = $trial["data"];

      $logger->logProgress("      Verifying Trial " . $trial["ID"]);

      // Now this will have a number of data sets. And we need to process each one of those. 
      foreach ($data as $dataset_name => $dataset){
         // Each of these will have a raw data, and a fixation_l and fixation_r. 
         $raw_data = $dataset["raw_data"];
         $fixations_l = $dataset["fixations_l"];
         $fixations_r = $dataset["fixations_r"];

         $computed_fix_l = array();
         $computed_fix_r = array();

         foreach ($raw_data as $point){
            $time = $point["ts"];
            $xr   = $point["xr"];
            $yr   = $point["yr"];
            $yl   = $point["yl"];
            $xl   = $point["xl"];
            $fix = $onlineFixR->computeFixationsOnline($xr, $yr, $time);
            if ($fix->valid) {
               $f["x"] = $fix->x;
               $f["y"] = $fix->y;
               $f["time"] = $fix->time;
               $f["duration"] = $fix->duration;
               $computed_fix_r[] = $f;         
            }
            $fix = $onlineFixL->computeFixationsOnline($xl, $yl, $time);
            if ($fix->valid) {
               $f["x"] = $fix->x;
               $f["y"] = $fix->y;
               $f["time"] = $fix->time;
               $f["duration"] = $fix->duration;
               $computed_fix_l[] = $f;
            }            
         }

         $fix = $onlineFixR->finalizeComputation();         
         if ($fix->valid) {
            $f["x"] = $fix->x;
            $f["y"] = $fix->y;
            $f["time"] = $fix->time;
            $f["duration"] = $fix->duration;
            $computed_fix_r[] = $f;
         }
         $fix = $onlineFixL->finalizeComputation();         
         if ($fix->valid){
            $f["x"] = $fix->x;
            $f["y"] = $fix->y;
            $f["time"] = $fix->time;
            $f["duration"] = $fix->duration;
            $computed_fix_l[] = $f;                     
         } 

         $logger->logProgress("         Verifying dataset: $dataset_name");

         // Now that all fixations have been computed we do the comparison. 
         $tab = "            ";

         compareComputedVsStoredFixations($computed_fix_r,$fixations_r,$logger,$tab . "R: ");
         compareComputedVsStoredFixations($computed_fix_l,$fixations_l,$logger,$tab . "L: ");
         
      }
    

   }

}

  
?> 

