<?php

include_once (__DIR__ . "/../common/generate_token.php");
include_once (__DIR__ . "/../common/named_constants.php");
include_once ("TableBaseClass.php");
include_once ("DBCon.php");

class TableSecrets extends TableBaseClass {

   const TABLE_NAME                = "secrets";
   const IN_DB                     = DBCon::DB_SECURE;
   
   const COL_SECRET_KEY            = "secret_key";
   const COL_INSTITUTION_ID        = "institution_id";
   const COL_INSTITUTION_INSTANCE  = "institution_instance";
   const COL_SECRET                = "secret";
   const COL_ENABLED               = "enabled";
   const COL_PERMISSIONS           = "permissions";

   const ROW_ENABLED               = 1;
   const ROW_DISABLED              = 0;

   private const SECRET_KEY_LENGTH   = 128;
   private const SECRET_LENGTH       = 256;

   // Stadard permissions for a VMClient. 
   private const VMCLIENT_PERMISSIONS = [
      APIEndpoints::PORTAL_USERS => [ PortalUserOperations::GETALLMEDICAL ],
      APIEndpoints::REPORTS      => [ ReportOperations::GENERATE ]
   ];   

   function __construct($con){
      parent::__construct($con);
   }

   function createNewSecret($institution_id, $instution_instance, $permissions = array()){

      $ans = $this->getKeySecretAndPermissions($institution_id,$instution_instance);
      if ($ans === false) return false;
      if (count($ans) == 1){
         $this->error = "There is an already enabled secret for this $institution_id and instance $instution_instance";
         return false;
      }

      $key = generateToken(self::SECRET_KEY_LENGTH);
      $secret = generateToken(self::SECRET_LENGTH);

      $p[self::COL_ENABLED] = self::ROW_ENABLED;
      $p[self::COL_INSTITUTION_ID] = $institution_id;
      $p[self::COL_INSTITUTION_INSTANCE] = $instution_instance;
      $p[self::COL_SECRET_KEY] = $key;
      $p[self::COL_SECRET] = $secret;
      if (empty($permissions)) {
         // If the permissions are empty, the default vm client permissions are given. 
         $p[self::COL_PERMISSIONS] = json_encode(self::VMCLIENT_PERMISSIONS);
      }

      $ans = $this->insertionOperation($p,"Creating new secret");
      if ($ans === false) return false;

      $ret[$key] = $secret;
      return $ret;

   }

   function getKeySecretAndPermissions($institution_id, $instution_instance){      

      // There can't be another enabled row with the same instance and id. 

      $operation = new SelectOperation();
      $res = true;
      $res = $res && $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED);
      $res = $res && $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution_id);
      $res = $res && $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_INSTANCE,$instution_instance);
      if ($res === false){
         $this->error = "Getting Secret, Keys and Permissions: " . $operation->getError();
         return false;
      }

      $required = [self::COL_SECRET, self::COL_SECRET_KEY, self::COL_PERMISSIONS];

      $ans = $this->simpleSelect($required,$operation);
      if ($ans === false){
         $this->error = "Checking for existante of key: " . $this->error;
         return false;
      }

      if (count($ans) > 1){
         $this->error = "Found more than one enabled secret which should not be possible";
         return false;
      }
      return $ans;

   }

}
?>