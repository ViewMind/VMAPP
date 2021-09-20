<?php

   include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
   include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");

   ////////////////////////// INPUTS
   $input_json_file = "/home/ariel/repos/viewmind_projects/Scripts/php/reprocess_legacy_data/work/1145868706/524/2020_11_14_14_23_37/gonogo_2020_11_10_11_25.json";
   $lang = "en";

   ////////////////////////// INPUTS 
   const PDF_GEN_PATH = "/home/ariel/repos/viewmind_projects/VMPortal/Backend/pdf_report_generation";
   const VERSION = "v1";

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