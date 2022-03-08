<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");
include_once (__DIR__ . "/../common/named_constants.php");

class TableUpdates extends TableBaseClass {
   
   const TABLE_NAME                = "updates";
   const IN_DB                     = DBCon::DB_MAIN;

   const COL_INSTITUTION_ID        = "institution_id";
   const COL_INSTITUTION_INSTANCE  = "institution_instance";
   const COL_UNIQUE_ID             = "unique_id";
   const COL_CREATION_DATE         = "creation_date";
   const COL_VERSION_STRING        = "version_string";
   const COL_EYETRACKER_KEY        = "eyetracker_key";
   const COL_VALID                 = "valid";

   function __construct($con){
      parent::__construct($con);
   }

   function addInstance($institution_id, $number, $version, $eyetracker_key){

      if ($version == ""){
         $this->error = "Can not add a new update instance with an empty version string";
         return false;
      }

      // First we form the unique key.
      $unique_key = $institution_id . "." . $number;

      // Now we check for presence. WE can't user insert on update, due to the $eyetracker_key not being present as option. IN that case we need to get it before we insert. 
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_UNIQUE_ID,$unique_key)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,"1")) { // The latest is marked by a 1 while all others are marked by a zero. 
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      $cols_to_get = [self::COL_KEYID,self::COL_VERSION_STRING,self::COL_EYETRACKER_KEY];

      $result = $this->simpleSelect($cols_to_get,$select);
      if ($result === FALSE){
         return false;
      }

      if (empty($result)){
         // This is a brand new insert. We need to make sure the $eyetracker_key exists and is valid.
         if ($eyetracker_key == ""){
            $this->error = "Cannot add a new Update row without an eyetracker key";
            return false;
         }

         // We check for a valid key. 
         if (!EyeTrackerCodes::validate($eyetracker_key)){
            $this->error = "$eyetracker_key is not a valid eyetracker key";
            return false;
         }      
         
         // Now we simply insert.
         $insert[self::COL_UNIQUE_ID] = $unique_key;
         $insert[self::COL_EYETRACKER_KEY] = $eyetracker_key;
         $insert[self::COL_INSTITUTION_ID] = $institution_id;
         $insert[self::COL_INSTITUTION_INSTANCE] = $number;
         $insert[self::COL_VERSION_STRING] = $version;
         $insert[self::COL_VALID] = 1;

      }
      else{

         // This is an update to a current row. If the values are different.
         if (count($result) != 1){
            $this->error = "Found more than one valid row on the update table for institution $institution_id and instance $number";
            return false;
         }

         $row = $result[0];
         $current_row_id = $row[self::COL_KEYID];
         $current_version = $row[self::COL_VERSION_STRING];
         $current_etkey = $row[self::COL_EYETRACKER_KEY];

         $update_version = false;
         $update_etkey = false;
         if ($current_version != $version) $update_version = true;
         
         // We now check if the et key needs an update as well. 
         if ($eyetracker_key != ""){
            // We check for a valid key. 
            if (!EyeTrackerCodes::validate($eyetracker_key)){
               $this->error = "$eyetracker_key is not a valid eyetracker key";
               return false;
            } 
            
            // Now we know we have a valid eyetracker key, we can compare.
            if ($eyetracker_key != $current_etkey) {
               $update_etkey = true;
            }            
         }

         // If neither value requires an update, there is nothing to do. 
         if ((!$update_etkey) && (!$update_version)) return array();

         // Otherwise we need to invalidate the last row. 
         $update[self::COL_VALID] = 0;
         $result = $this->updateOperation($update,"On Invalidating Lastest Update for $institution_id, instance $number",self::COL_KEYID,$current_row_id);
         if ($result === false) return false;

         // And we now prepare the insertion 
         $insert[self::COL_UNIQUE_ID] = $unique_key;
         
         if ($update_etkey) $insert[self::COL_EYETRACKER_KEY] = $eyetracker_key;
         else $insert[self::COL_EYETRACKER_KEY] = $current_etkey;

         if ($update_version) $insert[self::COL_VERSION_STRING] = $version;
         else $insert[self::COL_VERSION_STRING] = $current_version;
         
         $insert[self::COL_INSTITUTION_ID] = $institution_id;
         $insert[self::COL_INSTITUTION_INSTANCE] = $number;
         $insert[self::COL_VALID] = 1;

      }

      return $this->insertionOperation($insert,"On creating new Update Entry");

   }

   function getVersion($institution_id,$number){

      $select = new SelectOperation();
      $unique_key = $institution_id . "." . $number;
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_UNIQUE_ID,$unique_key)){         
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      $cols_to_get = [self::COL_VERSION_STRING,self::COL_EYETRACKER_KEY];
      return $this->simpleSelect($cols_to_get,$select);

   }

   function updateJustVersionOfList($update_list,$new_version){

      $cols_to_get = [self::COL_INSTITUTION_ID, self::COL_INSTITUTION_INSTANCE, self::COL_EYETRACKER_KEY, self::COL_UNIQUE_ID];
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::IN,self::COL_UNIQUE_ID,$update_list)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      // WE get all the values that we must reinsert before we make the same set of 
      $to_update = $this->simpleSelect($cols_to_get,$select);
      if ($to_update === FALSE) return false;


      // The exact same set of data is not set as invalid
      $params[self::COL_VALID] = 0;
      $ans = $this->simpleUpdate($params,"Updating old valid update rows to invalid",$select);
      if ($ans === FALSE) return false;

      // We reinsert the data with the new version and the valid value. 
      $update_params = array();
      foreach ($to_update as $item){
         foreach ($item as $col => $value) {
            if (!array_key_exists($col, $update_params)) {
               $update_params[$col] = array();
            }
            $update_params[$col][] = $value;
         }
         $update_params[self::COL_VALID][] = 1;
         $update_params[self::COL_VERSION_STRING][] = $new_version;
      }

      return $this->insertionOperation($update_params,"Updating to new version $new_version");

   }

   /**
    * Updating all valid requires
    * a) Getting a list of all valid institutions.
    * b) Copying all info of those rows chainging only the new version.
    * c) Updating all rows to be 0 
    * d) Inserting the copied rows with the new version. 
    */

   function updateAllValidToNewerVersion($new_version){

      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      $cols_to_get = [self::COL_INSTITUTION_ID,self::COL_INSTITUTION_INSTANCE,self::COL_UNIQUE_ID,self::COL_EYETRACKER_KEY];

      $ans = $this->simpleSelect($cols_to_get,$select);
      if ($ans === false){
         $this->error = "Failed getting the the valid updatable instances. Reason: " . $this->error;
         return false;
      }

      // Creating insersion array
      $cols_to_copy = [ self::COL_INSTITUTION_ID , self::COL_INSTITUTION_INSTANCE , self::COL_UNIQUE_ID, self::COL_EYETRACKER_KEY];

      $insertion_array[self::COL_INSTITUTION_ID] = array();
      $insertion_array[self::COL_INSTITUTION_INSTANCE] = array();
      $insertion_array[self::COL_UNIQUE_ID] = array();
      $insertion_array[self::COL_VERSION_STRING] = array();
      $insertion_array[self::COL_VALID] = array();
      $insertion_array[self::COL_EYETRACKER_KEY] = array();

      // Copying the information that will nto change in the new rows. 
      foreach ($ans as $row){
         //echo "Valid key at " . $row[self::COL_UNIQUE_ID] . "\n";
         foreach ($cols_to_copy as $col){            
            $insertion_array[$col][] = $row[$col];
         }
         $insertion_array[self::COL_VALID][] = 1;
         $insertion_array[self::COL_VERSION_STRING][] = $new_version;
      }

      //echo "INSERTION ARRAY\n";
      //var_dump($insertion_array);

      // Invalidating the old rows.
      $params[self::COL_VALID] = 0;      
      $ans = $this->updateOperation($params,"Invalidating current update rows",self::COL_VALID,1);   
      if ($ans === false) return false;      

      // Finally inserting the new rows.
      return $this->insertionOperation($insertion_array,"Inserting new rows for all valid update");

   }

   function getLatestVersion(){
      $select = new SelectOperation();
      $select->setExtra(SelectExtras::ORDER,self::COL_VERSION_STRING);
      $select->setExtra(SelectExtras::ORDER_DIRECTION,SelectOrderDirection::DESC);
      $select->setExtra(SelectExtras::LIMIT,1);
      $ans = $this->simpleSelect([self::COL_VERSION_STRING],$select);
      if ($ans === false) return false;
      else return $ans[0][self::COL_VERSION_STRING];
   }

   function listCurrentVersionForAllInstances(){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      return $this->simpleSelect([],$select);
   }

}

?>