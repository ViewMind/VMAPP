<?php

include_once ("ObjectPortalUsers.php");
include_once (__DIR__ . "/../db_management/TableProcessingParameters.php");
include_once (__DIR__ . "/../db_management/TableInstitution.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");
include_once (__DIR__ . "/../db_management/TableUpdates.php");
include_once (__DIR__ . "/../common/named_constants.php");

class ObjectInstitution extends ObjectBaseClass{

   private ObjectPortalUsers $portal_users;   

   private const ApplicationFileName = "app.zip";

   function __construct($service,$headers){
      parent::__construct($service,$headers);
      $this->portal_users = new ObjectPortalUsers($service,$headers);
   }

   function operating_information($identifier,$parameters){

      // We verify that the proper URL parameters were used. 
      $expected = [URLParameterNames::PPKEY, URLParameterNames::INSTANCE, URLParameterNames::VERSION];

      foreach ($expected as $p) {
          if (!array_key_exists($p, $parameters)) {
              $this->suggested_http_code = 401;
              $this->returnable_error = "Missing url parameter";
              $this->error = "operating information operation requires the parameter $p";
              return false;
          }
      }

      $pname = $parameters[URLParameterNames::PPKEY];
      $inst_number = $parameters[URLParameterNames::INSTANCE];
      $version = $parameters[URLParameterNames::VERSION];

      $institution_id = $_POST[POSTFields::INSTITUTION_ID];
      $institution_instance = $_POST[POSTFields::INSTITUION_INSTANCE];

      if (($identifier != $institution_id) || ($institution_instance != $inst_number)){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Mismatch between signature info and requested info";
         $this->error = "Asking for operating information from $identifier.$inst_number, however signature is from $institution_id.$institution_instance";
         return false;         
      }

      $ans = $this->portal_users->getallmedical($identifier,$parameters);
      if ($ans === FALSE){
         $this->suggested_http_code = $this->portal_users->suggested_http_code;
         $this->returnable_error = "Could not get institution user information";
         $this->error = "On operation geallmedical: " .  $this->portal_users->getError();
         return false;
      }

      $ret[ResponseDataSubFields::MEDICS] = $ans;

      $tpp = new TableProcessingParameters($this->con_main);
      $ans = $tpp->getProductParameters($pname);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get eyetracker parameters";
         $this->error = "On obtaining the processing parameters: " . $tpp->getError();
         return false;
      }

      if (count($ans) == 0){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Could not get eyetracker parameters";
         $this->error = "No processing parameters found for product: $pname";
         return false;
      }

      $ret[ResponseDataSubFields::PROC_PARAMS] = json_decode($ans[0][TableProcessingParameters::COL_PROC_PARAM],true);
      if (json_last_error() != JSON_ERROR_NONE){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get eyetracker parameters";
         $this->error = "Failed to decode JSON String for processing parameters for $pname: " . json_last_error_msg();
         return false;
      }
      $ret[ResponseDataSubFields::FREQ_PARAMS] = json_decode($ans[0][TableProcessingParameters::COL_FREQ_PARAM],true);
      if (json_last_error() != JSON_ERROR_NONE){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get QC parameters";
         $this->error = "Failed to decode JSON String for frequeny parameters for $pname: " . json_last_error_msg();
         return false;
      }

