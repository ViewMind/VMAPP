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
   const COL_UNIQUE_ID             = "unique_id";
   const COL_PERMISSIONS           = "permissions";

   const ROW_ENABLED               = 1;
   const ROW_DISABLED              = 0;

   private const SECRET_KEY_LENGTH   = 128;
   private const SECRET_LENGTH       = 256;

   // Stadard permissions for a VMClient. 
   private const VMCLIENT_PERMISSIONS = [
      APIEndpoints::REPORTS      => [ ReportOperations::GENERATE ],
      APIEndpoints::INSTITUTION  => [ InstitutionOperations::OPR_INFO, InstitutionOperations::GET_UPDATE ],
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
      $p[self::COL_UNIQUE_ID] = "$institution_id.$instution_instance";
      if (empty($permissions)) {
         // If the permissions are empty, the default vm client permissions are given. 
         $p[self::COL_PERMISSIONS] = json_encode(self::VMCLIENT_PERMISSIONS);
      }

      $ans = $this->insertionOperation($p,"Creating new secret");
      if ($ans === false) return false;

      $ret[$key] = $secret;
      return $ret;

   }

   function getLastInstanceForInstitution($institution_id){

      $operation = new SelectOperation();
      
      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;
      }

      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution_id)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;
      }

      $operation->setExtra(SelectExtras::ORDER,self::COL_INSTITUTION_INSTANCE);
      $operation->setExtra(SelectExtras::ORDER_DIRECTION,SelectOrderDirection::DESC);
      $operation->setExtra(SelectExtras::LIMIT,1);

      $ans = $this->simpleSelect([self::COL_INSTITUTION_INSTANCE],$operation);
      if ($ans === false){
         $this->error = "Failed in getting the largest instance value for institution $institution_id: " . $this->error;
         return false;
      }

      return $ans[0][self::COL_INSTITUTION_INSTANCE];

   }

   function getKeySecretAndPermissions($institution_id, $instution_instance){      

      // There can't be another enabled row with the same instance and id. 

      $operation = new SelectOperation();
      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;
      }
      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution_id)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;
      }
      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_INSTANCE,$instution_instance)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
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


   function getPermissionsFrom($unique_ids){
      $cols_to_get = [self::COL_KEYID, self::COL_UNIQUE_ID , self::COL_PERMISSIONS];
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::IN,self::COL_UNIQUE_ID,$unique_ids)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      return $this->simpleSelect($cols_to_get,$select);
   }

   function resetBasicClientPermissions(){
      $params[self::COL_PERMISSIONS] = json_encode(self::VMCLIENT_PERMISSIONS);
      return $this->updateOperation($params,"On Resetting Secret's Basic Client Permissions Permissions",self::COL_ENABLED,self::ROW_ENABLED);
   }

   function getEnabledInstancesForInstitution($institution_id){

      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution_id)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      $cols_to_get = [self::COL_INSTITUTION_INSTANCE];
      return $this->simpleSelect($cols_to_get,$select);

   }

   function setPermissionOnUniqueID($permissions, $unique_id){
      $select = new SelectOperation();
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_ENABLED,self::ROW_ENABLED);
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_UNIQUE_ID,$unique_id);
      $params[self::COL_PERMISSIONS] = $permissions;
      return $this->simpleUpdate($params,"Update Secret Permissions",$select);
   }

}
?>