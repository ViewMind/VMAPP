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


}

?>