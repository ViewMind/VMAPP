<?php

   include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
   include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");

   ////////////////////////// INPUTS
   //$input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/reprocess_legacy_data/work/1145868706/524/2020_11_14_14_23_37/gonogo_2020_11_10_11_25.json";
   $input_json_file = "local_work_nback/nbackrt_2021_09_22_10_44.json";
   //$input_json_file = "local_work/gonogo_2021_09_22_10_48.json";
   $lang = "en";

   ////////////////////////// INPUTS 
   const PDF_GEN_PATH = "/home/ariel/repos/viewmind_projects/VMPortal/Backend/pdf_report_generation";
   const VERSION = "v1";
   const S3_BUCKET   = "vm-eu-raw-data";
   const S3_PROFILE  = "viewmind_eu";
   const WORK_DIR    = "local_work";

   // If filelink is NOT empty, this is what will be processed. 
   //$filelink = "2021_09_22_13_51_53_795631.zip";
   $filelink = "";

   if ($filelink != "") {
       shell_exec("rm -rf "  . WORK_DIR);
       shell_exec("mkdir -p " . WORK_DIR);
         
       echo "Downloading from s3 ...\n";
       $cmd = "aws s3 cp s3://" . S3_BUCKET . "/$filelink --profile=" . S3_PROFILE . " .";
       echo "CMD: $cmd\n";
       shell_exec($cmd);
   
       echo "Untarring ...\n";
       $cmd = "tar -xvzf $filelink";
       echo "CMD: $cmd\n";
       shell_exec($cmd);
   
       echo "Removing zip file ...\n";
       shell_exec("rm $filelink");
   
       echo "Flattening the file path....";
   
       $json = "home";
       while (!is_file($json)) {
           $all = scandir($json);
           array_shift($all);
           array_shift($all);
           if (count($all) !=  1) {
               echo "Flattening failed. Current path of $json does not contain a single directory\n";
               exit();
           }
           $json = $json . "/" . $all[0];
       }
   
       echo "File path $json\n";
       $pathinfo = pathinfo($json);
       $filename = WORK_DIR . "/" . $pathinfo["filename"] . ".json";
       shell_exec("mv $json $filename");
   
       // Doign clean up
       shell_exec("rm -rf home");
   
       $input_json_file = $filename;
       
   }

   ////////////////////////// SCRIPT START

   // Getting the work directory from the file path.
   $pathinfo = pathinfo($input_json_file);
   $workdir = $pathinfo["dirname"];
   $basename = $pathinfo["filename"];

   echo "Work Directory Set to: $workdir\n";
   echo "Creating the CSV File\n";

   $csvs = [];
   $vmdc = null;
   $ans = processJSONFileFull($input_json_file,$csvs,$vmdc);
   if ($ans != ""){
      echo "Failed to process JSON File: $input_json_file. Reason: $ans\n";
      exit();
   }

   echo "Generated CSVs:\n";
   foreach ($csvs as $study => $filename){
      echo "   $study => $filename\n";
   }

   echo "R Processing  ...\n";
   $ans = RProcessing($vmdc,$csvs,$workdir);
   if ($ans != ""){
      echo "Failed on RProcessing. Reason: $ans\n";
      exit();
   }

   echo "Generating the PDF...\n";

   // Setting the variables for PDF report generation. 
   $report_script = "$workdir/routput.json"; // The output of RProcessing is always routput.json in the work directory. 
   $error_log = "$workdir/pdf_generate_error_log.log";
   $pdfoutput = "$workdir/$basename.pdf";
   $script = PDF_GEN_PATH . "/" . VERSION . "/pdf_report_generator.js";

   $cmd = "node $script $lang $report_script $pdfoutput > $error_log 2>&1";
   $output = array();
   $retval = 0;
   exec($cmd, $output,$retval);
   
   if ($retval != 0){
      echo "Error in report generation\n";
      echo "CMD: $cmd\n";
      echo "============= RAW OUTPUT START\n";
      echo implode("\n",$output);
      echo "\n";
      echo file_get_contents($error_log);
      echo "============= RAW OUTPUT END\n";
   }
   else{
      echo "Finished successfully\n";
      echo "Output:\n";
      echo implode("\n",$output);
      echo file_get_contents($error_log);
      echo "\n";      
      //echo "PDF can be found at: $pdfoutput\n";
   }





?>
