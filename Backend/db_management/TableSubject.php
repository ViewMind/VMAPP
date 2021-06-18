<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableSubject extends TableBaseClass {

   const TABLE_NAME               = "subjects";
   const IN_DB                    = DBCon::DB_SECURE;

   const COL_NAME                 = "name";
   const COL_LAST_NAME            = "last_name";
   const COL_BIRTH_COUNTRY_CODE   = "birth_country_code";
   const COL_BIRTH_DATE           = "birth_date";
   const COL_AGE                  = "age";
   const COL_YEARS_FORMATION      = "years_formation";
   const COL_INTERNAL_ID          = "internal_id";
   const COL_INSTITUTION_ID       = "institution_id";
   const COL_INSTANCE_NUMBER      = "instance_number";
   const COL_UNIQUE_ID            = "unique_id";
   const COL_LASTEST              = "lastest";
   const COL_GENDER               = "gender";

   function __construct(PDO $con){
      parent::__construct($con);
   }

   static function mapSubjectFromVMDCToColumns($subject){
      $map = array();
      $map[SubjectField::NAME]                     = self::COL_NAME;
      $map[SubjectField::LASTNAME]                 = self::COL_LAST_NAME;
      $map[SubjectField::YEARS_FORMATION]          = self::COL_YEARS_FORMATION;
      $map[SubjectField::INSTITUTION_PROVIDED_ID ] = self::COL_INTERNAL_ID;
      $map[SubjectField::BIRTHDATE]                = self::COL_BIRTH_DATE;
      $map[SubjectField::BIRTH_COUNTRY]            = self::COL_BIRTH_COUNTRY_CODE;
      $map[SubjectField::GENDER]                   = self::COL_GENDER;
      $map[SubjectField::AGE]                      = self::COL_AGE;
      $map[SubjectField::LOCAL_ID]                 = self::COL_UNIQUE_ID;
      $new = array();
      foreach($subject as $name => $value){
         if (array_key_exists($name,$map)){
            $new[$map[$name]] = $value;
         }
      }
      return $new;
   }

   function addOrUpdateSubject($params){
      
      $columns_that_will_be_compared = [self::COL_NAME, 
      self::COL_LAST_NAME, 
      self::COL_BIRTH_COUNTRY_CODE, 
      self::COL_BIRTH_DATE, 
      self::COL_AGE, 
      self::COL_YEARS_FORMATION, 
      self::COL_GENDER,
      self::COL_INSTITUTION_ID,
      self::COL_INSTANCE_NUMBER,
      self::COL_INTERNAL_ID];

      $cols_to_check = $columns_that_will_be_compared;
      $cols_to_check[] = self::COL_UNIQUE_ID;
      $cols_to_compare = array();

      //echoOut($params,true);

      // We need to verify that all the columns were passed on in $params.
      foreach ($cols_to_check as $column){
         if (!array_key_exists($column,$params)){
            $this->error = "Adding/Modifying a Subject, column $column is missing";
            return false;
         }

         if (in_array($column, $columns_that_will_be_compared)) {
            if ($params[$column] == "") $cols_to_compare[$column] = NULL;
            else $cols_to_compare[$column] = $params[$column];
         }
      }

      //echoOut($cols_to_compare,true);

      $unique_subject_id = $params[self::COL_UNIQUE_ID];
      $unique_subject_column = self::COL_UNIQUE_ID;
      $latest_column = self::COL_LASTEST;

      //var_dump($cols_to_compare);

      return $this->insertOnUpdatedInfo($cols_to_compare,$unique_subject_id,$unique_subject_column,$latest_column);

   }

}

// // Creating the connections. 
// $dbcon = new DBCon();      
// $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_DP);
// if ($con_secure == NULL){
//    echo "Error creating db connection: " . $dbcon->getError() . "\n";
// }

// $ts = new TableSubject($con_secure);

// $params[TableSubject::COL_BIRTH_DATE] = "2020-01-01";
// $params[TableSubject::COL_GENDER] = "M";
// $params[TableSubject::COL_INSTANCE_NUMBER] = "0";
// $params[TableSubject::COL_INSTITUTION_ID] = "1000";
// $params[TableSubject::COL_INTERNAL_ID] = "Something Else";
// $params[TableSubject::COL_LAST_NAME] = "Smith Jr.";
// $params[TableSubject::COL_NAME] = "John";
// $params[TableSubject::COL_UNIQUE_ID] = "MyID0002";
// $params[TableSubject::COL_YEARS_FORMATION] = "0";
// $params[TableSubject::COL_BIRTH_COUNTRY_CODE] = "AR";
// $params[TableSubject::COL_AGE] = "7";

// $ans = $ts->addOrUpdateSubject($params);

// if ($ans === FALSE){
//    echo "Error: " . $ts->getError() . "\n";
//    return;
// }

// echo "All good\n";

?>