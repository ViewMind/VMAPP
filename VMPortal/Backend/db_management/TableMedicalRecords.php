<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableMedicalRecords extends TableBaseClass {

   const TABLE_NAME               = "medical_records";
   const IN_DB                    = DBCon::DB_SECURE;

   const COL_NAME                 = "first_name";
   const COL_LAST_NAME            = "last_name";
   const COL_UNIQUE_ID            = "unique_id";
   const COL_VIEWMIND_ID          = "viewmind_id";
   const COL_INSTITUTION_ID       = "institution_id";
   const COL_SUBJECT_LINK         = "subject_link";
   const COL_RECORD               = "record";
   const COL_LAST_MOD_BY          = "last_mod";
   const COL_UPDATE               = "last_update";
   const COL_VALID                = "valid";

   const IS_LATEST                = 1;
   const IS_OLD                   = 0;

   function __construct(PDO $con){
      parent::__construct($con);
   }


   function getSubjectMedicalRecord($id){
      $cols_to_get = [];
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VIEWMIND_ID,$id)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      return $this->simpleSelect($cols_to_get,$select);
   }

   function newMedicalRecord($record){

      // We generate the unique id.
      $now = new DateTime();
      $viewmind_id = $now->format("Y_m_d_H_i_s_u");

      // Any missing data is assumed empty, with the exception of the viewmind_id and the institution id which must be present. 
      // But the viewmind id is generated. 
      $obligatory_data = [self::COL_NAME, self::COL_LAST_NAME, self::COL_UNIQUE_ID, self::COL_SUBJECT_LINK];

      $params = array();
      foreach ($obligatory_data as $col){
         if (!array_key_exists($col,$record)){
            $record[$col] = "";
         }
         $params[$col] = $record[$col];
      }
      $params[self::COL_RECORD] = $record;
      $params[self::COL_VIEWMIND_ID] = $viewmind_id;

      if (!array_key_exists(self::COL_INSTITUTION_ID,$record)){
         $this->error = "Cannot create medical record when column " . self::COL_INSTITUTION_ID . " is missing";
         return false;
      }
      $params[self::COL_INSTITUTION_ID] = $record[self::COL_INSTITUTION_ID];

      return $this->insertionOperation($params,"On Insertion of New Medical Record");

   }

   function getMedicalRecordsForInstitution($institution){
      $cols_to_get = [self::COL_NAME, self::COL_LAST_NAME, self::COL_UNIQUE_ID, self::COL_VIEWMIND_ID];
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_VALID,1)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }

      $select->setExtra(SelectExtras::ORDER,self::COL_LAST_NAME);
      $select->setExtra(SelectExtras::ORDER_DIRECTION,SelectOrderDirection::ASC);

      return $this->simpleSelect($cols_to_get,$select);          
   }

   function updateMedicalRecord($record){
      // Any missing data is assumed empty, with the exception of the viewmind_id
      $obligatory_data = [self::COL_NAME, self::COL_LAST_NAME, self::COL_UNIQUE_ID, self::COL_SUBJECT_LINK];

      // We must ensure these columns ARE NOT in the record.
      $cols_to_unset = [self::COL_KEYID, self::COL_VALID, self::COL_VIEWMIND_ID];

      if (!array_key_exists(self::COL_VIEWMIND_ID,$record)){
         $this->error = "Cannot update medical record when column " . self::COL_VIEWMIND_ID . " is missing";
         return false;
      }

      if (!array_key_exists(self::COL_INSTITUTION_ID,$record)){
         $this->error = "Cannot update medical record when column " . self::COL_INSTITUTION_ID . " is missing";
         return false;
      }


      $unique_id = $record[self::COL_VIEWMIND_ID];

      // Setting all values by default, if missing and creating the params struct for the query.
      $params = array();
      foreach ($obligatory_data as $col){
         if (!array_key_exists($col,$record)){
            $record[$col] = "";
         }
         $params[$col] = $record[$col];
      }
      $params[self::COL_RECORD] = $record;

      // Deleting unnncessary columns. 
      foreach ($cols_to_unset as $col){
         unset($record[$col]);
      }

      return $this->insertOnUpdatedInfo($params,$record[self::COL_VIEWMIND_ID],self::COL_VIEWMIND_ID,self::COL_VALID);
      
   }


}
?>