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
   const COL_PARTNER_ID        = "partner_id";
   const COL_USER_ROLE         = "user_role";
   const COL_ENABLED           = "enabled";

   // Enable values
   const ENABLED               = 1;
   const PENDING               = 2;
   const DISABLED              = 0;
   const NOLOG                 = 3;

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
      APIEndpoints::PORTAL_USERS => [ PortalUserOperations::MODIFY_OWN, PortalUserOperations::LOGOUT]
   ];

   // Administrative Portal User Permissions
   private const ADMIN_PORTAL_USER_PERMISSIONS = [
      APIEndpoints::INSTITUTION  => [ InstitutionOperations::LIST ],
      APIEndpoints::REPORTS      => [ ReportOperations::LIST_ALL_OWN_INST, ReportOperations::GET_OWN_INSTITUTION ],
      APIEndpoints::SUBJECTS     => [ SubjectOperations::LIST_ALL_OWN_INST ],
      APIEndpoints::PORTAL_USERS => [ PortalUserOperations::MODIFY_OWN, PortalUserOperations::LOGOUT ]
   ];

   private const CLEAN_PERMISSIONS = [
      APIEndpoints::PORTAL_USERS => [ PortalUserOperations::MODIFY_OWN, PortalUserOperations::LOGOUT ]
   ];

   private const MED_REC_PERMISSIONS = [
      APIEndpoints::MEDICAL_RECORDS => [ MedRecordsOperations::MODIFY, MedRecordsOperations::GET, MedRecordsOperations::LIST ]
   ];

   private const MASTER_ADMIN_PERMISSIONS = [
      APIEndpoints::REPORTS => [ ReportOperations::ADMIN_LIST ]
   ];

   function __construct($con){
      parent::__construct($con);
   }

   static function getAdminPermissions(){
      return self::ADMIN_PORTAL_USER_PERMISSIONS;
   }

   static function getStandardPermissions(){
      return self::STANDARD_PORTAL_USER_PERMISSIONS;
   }

   static function getMedRecPermissions(){
      return self::MED_REC_PERMISSIONS;
   }

   static function getMasterAdminPermissions(){
      return self::MASTER_ADMIN_PERMISSIONS;
   }

   static function getCleanPermissions(){
      return self::CLEAN_PERMISSIONS;
   }

   function getPermissionsForEmailList($email_list){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::IN,self::COL_EMAIL,$email_list)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }      
      $cols_to_get = [self::COL_EMAIL,self::COL_PERMISSIONS];
      return $this->simpleSelect($cols_to_get,$select);
   }

   function setStandardUserPermissionsToUserList($user_list){
      return $this->setPermissionsToUserList($user_list,self::STANDARD_PORTAL_USER_PERMISSIONS);
   }

   function setAdminPortalPermissionsToUserList($user_list){
      return $this->setPermissionsToUserList($user_list,self::ADMIN_PORTAL_USER_PERMISSIONS);
   }

   function setPermissionsToUserList($user_list,$permissions){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::IN,self::COL_EMAIL,$user_list)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }      
      $params[self::COL_PERMISSIONS] = json_encode($permissions);
      return $this->simpleUpdate($params,"Updateing User Permissions by Email List",$select);
   }


   function getGeneratedAuthToken(){
      return $this->generated_auth_token;
   }

   // function addAPIUserOnly($params){
   //    $params[self::COL_USER_ROLE] = self::ROLE_API_USER_ONLY;
   //    $params[self::COL_ENABLED] = self::ENABLED;
   //    return $this->addPortalUser($params);
   // }
   
   // function addMedicalRoleUser($params){
   //    if (!array_key_exists(self::COL_PERMISSIONS,$params)){
   //       // If permissions weren't provided, then standard permissions are set. 
   //       $params[self::COL_PERMISSIONS] = json_encode(self::STANDARD_PORTAL_USER_PERMISSIONS);
   //    }
   //    $params[self::COL_USER_ROLE] = self::ROLE_MEDICAL;
   //    return $this->addPortalUser($params);
   // }

   // function addInstitutionAdminRole($params){
   //    if (!array_key_exists(self::COL_PERMISSIONS,$params)){
   //       // If permissions weren't provided, then standard permissions are set. 
   //       $params[self::COL_PERMISSIONS] = json_encode(self::STANDARD_PORTAL_USER_PERMISSIONS);
   //    }
   //    $params[self::COL_USER_ROLE] = self::ROLE_INTITUTION_ADMIN;
   //    return $this->addPortalUser($params);
   // }

   // function addAPIOnlyUser($params){
   //    // When creating an api user only, the permissions NEED to be provided. 
   //    $params[self::COL_USER_ROLE] = self::ROLE_API_USER_ONLY;
   //    return $this->addPortalUser($params,true);
   // }

   function addPortalUser($params, $force_enabled = false){

      $this->mandatory = [self::COL_NAME, self::COL_LASTNAME, self::COL_EMAIL, self::COL_USER_ROLE, self::COL_PERMISSIONS];
      
      // When the user is created this function should create it's autentication token, and neither the enabled or the creation date can be touched.  
      $this->avoided = [self::COL_KEYID,self::COL_CREATION_DATE];

      // Generate the enabling token. 
      $token = generateToken(self::TOKEN_LENGTH);
      $params[self::COL_CREATION_TOKEN] = $token;
      if ($force_enabled) $params[self::COL_ENABLED] = self::ENABLED;
      else $params[self::COL_ENABLED] = self::PENDING; // All users are created with a pending status. 

      $ans = $this->insertionOperation($params,"Adding a Portal User");
      if ($ans === false) return false;

      // We need to return the creation Token.
      $select = new SelectOperation();
      $cols_to_get = [self::COL_CREATION_TOKEN];
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$params[self::COL_EMAIL])){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      return $this->simpleSelect($cols_to_get,$select);


   }

   function getInfoForUser($email_or_id, bool $enabled_only){
      $cols_to_get = array();

      $operation = new SelectOperation();
      if (is_numeric($email_or_id)) {
         // This is so that we can query either via email or keyid. 
         if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_KEYID,$email_or_id)){
            $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
            return false;   
         }
      }
      else{
         if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$email_or_id)){
            $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
            return false;   
         }
      }

      if ($enabled_only) {
         if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL, self::COL_ENABLED, self::ENABLED)){
            $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
            return false;   
         }
      }

      return $this->simpleSelect($cols_to_get,$operation);
   }

   function getPPNameMap(){
      $cols_to_get = [self::COL_KEYID, self::COL_NAME, self::COL_LASTNAME];
      $select = new SelectOperation();
      $ans = $this->simpleSelect($cols_to_get,$select);
      if ($ans === false) return false;
      $ret = array();
      foreach ($ans as $row){
         $ret[$row[self::COL_KEYID]] = $row[self::COL_LASTNAME] . ", " . $row[self::COL_NAME];
      }
      return $ret;
   }

   function getAllNamesForIDList($id_list, $enabled_status){
      
      $cols_to_get = [self::COL_KEYID, self::COL_EMAIL, self::COL_NAME, self::COL_LASTNAME, self::COL_USER_ROLE];
      $operation = new SelectOperation();
      
      if (!$operation->addConditionToANDList(SelectColumnComparison::IN,self::COL_KEYID,$id_list)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;   
      }
            
      if (!empty($enabled_status)){
         // Enable status is limited ONLY if the list is not empty. 
         if (!$operation->addConditionToANDList(SelectColumnComparison::IN, self::COL_ENABLED, $enabled_status)){
            $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
            return false;   
         }
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

   function invalidateUserToken($keyid){
      $params[self::COL_TOKEN] = "";
      return $this->updateOperation($params,"Auth token set",self::COL_KEYID,$keyid);
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

   // function addNonLoginParterUsers($insert){
      
   //    $insert[self::COL_PERMISSIONS] = "{}";
   //    $insert[self::COL_USER_ROLE] = self::ROLE_MEDICAL;
   //    $insert[self::COL_ENABLED] = self::NOLOG;

   //    //var_dump($insert);

   //    $this->avoided = [self::COL_PASSWD, self::COL_TOKEN, self::COL_TOKEN_EXPIRATION, self::COL_CREATION_TOKEN, self::COL_KEYID];
   //    $this->mandatory = [self::COL_EMAIL, self::COL_PARTNER_ID, self::COL_LASTNAME, self::COL_NAME, self::COL_PERMISSIONS, self::COL_USER_ROLE, self::COL_ENABLED];

   //    return $this->insertionOperation($insert,"Inserting NonLoginPartner ",true);

   // }

   function getUsersPermission($unique_id){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_EMAIL,$unique_id)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;   
      }
      $cols_to_get = [self::COL_PERMISSIONS];
      return $this->simpleSelect($cols_to_get,$select);
   }
   
   // Make sure that this still makes sense. 
   function setUsersPermission($unique_id,$permissions,$role = -1){
      $params[self::COL_PERMISSIONS] = $permissions;
      if ($role != -1){
         $params[self::COL_USER_ROLE] = $role;
      }

      // This is done so that we can update on either mail or numerica ID acconding to convenience
      $col_on_update = self::COL_EMAIL;
      if (is_numeric($unique_id)){
         $col_on_update = self::COL_KEYID;
      }

      return $this->updateOperation($params,"Setting Portal User Permissions",$col_on_update,$unique_id);
   }

   function addPermissionsToAll($to_add){
      $select = new SelectOperation();
      $cols_to_get = [self::COL_KEYID, self::COL_PERMISSIONS];
      $rows = $this->simpleSelect($cols_to_get,$select);
      if ($rows === false) return false;

      $update = array();

      foreach ($rows as $row){
         $keyid = $row[self::COL_KEYID];
         $permissions = json_decode($row[self::COL_PERMISSIONS],true);
         //var_dump($to_add);
         foreach ($to_add as $object => $operations){
            if (!array_key_exists($object,$permissions)){
               $permissions[$object] = array();
            }
            //echo "BEFORE " . implode(",",$permissions[$object]) ."\n";
            $permissions[$object] = array_merge($permissions[$object],$operations);
            //echo "AFTER " . implode(",",$permissions[$object]) ."\n";
            $permissions[$object] = array_unique($permissions[$object]);
         }
         $update[$keyid] = json_encode($permissions);
      }

      return $this->batchUpdate($update,self::COL_KEYID,self::COL_PERMISSIONS);

   }

   function listEnabledUsers(){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ENABLED)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;   
      }
      $cols_to_get = [self::COL_NAME,self::COL_LASTNAME,self::COL_EMAIL,self::COL_KEYID];
      return $this->simpleSelect($cols_to_get,$select);
   }

}
?>