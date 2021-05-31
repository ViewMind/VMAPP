<?php

const INPUT_FILE = "input.cnf";

function readAndVerifyData($required_parameters,$section_name){

   $input_file = __DIR__ . "/" . INPUT_FILE;
   if (!is_file($input_file)){
      echo "No " . $input_file . " found\n";
      exit();
   }

   // Loading the INI file.
   $config = parse_ini_file($input_file,true);

   if ($config === false){
      echo "Could not parse INI file\n";
      exit();
   }

   // Checking the section we want is here. 
   if (!array_key_exists($section_name,$config)){
      echo "Could not find $section_name in input file\n";
      exit();
   }

   // Checking that all required parameters are present. 
   echo "Printint input parameters:\n";
   $params = array();
   foreach ($required_parameters as $p){
      if (!array_key_exists($p,$config[$section_name])){
         echo "Required parameter $p is missing from section $section_name\n";
         exit();
      }
      else{
         // This is done to avoid any EXTRA parameters othere than the required, contained in the section. Avoids potentail screw ups. 
         $params[$p] = $config[$section_name][$p];
         echo "   $p = " . $params[$p] . "\n";
      }
   }

   // Verifying the input to the user:
   echo "Is everything ok? Type yes to continue\n";
   $handle = fopen ("php://stdin","r");
   $line = fgets($handle);
   if(trim($line) != 'yes'){
       echo "ABORTING!\n";
       exit();
   }
   fclose($handle);
   echo "\n"; 
   echo "Advancing ...\n";   
   return $params;
}

?>