<?php

include_once ("../db_management/TableSecrets.php");

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
   echo "Printing input parameters for '$section_name':\n";
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


function expandShortInstanceListToConprenhensiveList($string_list, TableSecrets &$ts){
   // Expanding into a list unique ids.
   $list_to_adapt = explode(" ",$string_list);
   $unique_ids = array();
   foreach ($list_to_adapt as $uid){
      $temp = explode(".",$uid);
      if (count($temp) != 2){
         echo "Error invalid Institution Instance ID format: $uid\n";
         exit();
      }
      $institution = $temp[0];
      $instance    = $temp[1];
   
      if (!is_numeric($institution)){
         echo "Error invalid Institution Instance ID format: $uid\n";
         exit();
      }
      
   
      if ($instance == "x"){
   
         // We need to get all the instances enabled by       
         $ans = $ts->getEnabledInstancesForInstitution($institution);
         if ($ans === FALSE){
            echo "ERROR: " . $ts->getError() . "\n";
            return false;
            exit();
         }
   
         foreach ($ans as $row){    
            $unique_ids[] = $institution . "."  . $row[TableSecrets::COL_INSTITUTION_INSTANCE];
         }
   
         continue;
   
      }
      else if (!is_numeric($instance)){
         echo "Error invalid Institution Instance ID format: $uid\n";
         exit();
      }
   
      // If it got here all checks out and we add it as is. 
      $unique_ids[] = $uid;
   
   }
   
   // We make sure the array's are truly unique 
   //var_dump($unique_ids);
   $unique_ids = array_unique($unique_ids);   
   return $unique_ids;
}

?>