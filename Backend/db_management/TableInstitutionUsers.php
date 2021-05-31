<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableInstitutionUsers extends TableBaseClass {
   
   const TABLE_NAME               = "institution_users";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_PORTAL_USER          = "portal_user";
   const COL_INSTITUTION_ID       = "institution_id";

   function __construct($con){
      parent::__construct($con);
   }

   function linkUserToInstitution($user_id, $inst_id){            
      $params[self::COL_PORTAL_USER] = $user_id;
      $params[self::COL_INSTITUTION_ID] = $inst_id;
      return $this->insertionOperation($params,"Link portal user to institution");
   }

}
?>