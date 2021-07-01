<?php

include_once ("ObjectPortalUsers.php");
include_once (__DIR__ . "/../db_management/TableProcessingParameters.php");
include_once (__DIR__ . "/../db_management/TableInstitution.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");
include_once (__DIR__ . "/../common/named_constants.php");

class ObjectInstitution extends ObjectBaseClass{

   private ObjectPortalUsers $portal_users;

   function __construct($service,$headers){
      parent::__construct($service,$headers);
      $this->portal_users = new ObjectPortalUsers($service,$headers);
   }

   function operating_information($identifier,$parameters){

      // We verify that the proper URL parameters were used. 
      if (!array_key_exists(URLParameterNames::PPKEY,$parameters)){
         $this->suggested_http_code = 401;
         $this->returnable_error = "Missing url eyetracker parameter";
         $this->error = "operating information operation requires the parameter " . URLParameterNames::PPKEY;
         return false;
      }
      $pname = $parameters[URLParameterNames::PPKEY];

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

      return $ret;

   }

   function list($identifier,$parameters){

      if ($this->error != "") return false;

      // The $identifier in this case should be a portal user. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ti  = new TableInstitution($this->con_main);

      $ans = $tiu->getInstitutionsForUser($identifier);
      if ($ans === FALSE){
         $this->error = "Failed getting institutions for user. Reason: " . $tiu->getError();
         return false;
      }

      $id_list = array();
      foreach ($ans as $inst_id_row){
         $id_list[] = $inst_id_row[TableInstitutionUsers::COL_INSTITUTION_ID];
      }

      if (empty($id_list)){
         $this->error = "No institutions found for user $identifier";
         return false;
      }

      // We get the actual institution information.
      $ans = $ti->getInstitutionInformationFor($id_list);
      if ($ans === FALSE){
         $this->error = "Failed getting institution information for user. Reason: " . $ti->getError();
         return false;
      }

      return $ans;

   }


}

?>