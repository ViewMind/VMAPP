<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableInstitution extends TableBaseClass {
   
   const TABLE_NAME               = "institution";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_INSTITUTION_NAME     = "institution_name";
   const COL_EMAIL                = "email";
   const COL_ADDRESS              = "address";
   const COL_STATE_OR_PROVINCE    = "state_or_province";
   const COL_COUNTRY              = "country";
   const COL_POSTAL_CODE          = "postal_code";
   const COL_ENABLED              = "enabled";
   const COL_PHONE_NUMBER         = "phone_number";

   function __construct(PDO $con){
      parent::__construct($con);
   }

   function createInstitution($params){
      
      // Basic verification. 
      $this->mandatory = [self::COL_INSTITUTION_NAME];
      $this->avoided = [self::COL_KEYID];

      return $this->insertionOperation($params,"Create Institution");

   }

   function exists($keyid){

      return $this->verifyExistsRow(self::COL_KEYID,$keyid);

   }

   function getInstitutionInformationFor($list_of_ids){

      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::IN,self::COL_KEYID,$list_of_ids)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      return $this->simpleSelect(array(),$select);

   }

   function getInstitutionNameMap(){
      $cols_to_get = [self::COL_KEYID, self::COL_INSTITUTION_NAME];
      $select = new SelectOperation();
      $ans = $this->simpleSelect($cols_to_get,$select);
      if ($ans === false) return false;
      $ret = array();
      foreach ($ans as $row){
         $ret[$row[self::COL_KEYID]] = $row[self::COL_INSTITUTION_NAME];
      }
      return $ret;
   }

}

?>