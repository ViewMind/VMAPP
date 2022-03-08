<?php

include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");
include_once("../unified_csv/nbackms_hitboxes.php");
include_once("raw_data_csv.php");

$base_work_dir = "work";
$list_of_work_dirs = scandir($base_work_dir);

foreach ($list_of_work_dirs as $work_dir){
   
   if ($work_dir == ".")  continue;
   if ($work_dir == "..")  continue;

   $input_json_file = "";
   $filename = "";
   $list = scandir("$base_work_dir/$work_dir");
   foreach ($list as $file){
      if (str_ends_with($file,".json") && (!str_contains($file,"-whb"))){
         $filename = $file;
         $input_json_file = "$base_work_dir/$work_dir/$file";
         break;
      }
   }

   echo "Processing directory: $base_work_dir/$work_dir\n";

   if (str_contains($filename,"nbackms")){
      // NBackMS requires the computation of the target boxes. 
      $output_file = createComplimentaryNBackMSFile($input_json_file);
   }
   else {
      $output_file = $input_json_file;
   }
   
   // Getting the work directory from the file path.
   $pathinfo = pathinfo($output_file);
   $workdir = $pathinfo["dirname"];
   $basename = $pathinfo["filename"];
   
   $csvs = [];
   $vmdc = null;
   $ans = processJSONFileFull($output_file,$csvs,$vmdc);
   
   if ($ans != ""){
      echo "Failed to process JSON File: $output_file. Reason: $ans\n";
      exit();
   }

   RawDataCSVGen::generateRawDataCSV($csvs,$vmdc);
   
}




?>

