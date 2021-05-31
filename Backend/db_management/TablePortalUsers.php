<?php

include_once ("DBCon.php");
include_once ("../common/generate_token.php");
include_once ("TableBaseClass.php");

class TablePortalUsers extends TableBaseClass {
   
   const TABLE_NAME            =  "portal_users";
   const IN_DB                 =  DBCon::DB_MAIN;

   const COL_NAME              =  "name";
   const COL_LASTNAME          =  "lastname";
   const COL_EMAIL             =  "email";
   const COL_PASSWD            =  "passwd";
   const COL_CREATION_TOKEN    =  "creation_token";
   const COL_CREATION_DATE     =  "creation_date";
   const COL_USER_ROLE         =  "user_role";
   const COL_ENABLED           =  "enabled";

   // Enable values
   const ENABLED               = 1;
   const DISABLED              = 0;

   // The possible roles for a portal user. 
   const ROLE_INTITUTION_ADMIN = 0;
   const ROLE_MEDICAL          = 1;

   // String length for the activation token. 
   private const TOKEN_LENGTH  = 256;

   function __construct($con){
      parent::__construct($con);
   }

   function addMedicalRoleUser($params){
      $params[self::COL_USER_ROLE] = self::ROLE_MEDICAL;
      return $this->addPortalUser($params);
   }

   function addInstitutionAdminRole($params){
      $params[self::COL_USER_ROLE] = self::ROLE_INTITUTION_ADMIN;
      return $this->addPortalUser($params);
   }

   private function addPortalUser($params){

      $this->mandatory = [self::COL_NAME, self::COL_LASTNAME, self::COL_EMAIL, self::COL_USER_ROLE];
      
      // When the user is created this function should create it's autentication token, and neither the enabled or the creation date can be touched.  
      $this->avoided = [self::COL_KEYID,self::COL_CREATION_DATE];

      // Generate the enabling token. 
      $token = generateToken(self::TOKEN_LENGTH);
      $params[self::COL_CREATION_TOKEN] = $token;
      $params[self::COL_ENABLED] = self::DISABLED;

      return $this->insertionOperation($params,"Adding a Portal User");

   }

   function confirmPortalUser($params){

   }

}
?>