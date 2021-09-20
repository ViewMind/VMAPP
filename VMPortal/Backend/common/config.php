<?php

/**
 * Successfull loading of this file is a must in order to use ANYTHING on the backend
 * All files that use it the status must be checked. 
 */

include_once ("named_constants.php");
include_once (__DIR__ . "/../db_management/DBCon.php");

// Defining the routing map. 
const ROUTING = [
   APIEndpoints::INSTITUTION  => "ObjectInstitution",
   APIEndpoints::REPORTS      => "ObjectReports",
   APIEndpoints::PORTAL_USERS => "ObjectPortalUsers",
   APIEndpoints::SUBJECTS     => "ObjectSubjects"
];

// Loading the pointer location
$pointer_location = parse_ini_file(__DIR__."/config.cnf",true);
$pointer_location = $pointer_location["pointer_location"];

if (!is_dir($pointer_location)){
   server_error("Config repo $pointer_location either does not exist or is not a directory");
   define("CONFIG",array());
   return;
}

// Setting the pointer location for db conf
DBCon::setPointerLocation($pointer_location);

// Defining the configuration file
define("CONFIG_FILE","$pointer_location/config.cnf");

// Pameter groups that must be present in the configuration file. 
$parameter_groups = [ 
   GlobalConfigGeneral::GROUP_NAME => GlobalConfigGeneral::getConstList(["GROUP_NAME"]),
   GlobalConfigLogs::GROUP_NAME => GlobalConfigLogs::getConstList(["GROUP_NAME"]),
   GlobalConfigProcResources::GROUP_NAME => GlobalConfigProcResources::getConstList(["GROUP_NAME"]),
   GlobalConfigS3::GROUP_NAME => GlobalConfigS3::getConstList(["GROUP_NAME"]),
   GlobalConfigUpdateResources::GROUP_NAME => GlobalConfigUpdateResources::getConstList(["GROUP_NAME"])
];

// Checking that this is so. 
$config = parse_ini_file(CONFIG_FILE,true);

if ($config === FALSE){   
   define("CONFIG",array());
   return;
}

foreach ($parameter_groups as $name => $required){

   // Checking that the group exists. 

   if (!array_key_exists($name,$config)){
      server_error("Configuration group $name was not found in the configuration file: " . CONFIG_FILE);
      define("CONFIG",array());
      return;      
   }

   // Checking that all the vlaues are there
   foreach ($required as $r){
      if (!array_key_exists($r,$config[$name])){
         server_error("Configuration group $name does not have required parameter $r, file: " . CONFIG_FILE);
         define("CONFIG",array());
         return;         
      }
   }

}

// All was good we define config.
define("CONFIG",$config);

//error_log("CONFIG ARRAY KEYS: " . implode(" , ",array_keys($config)));

/**
 * Function for bugging out. 
 */
function server_error($error){
   $ret[ResponseFields::HTTP_CODE] = 500;
   $ret[ResponseFields::MESSAGE] = "Internal server error";
   error_log($error);
   http_response_code(500);
   echo json_encode($ret);
}




?>