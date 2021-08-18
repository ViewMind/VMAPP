<?php

   $json = parse_ini_file("test_pdf.ini");
   
   $path = $json["path"];
   $version = $json["version"];
   $report_script  = $json["report"];

   $script = "$path/$version/pdf_report_generator.js";
   $lang = "en";
   $pdfoutput   = "generated_report.pdf";
   $error_log = "error.log";
   
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
   }

?>
