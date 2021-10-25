<?php

include_once (__DIR__ . "/../common/generate_token.php");
include_once (__DIR__ . "/../common/named_constants.php");
include_once ("TableBaseClass.php");
include_once ("DBCon.php");

class TableAppPasswordRecovery extends TableBaseClass {

   const TABLE_NAME                = "app_password_recovery";
   const IN_DB                     = DBCon::DB_SECURE;
   
   const COL_INSTITUTION_ID        = "institution_id";
   const COL_INSTITUTION_INSTANCE  = "institution_instance";
   const COL_UNIQUE_ID             = "unique_id";
   const COL_PASSWORD_HASH         = "password_hash";

   private const PASSWORD_LENGTH   = 10;

   function __construct($con){
      parent::__construct($con);
   }

   function createPassword($institution_id, $instution_instance){

      $uppercase = range("A","Z");
      $lowercase = range("a","z");
      $numbers   = range(0,9);

      $all = array_merge($uppercase,$lowercase,$numbers);
      $N = count($all);

      // Making sure there is at least one of each type of character
      $pass_chars = array();
      $pass_chars[] = $uppercase[random_int(0,count($uppercase)-1)];
      $pass_chars[] = $lowercase[random_int(0,count($lowercase)-1)];
      $pass_chars[] = $numbers[random_int(0,count($numbers)-1)];

      while (count($pass_chars) < self::PASSWORD_LENGTH){
         $index = random_int(0,$N-1);
         $pass_chars[] = $all[$index];
      }

      // Assembling the randomly chonsen characters, randomly. 
      $password = "";
      while (count($pass_chars) > 0) {
         $index = random_int(0,count($pass_chars)-1);
         $password = $password . $pass_chars[$index];
         array_splice($pass_chars, $index, 1);
      }

      // Now the password is hashed. 
      $hash = hash('sha3-512', $password);

      $params[self::COL_INSTITUTION_ID] = $institution_id;
      $params[self::COL_INSTITUTION_INSTANCE] = $instution_instance;
      $params[self::COL_UNIQUE_ID] = "$institution_id.$instution_instance";
      $params[self::COL_PASSWORD_HASH] = $hash;
      
      $ans = $this->insertionOperation($params,"Inserting App Recovery Password");
      if ($ans === false) return false;
      else return $password;

   }

   function getInstancePassword($institution_id, $instution_instance){      
      $select = new SelectOperation();

      $uid = "$institution_id.$instution_instance";

      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_UNIQUE_ID,$uid)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      } 
      
      $select->setExtra(SelectExtras::ORDER,self::COL_KEYID);
      $select->setExtra(SelectExtras::ORDER_DIRECTION,SelectOrderDirection::DESC);
      $select->setExtra(SelectExtras::LIMIT,1);

      $cols_to_get = [self::COL_KEYID, self::COL_UNIQUE_ID, self::COL_PASSWORD_HASH];

      return $this->simpleSelect($cols_to_get,$select);

   }



}
?>