<?php

  include_once("reprocess_binding.php");
  include_once("reprocess_gonogo.php");
  include_once("reprocess_nbackrt.php");

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  SETUP

  $input_directory = "work";
  $force_reprocessing = ["gonogo", "binding", "nbackrt"]; // Array which is a list of studies to force reprocessing. 
  //$force_reprocessing = [];

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  FUNCTIONS

  /**
   * This will simply list all files. 
   */
  function recursiveFileListGeneration($dir,&$list_of_files){

    $all_files = scandir($dir);
    foreach ($all_files as $file){
       if ($file == ".") continue;
       if ($file == "..") continue;
       $test_string = "$dir/$file";
       if (is_dir($test_string)){
          recursiveFileListGeneration($test_string,$list_of_files);
       }
       else if (is_file($test_string)){
          $list_of_files[] = $test_string;
       }
    }

  }

  /**
   * This will simply create a map where each directory is associated with list of the studies and whether the CSV and JSON are present. 
   */
  function generateDirectoryListFromFileList($file_list){
    $directories = array();
    foreach ($file_list as $file){
      
       $pathinfo = pathinfo($file);
       $dir = $pathinfo["dirname"];
       if (array_key_exists($dir,$directories)) continue;

       $study_list = getStudyList($dir);
       $directories[$dir] = $study_list;

    }
    return $directories;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  SCRIPT START
  $logger = new LogManager("reprocessing_log.log");
  $logger->enableOutput();
  $logger->logProgress("Creating recursive file list ...");

  $list = array();
  recursiveFileListGeneration($input_directory,$list);

  $logger->logProgress("Creating directory list ...");
  $directories = generateDirectoryListFromFileList($list);

  // Quick code used for checking the previous functions work as expected. 
  // foreach ($directories as $d => $studies){
  //   foreach ($studies as $study_name => $is_processed){
  //     $str = "";
  //     if ($is_processed["json"]){
  //       $str = $str . "reconverted to JSON";
  //     }
  //     if ($is_processed["csv"]){
  //       $str = $str . " and reprocessed into CSV";
  //     }
  //     if ($str != ""){
  //       echo "DIR: $d has $study_name study which was $str\n";
  //     }
  //   }
  // }

  /// Actually doing the reprocessing
  $n = count($directories);
  $logger->logProgress("Starting reprocessing on $n directories");

  $counts["nbackrt"]["found"] = 0;
  $counts["binding"]["found"] = 0;
  $counts["gonogo" ]["found"] = 0;
  $counts["nbackrt"]["reprocessed"] = 0;
  $counts["binding"]["reprocessed"] = 0;
  $counts["gonogo" ]["reprocessed"] = 0;


  $missing_file_messages = ["Could not find UC File","Could not find BC File","Could not find Eye Rep Gen Conf","BC File found but contains no data","UC File found but contains no data"];

  // Quick code used for checking the previous functions work as expected. 
  foreach ($directories as $d => $studies){     
     $logger->logProgress("Processing directory $d ... $n Remaining");
     $n--;
     foreach ($studies as $study_name => $is_processed){

        $should_process = !$is_processed["json"] || !$is_processed["csv"];
        $should_process = $should_process || in_array($study_name,$force_reprocessing);

        if ($study_name == "binding"){          
           if ($should_process) {
              $counts[$study_name]["reprocessed"]++;
              $ans = processDirectoryForBinding($d);
              if ($ans != "") {
                  if (!in_array($ans,$missing_file_messages)) {
                    $logger->logError("Failed BINIDING Reprocessing. Reason: $ans");
                    exit();
                  }
                  else{
                    $logger->logWarning("Skipping directory due to: $ans");
                  }
              }
           }
           $counts[$study_name]["found"]++;
        }
        else if ($study_name == "nbackrt"){
          if ($should_process) {
            $counts[$study_name]["reprocessed"]++;
            $ans = processDirectoryForNBackRT($d);
            if ($ans != "") {
                $logger->logError("Failed NBackRT Reprocessing. Reason: $ans");
                exit();
            }
          }
          $counts[$study_name]["found"]++;
        }
        else if ($study_name == "gonogo"){
          if ($should_process) {
            $counts[$study_name]["reprocessed"]++;
            $ans = processDirectoryForGoNoGo($d);
            if ($ans != "") {
                $logger->logError("Failed GoNoGo Reprocessing. Reason: $ans");
                exit();
            }
          }
          $counts[$study_name]["found"]++;
        }        
     }
  }

  var_dump($counts);


?>