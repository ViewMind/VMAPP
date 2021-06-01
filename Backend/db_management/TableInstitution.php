<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableInstitution extends TableBaseClass {
   
   const TABLE_NAME               = "institution";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_INSTITUTION_NAME     = "institution_name";
   const COL_INSTITUTION_ID       = "institution_id";
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



}

?>