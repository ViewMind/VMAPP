<?php

function loadInputs($action){
   // Loading the inputs file.
   $inputs_file = "./inputs.cnf";
   if (!is_file($inputs_file)){
      echo "No inputs file found: '$inputs_file'\n";
      exit();
   }

   $input = json_decode(file_get_contents($inputs_file),true);

   if (!is_array($input)){
      echo "Failed in loading the input file. Last JSON error: " . json_last_error_msg() . "\n";
      exit();
   }
   
   if (!array_key_exists($action,$input)){
      echo "No input for action '$action'. Exiting\n";
      exit();
   }   
   return $input[$action];
}

?>