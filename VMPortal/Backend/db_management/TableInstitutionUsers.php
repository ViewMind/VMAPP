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

   function getUsersForInstitution($institution){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution)){
         $this->error = "Getting all users for institution $institution: " . $select->getError();
         return false;
      }
      return $this->simpleSelect(array(),$select);
   }

   function getInstitutionsForUser($user_id){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PORTAL_USER,$user_id)){
         $this->error = "Getting the insitution ids for user $user_id: " . $select->getError();
      }
      return $this->simpleSelect([self::COL_INSTITUTION_ID],$select);
   }

}
?>