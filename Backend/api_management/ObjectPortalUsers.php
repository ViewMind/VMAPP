<?php

include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TablePortalUsers.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");

class ObjectPortalUsers extends ObjectBaseClass{


   function __construct($service){
      parent::__construct($service);
   }

   function getallmedical($identifier,$parameters){

      // WE first need to get the list of IDs form the Institution - User ID table. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ans = $tiu->getUsersForInstitution($identifier);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Getting ids for institution $identifier users: " . $tiu->getError();
         return false;
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
         $this->error = "Getting names for userids for institution $identifier: " . $tpu->getError();
         return false;
      }

      //echoOut($ans,true);

      // We create the final structure.
      foreach ($ans as $row){
         $ans[$row[TablePortalUsers::COL_KEYID]] = array();
         $ans[$row[TablePortalUsers::COL_KEYID]]["name"] = $row[TablePortalUsers::COL_NAME] . " " . $row[TablePortalUsers::COL_LASTNAME];
         $ans[$row[TablePortalUsers::COL_KEYID]]["email"] = $row[TablePortalUsers::COL_EMAIL];
      }

      return $ans;

   }


}

?>