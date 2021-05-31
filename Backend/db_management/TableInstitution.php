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

   // const COLUMN_TYPE_MAP = [
   //    self::COL_KEYID             => "i",
   //    self::COL_INSTITUTION_NAME  => "s",
   //    self::COL_INSTITUTION_ID    => "i",
   //    self::COL_EMAIL             => "s",
   //    self::COL_ADDRESS           => "s",
   //    self::COL_STATE_OR_PROVINCE => "s",
   //    self::COL_COUNTRY           => "s",
   //    self::COL_POSTAL_CODE       => "s",
   //    self::COL_ENABLED           => "i",
   //    self::COL_PHONE_NUMBER      => "s"
   // ];

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