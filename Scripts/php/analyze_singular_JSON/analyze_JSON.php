<?php

   include_once("dataset_grapher.php");
   include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
   include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");

   ////////////////////////// INPUTS
   //$input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/reprocess_legacy_data/work/1145868706/524/2020_11_14_14_23_37/gonogo_2020_11_10_11_25.json";
   //$input_json_file = "local_work_nback/nbackrt_2021_09_22_10_44.json";
   //$input_json_file = "local_work/gonogo_2021_09_22_10_48.json";
   //$input_json_file = "oneshot/binding_2021_07_13_11_02.json";   
   //$input_json_file = "local_work_reprocessed_htc_gonogo/gonogo_2021_09_07_17_45_reprocessed.json";
   //$input_json_file = "local_work_reprocessed_hp_gonogo/gonogo_2021_09_07_17_13_reprocessed.json";
   //$input_json_file = "local_work_gng_gera/gonogo_2021_10_06_18_40.json";
   //$input_json_file = "local_work_mendoza_nbackrt/nbackrt_2021_10_04_10_20_reprocessed.json"; 
   //$input_json_file = "local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29_reprocessed_qc_corrected.json";
   //$input_json_file = "local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29_reprocessed_qc_corrected.json";
   //$input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_nback_negative/nbackrt_2021_10_14_15_15_reprocessed_qc_corrected.json";
   $input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work/gonogo_2021_10_13_17_23.json";
   $input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_binding/binding_2021_10_21_10_23.json";

   
   $lang = "en";

   ////////////////////////// INPUTS 
   const PDF_GEN_PATH = "/home/ariel/repos/viewmind_projects/VMPortal/Backend/pdf_report_generation";
   const VERSION      = "v1";
   const S3_BUCKET    = "vm-eu-raw-data";
   const S3_PROFILE   = "viewmind_eu";
   //const WORK_DIR    = "local_work_mercedes_nback";
   //const WORK_DIR    = "local_work_aranda_nbackrt";
   //const WORK_DIR    = "local_work_mark_athelete";
   const WORK_DIR    = "local_work";

   // If filelink is NOT empty, this is what will be processed. 
   $filelink = "";
   //$filelink = "2021_09_22_13_51_53_795631.zip";
   //$filelink = "2021_10_05_16_31_22_088792.zip";
   //$filelink = "2021_10_05_21_50_11_857874.zip";
   //$filelink = "2021_10_06_21_36_26_540637.zip";
   //$filelink = "2021_10_06_21_30_19_261802.zip";   
   //$filelink = "2021_10_07_12_33_04_728107.zip";
   //$filelink ="2021_10_12_13_29_13_921659.zip";
   //$filelink = "2021_10_05_22_38_39_471318.zip";
   //$filelink = "2021_10_13_21_29_20_210489.zip";
   //$filelink = "2021_10_21_13_37_28_470025.zip";
   $filelink = "2021_10_21_14_29_14_303386.zip";

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

   echo "Creating Fixations and Raw Data Plots\n";
   $grapher = new DataSetGrapher($input_json_file);
   $grapher->plotDataSets();

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

   echo "Generating the report json ...\n";

   // We need to get the finalized string.
   $report_script = "$workdir/report_json_filename.json";
   $fid = fopen($report_script,"w");
   fwrite($fid,$vmdc->getFinalizedResultString());
   fclose($fid);

   echo "Generating the PDF...\n";

   // Setting the variables for PDF report generation. 
   //$report_script = "$workdir/routput.json"; // The output of RProcessing is always routput.json in the work directory. 
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
