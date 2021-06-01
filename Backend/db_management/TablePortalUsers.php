<?php

include_once ("DBCon.php");
include_once (__DIR__ . "/../common/generate_token.php");
include_once (__DIR__ ."/../common/named_constants.php");
include_once ("TableBaseClass.php");

class TablePortalUsers extends TableBaseClass {
   
   const TABLE_NAME            =  "portal_users";
   const IN_DB                 =  DBCon::DB_SECURE;

   const COL_KEYID             = "keyid";
   const COL_NAME              = "name";
   const COL_LASTNAME          = "lastname";
   const COL_EMAIL             = "email";
   const COL_PASSWD            = "passwd";
   const COL_COMPANY           = "company";
   const COL_CREATION_TOKEN    = "creation_token";
   const COL_CREATION_DATE     = "creation_date";
   const COL_TOKEN             = "token";
   const COL_TOKEN_EXPIRATION  = "token_expiration";
   const COL_PERMISSIONS       = "permissions";
   const COL_USER_ROLE         = "user_role";
   const COL_ENABLED           = "enabled";

   // Enable values
   const ENABLED               = 1;
   const DISABLED              = 0;

   // The possible roles for a portal user. 
   const ROLE_INTITUTION_ADMIN = 0;
   const ROLE_MEDICAL          = 1;
   const ROLE_API_USER_ONLY    = 2;

   // String length for the activation token. 
   private const TOKEN_LENGTH  = 256;

   // Standard portal user permissions. 
   private const STANDARD_PORTAL_USER_PERMISSIONS = [
      APIEndpoints::INSTITUTION  => [ InstitutionOperations::LIST ],
      APIEndpoints::REPORTS      => [ ReportOperations::LIST, ReportOperations::GET ]
   ];

   function __construct($con){
      parent::__construct($con);
   }

   function addMedicalRoleUser($params){
      if (!array_key_exists(self::COL_PERMISSIONS,$params)){
         // If permissions weren't provided, then standard permissions are set. 
         $params[self::COL_PERMISSIONS] = json_encode(self::STANDARD_PORTAL_USER_PERMISSIONS);
      }
      $params[self::COL_USER_ROLE] = self::ROLE_MEDICAL;
      return $this->addPortalUser($params);
   }

   function addInstitutionAdminRole($params){
      if (!array_key_exists(self::COL_PERMISSIONS,$params)){
         // If permissions weren't provided, then standard permissions are set. 
         $params[self::COL_PERMISSIONS] = json_encode(self::STANDARD_PORTAL_USER_PERMISSIONS);
      }
      $params[self::COL_USER_ROLE] = self::ROLE_INTITUTION_ADMIN;
      return $this->addPortalUser($params);
   }

   function addAPIOnlyUser($params){
      // When creating an api user only, the permissions NEED to be provided. 
      $params[self::COL_USER_ROLE] = self::ROLE_API_USER_ONLY;
      return $this->addPortalUser($params);
   }

   private function addPortalUser($params){

      $this->mandatory = [self::COL_NAME, self::COL_LASTNAME, self::COL_EMAIL, self::COL_USER_ROLE, self::COL_PERMISSIONS];
      
      // When the user is created this function should create it's autentication token, and neither the enabled or the creation date can be touched.  
      $this->avoided = [self::COL_KEYID,self::COL_CREATION_DATE];

      // Generate the enabling token. 
      $token = generateToken(self::TOKEN_LENGTH);
      $params[self::COL_CREATION_TOKEN] = $token;
      $params[self::COL_ENABLED] = self::DISABLED;

      return $this->insertionOperation($params,"Adding a Portal User");

   }

   function getInfoForUser($email){
      $cols_to_get = array();
      $condition[self::COL_EMAIL] = $email;

      $operation = new SelectOperation();
      $ans = $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$email);
      if ($ans === false){
         $this->error = "Getting Info For User: " . $operation->getError();
         return false;
      }

      return $this->simpleSelect($cols_to_get,$operation);
   }

   function getAllNamesForIDList($id_list){
      $cols_to_get = [self::COL_KEYID, self::COL_EMAIL, self::COL_NAME, self::COL_LASTNAME];
      $operation = new SelectOperation();
      $ans = $operation->addConditionToANDList(SelectColumnComparison::IN,self::COL_KEYID,$id_list);
      if ($ans === false){
         $this->error = "Creating Portal User ID List: " . $operation->getError();
         return false;
      }
      return $this->simpleSelect($cols_to_get,$operation);
   }

   function confirmPortalUser($params){

   }

}
?>