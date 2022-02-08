<?php

include_once (__DIR__ . "/../db_management/TableInstitution.php");
include_once (__DIR__ . "/../db_management/TableSecrets.php");
include_once (__DIR__ . "/../db_management/TableUpdates.php");
include_once (__DIR__ . "/../db_management/TableAppPasswordRecovery.php");
include_once (__DIR__ . "/../common/named_constants.php");

class ObjectInstances extends ObjectBaseClass{


   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }


   function create($identifier,$parameters){

      // Creating the tables to be used. 
      $ti      = new TableInstitution($this->con_main);
      $ts      = new TableSecrets($this->con_secure);
      $tapr    = new TableAppPasswordRecovery($this->con_secure);
      $tupdate = new TableUpdates($this->con_main);

      //error_log("Parameters when creating instance: " . json_encode($parameters));

      // The eyetracker key url parameters is required and must be valid (it's value must be among the possible values)
      if (!array_key_exists(URLParameterNames::EYETRACKER_KEY, $parameters)) {
         $this->suggested_http_code = 401;
         $this->error = "URL Parameter " . URLParameterNames::EYETRACKER_KEY . " is requrired";
         return false;
      }
      $etkey = $parameters[URLParameterNames::EYETRACKER_KEY];

      if (!EyeTrackerCodes::validate($etkey)){
         $this->suggested_http_code = 401;
         $this->error = "URL Parameter $etkey is an invalid eyetracker key";
         return false;
      }

      // We need the institution name. 
      $ans = $ti->getInstitutionInformationFor([$identifier]);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in getting the information for instituion $identifier: " . $ti->getError(); 
         return false;
      }
      if (count($ans) === 0){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in getting the information for instituion $identifier. Id was not found";
         return false;
      }
      $instinfo = $ans[0];
      $institution_name = $instinfo[TableInstitution::COL_INSTITUTION_NAME];

      // First, we need to find the largest instance number
      $instance = $ts->getLastInstanceForInstitution($identifier);
      if ($instance === false){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Probable non existant institution id $identifier";
         $this->error = "Failed in getting the last instance for instituion. Reason: " . $ts->getError(); 
         return false;
      }

      // Third, we create a new secret.
      $instance = intval($instance);
      $instance++; // We add one. 

      $secret = $ts->createNewSecret($identifier,$instance);
      if ($secret === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed creating new secre for institution $identifier and instance $instance: " . $ts->getError(); 
         return false;
      }

      // Fourth we create a new master password for the app.
      $mpasswd = $tapr->createPassword($identifier,$instance);
      if ($mpasswd === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in creating restore app password for $identifier.$instance. Reason: " . $tapr->getError();
      }

      // Fifth we add the instance to the update table using either the latest version or the provided version.
      if (array_key_exists(URLParameterNames::VERSION,$parameters)){
         $version = $parameters[URLParameterNames::VERSION];
      }
      else {
         // We need to get the latest version.
         $version = $tupdate->getLatestVersion();
         if ($version === false){
            $this->suggested_http_code = 500;
            $this->returnable_error = "Internal database error";
            $this->error = "Failed in getting the latest version of the app" . $tupdate->getError(); 
            return false;   
         }
      }

      $ans = $tupdate->addInstance($identifier,$instance,$version,$etkey);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in adding a new instance to the update table. Reason" . $tupdate->getError(); 
         return false;         
      }

      // We put together the return value, starting with the contents fo the configuration file.  

      $configuration_file_contents = "";
      $configuration_file_contents = $configuration_file_contents . "institution_id = " . $identifier . ";\n";
      $configuration_file_contents = $configuration_file_contents . "instance_number = " . $instance . ";\n";
      $configuration_file_contents = $configuration_file_contents . "institution_name = " . $institution_name . ";\n";
      foreach ($secret as $key => $value){
         $configuration_file_contents = $configuration_file_contents . "instance_key = $key;\n";
         $configuration_file_contents = $configuration_file_contents . "instance_hash_key = $value;\n";
      }

      $ret["vmconfiguration"] = $configuration_file_contents;
      $ret["app_rec_passwd"]  = $mpasswd;

      return $ret;
   
   }

   function list($identifier,$parameters){
      
      // Creating the tables to be used. 
      $ti      = new TableInstitution($this->con_main);
      $tupdate = new TableUpdates($this->con_main);           
      

      // First we get teh list of all current version for each instance. 
      $ans = $tupdate->listCurrentVersionForAllInstances();
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in getting the list of current version for all instances: " . $tupdate->getError(); 
         return false;
      }

      // The information is of little use so we need to categorize it by institution. Hece we list the instituions.
      $institutions = $ti->getInstitutionNameMap();
      if ($institutions === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Internal database error";
         $this->error = "Failed in getting the list of institutions: " . $ti->getError(); 
         return false;
      }

      // Creating the return array. 
      $ret = array();
      foreach ($institutions as $inst_id => $inst_name){
         $ret[$inst_id]["name"] = $inst_name;
         $ret[$inst_id]["versions"] = array();
      }


      // adding the ifnormation from the update table. 
      foreach ($ans as $row){
         $version = $row[TableUpdates::COL_VERSION_STRING];
         $instance = $row[TableUpdates::COL_INSTITUTION_INSTANCE];
         $institution = $row[TableUpdates::COL_INSTITUTION_ID];

         if (!array_key_exists($institution,$ret)){
            $this->suggested_http_code = 500;
            $this->returnable_error = "Internal congruencty error";
            $this->error = "There is an instance with institution number $institution which is not present in the institution table"; 
            return false;
         }

         $ret[$institution]["versions"][$instance] = $version;

      }

      return $ret;
      
   }



}

?>