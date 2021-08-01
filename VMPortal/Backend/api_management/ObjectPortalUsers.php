<?php

include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TablePortalUsers.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");

class ObjectPortalUsers extends ObjectBaseClass{


   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }

   function getallmedical($identifier,$parameters){

      if ($identifier == ""){
         $this->error = "Empty institution identifier.";
         return false;
      }

      // WE first need to get the list of IDs form the Institution - User ID table. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ans = $tiu->getUsersForInstitution($identifier);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get information on the users for this institution";
         $this->error = "Getting ids for institution $identifier users: " . $tiu->getError();
         return false;
      }
      // If the answer is empty, it is just that there are no users for this institution 
      if (empty($ans)){
         return array();
      }

      // Transforming result into a list of ids. 
      $user_ids = array();
      foreach ($ans as $row){
         $user_ids[] = $row[TableInstitutionUsers::COL_PORTAL_USER];
      }

      // Then we map those to the info. 
      $tpu = new TablePortalUsers($this->con_secure);
      $ans = $tpu->getAllNamesForIDList($user_ids,[TablePortalUsers::ROLE_MEDICAL, TablePortalUsers::ROLE_INTITUTION_ADMIN]);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get information on the users for this institution";
         $this->error = "Getting names for userids for institution $identifier: " . $tpu->getError();
         return false;
      }

      //echoOut($ans,true);

      // // We create the final structure.
      // $ret = array();
      // foreach ($ans as $row){
      //    $ret[$row[TablePortalUsers::COL_KEYID]] = array();
      //    $ret[$row[TablePortalUsers::COL_KEYID]]["name"] = $row[TablePortalUsers::COL_NAME] . " " . $row[TablePortalUsers::COL_LASTNAME];
      //    $ret[$row[TablePortalUsers::COL_KEYID]]["email"] = $row[TablePortalUsers::COL_EMAIL];
      // }

      return $ans;

   }

   function modify_own($identifier,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      if ($portal_user != $identifier){
         $this->suggested_http_code = 401;
         $this->error = "Attempting to modify own data identifier ($identifier) and portal user ($portal_user) mistamatch";
         $this->returnable_error = "Parameter error in API call";
         return false;
      }

      // Checking that only the allowed columns are modified. 
      $allowed_cols = [ TablePortalUsers::COL_COMPANY, TablePortalUsers::COL_PASSWD, TablePortalUsers::COL_NAME, TablePortalUsers::COL_LASTNAME ];
      foreach ($parameters as $key => $value){
         if (!in_array($key,$allowed_cols)){
            $this->suggested_http_code = 401;
            $this->error = "When modifying user $portal_user, cannot specify value for $key";
            $this->returnable_error = "Parameter error in API call";
            return false;            
         }
      }

      // Hashing the password if present. 
      if (array_key_exists(TablePortalUsers::COL_PASSWD,$parameters)){
         $password = password_hash($parameters[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);
         $parameters[TablePortalUsers::COL_PASSWD] = $password;
      }

      // Updating the table. 
      $tpu = new TablePortalUsers($this->con_secure);
      $ans = $tpu->updateOwnUser($portal_user,$parameters);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "When modifying user $portal_user, update failed. Reason: " + $tpu->getError();
         $this->returnable_error = "Internal Server Error";
         return false;         
      }

      return array();

   }

   function addnologpusers($identifier,$parameters){

      // First we make sure that the institution idenfier matches the signature institution.
      if (!array_key_exists(POSTFields::INSTITUTION_ID,$this->json_data)){
         $this->suggested_http_code = 401;
         $this->error = "Missing Institution ID in request data";
         $this->returnable_error = "Missing request data";
         return false;         
      }

      $institution_id = $this->json_data[POSTFields::INSTITUTION_ID];
      if ($institution_id != $identifier){
         $this->suggested_http_code = 401;
         $this->error = "Institution in URL was $identifier but the sign in was done with institution $institution_id";
         $this->returnable_error = "Badly formed URL";
         return false;         
      }

      if (!array_key_exists("data",$this->json_data)){
         $this->suggested_http_code = 401;
         $this->error = "Missing 'data' field in the  in request data";
         $this->returnable_error = "Missing request data";
         return false;         
      }      

      $medics = $this->json_data["data"];
      if (!is_array($medics)){
         $this->suggested_http_code = 401;
         $this->error = "The 'data' field in the request data is not an array";
         $this->returnable_error = "Bad request data";
         return false; 
      }

      // Thes are the columns expected in each item of the array;
      //$columns = [TablePortalUsers::COL_EMAIL, TablePortalUsers::COL_NAME, TablePortalUsers::COL_LASTNAME, TablePortalUsers::COL_PARTNER_ID];
      $tpu = new TablePortalUsers($this->con_secure);
      $tiu = new TableInstitutionUsers($this->con_main);

      $ret["inserted"] = array();

      foreach ($medics as $medic){

         //var_dump($medic);

         $ans = $tpu->addNonLoginParterUsers($medic);
         if ($ans === FALSE){
            $this->suggested_http_code = 500;
            $this->error = "Error inserting non login partner user. Reason: " . $tpu->getError();
            $this->returnable_error = "Internal server error";
            return false;    
         }
         
         $insert_id = $tpu->getLastInserted()[0];
         if ($insert_id == 0) continue; // The user has already been created. 

         $ans = $tiu->linkUserToInstitution($insert_id,$institution_id);
         if ($ans === FALSE){
            $this->suggested_http_code = 500;
            $this->error = "Error linking non login partner user $insert_id to institution $institution_id. Reason: " . $tiu->getError();
            $this->returnable_error = "Internal server error";
            return false;    
         }

         $ret["inserted"][] = $medic[TablePortalUsers::COL_EMAIL];

      }

      return $ret;

   }


}

?>