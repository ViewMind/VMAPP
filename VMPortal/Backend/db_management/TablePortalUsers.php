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
   const PENDING               = 2;
   const DISABLED              = 0;

   // The possible roles for a portal user. 
   const ROLE_INTITUTION_ADMIN = 0;
   const ROLE_MEDICAL          = 1;
   const ROLE_API_USER_ONLY    = 2;

   // String length for the activation token. 
   private const TOKEN_LENGTH      = 512;
   private const AUTH_TOKEN_LENGTH = 512;
   private const AUTH_TOKEN_DURATION = 60; // minutes. 
   private const CREATION_LINK_MAX_TIME = 1; // hours

   private $generated_auth_token;

   // Standard portal user permissions. 
   private const STANDARD_PORTAL_USER_PERMISSIONS = [
      APIEndpoints::INSTITUTION  => [ InstitutionOperations::LIST ],
      APIEndpoints::REPORTS      => [ ReportOperations::LIST, ReportOperations::GET ],
      APIEndpoints::SUBJECTS     => [ SubjectOperations::LIST ],
      APIEndpoints::PORTAL_USERS => [ PortalUserOperations::MODIFY_OWN ]
   ];

   function __construct($con){
      parent::__construct($con);
   }


   function getGeneratedAuthToken(){
      return $this->generated_auth_token;
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
      $params[self::COL_ENABLED] = self::PENDING; // All users are created with a pending status. 

      $ans = $this->insertionOperation($params,"Adding a Portal User");
      if ($ans === FALSE) return;

      // We need to return the creation Token.
      $select = new SelectOperation();
      $cols_to_get = [self::COL_CREATION_TOKEN];
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$params[self::COL_EMAIL]);

      return $this->simpleSelect($cols_to_get,$select);


   }

   function getInfoForUser($email_or_id, bool $enabled_only){
      $cols_to_get = array();

      $operation = new SelectOperation();
      if (is_numeric($email_or_id)) {
         $ans = $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_KEYID,$email_or_id);
      }
      else{
         $ans = $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$email_or_id);
      }

      if ($enabled_only)
         $ans = $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ENABLED);

      if ($ans === false){
         $this->error = "Getting Info For User: " . $operation->getError();
         return false;
      }

      return $this->simpleSelect($cols_to_get,$operation);
   }

   function getAllNamesForIDList($id_list, $roles){
      $cols_to_get = [self::COL_KEYID, self::COL_EMAIL, self::COL_NAME, self::COL_LASTNAME, self::COL_USER_ROLE];
      $operation = new SelectOperation();
      $ans = $operation->addConditionToANDList(SelectColumnComparison::IN,self::COL_KEYID,$id_list);
      if (!empty($roles)) {
         // Roles are limited ONLY if the list is not empty. 
         $ans = $ans && $operation->addConditionToANDList(SelectColumnComparison::IN, self::COL_USER_ROLE, $roles);
      }
      if ($ans === false){
         $this->error = "Creating Portal User ID List: " . $operation->getError();
         return false;
      }
      return $this->simpleSelect($cols_to_get,$operation);
   }

   function setUserAuthToken($email){

      // We generate the token. 
      $token = generateToken(self::AUTH_TOKEN_LENGTH);
      $this->generated_auth_token = $token;

      // Computing the expiration date time
      $time = new DateTime();
      $time->add(new DateInterval('PT' . self::AUTH_TOKEN_DURATION . 'M'));
      $expiration = $time->format('Y-m-d H:i');

      $params[self::COL_TOKEN] = $token;
      $params[self::COL_TOKEN_EXPIRATION] = $expiration;

      // And updating the table. 
      return $this->updateOperation($params,"Auth token set",self::COL_EMAIL,$email);

   }

   function setUserPassword($user, $password){
      $params[self::COL_PASSWD] = $password;
      return $this->updateOperation($params,"Password Set",self::COL_EMAIL,$user);
   }

   function updateOwnUser($user, $params){

      // Ensure that the user critical values are NOT modified. 
      $this->avoided = [self::COL_CREATION_DATE, 
      self::COL_CREATION_TOKEN, self::COL_ENABLED, 
      self::COL_PERMISSIONS, self::COL_EMAIL, 
      self::COL_TOKEN_EXPIRATION,self::COL_USER_ROLE,
      self::COL_KEYID,self::COL_TOKEN];
      
      return $this->updateOperation($params,"Update Own User",self::COL_KEYID,$user);

   }

   function confirmPortalUser($email, $token){
      /// TODO: 
      /// 1) Check that the token corresponds to the email in the parameter and that the creation date is not too far off. 
      /// 2) If all OK in 1, then clear the creation token
      /// 3) Set the enabled flag from PENDING to ENABLE. 

      $info = $this->getInfoForUser($email,false);
      if ($info === FALSE){
         $this->error = "Could not get info on user $email for confirmation: " . $this->error;
         return false;
      }

      if (count($info) != 1){
         $this->error = "Could not get user info for $email. Does not appear to exist" ;
         return false;
      }

      $info = $info[0];

      // We check that the tokens are the same. 
      if ($token != $info[self::COL_CREATION_TOKEN]){
         $this->error = "Invalid authentication token for $email";
         return false;
      }


      // Now we check that the the tiem difference is okey. 
      $today = new DateTime();
      $creation_date = new DateTime($info[self::COL_CREATION_DATE]);
      $interval = $creation_date->diff($today);
      $diff_days   = $interval->format('%a');
      $diff_hours = $interval->format('%h');
      $diff_hours = ($diff_days*24+$diff_hours);

      if ($diff_hours > self::CREATION_LINK_MAX_TIME){         
         $this->error = "Token for $email expired as it was created $diff_hours hrs. ago";
         return false;
      }

      // All checks passed and so we can now set the token. 
      $params[self::COL_CREATION_TOKEN] = "";
      $params[self::COL_ENABLED] = self::ENABLED;

      return $this->updateOperation($params,"On Enable User",self::COL_EMAIL,$email);
      return true;

      // $datetime1 = new DateTime('2014-02-11 04:04:26 AM');
      // $datetime2 = new DateTime('2014-02-11 05:36:56 AM');
      // $interval = $datetime1->diff($datetime2);
      // echo $interval->format('%h')." Hours ".$interval->format('%i')." Minutes";      

   }

}
?>