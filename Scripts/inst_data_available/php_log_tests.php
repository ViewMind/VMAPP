<?php

   include ("log_manager.php");
   $log = new LogManager("");
   
   $log->logProgress("Some Log");
   $log->logProgress("Some Log");
   $log->logError("Log Some error");
   
   // This will generate a warning
   $a = $b;
   
   // Calling another shell script
   $output = shell_exec("./sh_secondary_script.sh");
   $log->logProgress("Output of secondary script:\n" . $output);
   
   // This will generate an error
   $a = new B();

?>