      // Now we get the current version. 
      $tup = new TableUpdates($this->con_main);
      $ans = $tup->getVersion($institution_id,$institution_instance);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Failed to version check.";
         $this->error = "Attempting to get version for $institution_id.$institution_instance: " . $tup->getError();
         return false;
      }

      if (empty($ans)){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Failed to version check.";
         $this->error = "Attempting to get version for $institution_id.$institution_instance, no version could be found";
         return false;
      }

      $current_version = $ans[0][TableUpdates::COL_VERSION_STRING];
      $for_eyetracker = $ans[0][TableUpdates::COL_EYETRACKER_KEY];

      // The presence of the update fields should be enough to trigger the updates. 
      if ($for_eyetracker != $pname){
         $ret[ResponseDataSubFields::UPDATE_ET_CHANGE] = $for_eyetracker;
         $ret[ResponseDataSubFields::UPDATE_VERSION] = $current_version;
      }
      else{
          if ($current_version != $version) {
              // We don't check for higher versions. Just different. This should allow rollbacks.
              $ret[ResponseDataSubFields::UPDATE_VERSION] = $current_version;
          }
      }

      return $ret;

   }

   function list($identifier,$parameters){

      if ($this->error != "") return false;

      // The $identifier in this case should be a portal user. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ti  = new TableInstitution($this->con_main);

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      // This only makes sense if the identifier is the same as the user.
      if ($portal_user != $identifier){
         $this->suggested_http_code = 401;
         $this->error = "Logged user $portal_user attempting to list institutions of $identifier";
         $this->returnable_error = "Illegal operation";
         return false;
      }

      $ans = $tiu->getInstitutionsForUser($identifier);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting institutions for user. Reason: " . $tiu->getError();
         $this->returnable_error = "Internal Server Error";
         return false;
      }

      $id_list = array();
      foreach ($ans as $inst_id_row){
         $id_list[] = $inst_id_row[TableInstitutionUsers::COL_INSTITUTION_ID];
      }

      if (empty($id_list)){
         $this->suggested_http_code = 401;
         $this->error = "No institutions found for user $identifier";
         return false;
      }

      // We get the actual institution information.
      $ans = $ti->getInstitutionInformationFor($id_list);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "Failed getting institution information for user. Reason: " . $ti->getError();
         $this->returnable_error = "Internal Server Error";
         return false;
      }

      return $ans;

   }

   function getupdate($identifier,$parameters){
      // We verify that the proper URL parameters were used. 
      $expected = [URLParameterNames::INSTANCE, URLParameterNames::REGION];

      foreach ($expected as $p) {
          if (!array_key_exists($p, $parameters)) {
              $this->suggested_http_code = 401;
              $this->returnable_error = "Missing url parameter";
              $this->error = "Getting the update requires the parameter $p";
              return false;
          }
      }

      $inst_number = $parameters[URLParameterNames::INSTANCE];
      $region      = $parameters[URLParameterNames::REGION];

      $institution_id = $_POST[POSTFields::INSTITUTION_ID];
      $institution_instance = $_POST[POSTFields::INSTITUION_INSTANCE];

      if (($identifier != $institution_id) || ($institution_instance != $inst_number)){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Mismatch between signature info and requested info";
         $this->error = "Asking for operating information from $identifier.$inst_number, however signature is from $institution_id.$institution_instance";
         return false;         
      }

      // Getting the update repo. 
      $update_repo = CONFIG[GlobalConfigUpdateResources::GROUP_NAME][GlobalConfigUpdateResources::UPDATE_REPO];

      // Getting the corresponding version. 
      $tup = new TableUpdates($this->con_main);

      $version_info = $tup->getVersion($institution_id,$inst_number);
      if ($version_info === false){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Failed to get update";
         $this->error = "Requesting update information for $identifier.$inst_number failed. Reason: " . $tup->getError();
         return false;           
      }

      if (empty($version_info)){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Failed to get update";
         $this->error = "Requesting update information for $identifier.$inst_number failed. Reason: no active record found in update table";
         return false;           
      }

      $version =  $version_info[0][TableUpdates::COL_VERSION_STRING];
      $etkey   =  $version_info[0][TableUpdates::COL_EYETRACKER_KEY];

      $this->file_path_to_return = $update_repo . "/$region/$etkey/$version/" . self::ApplicationFileName;

      if (!is_file($this->file_path_to_return)){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Failed to get update";
         $this->error = "Could not find requested update: " . $this->file_path_to_return;
         return false;           
      }

      return array();      

   }


}

?>