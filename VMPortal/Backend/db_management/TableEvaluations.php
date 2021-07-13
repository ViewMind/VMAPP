<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableEvaluations extends TableBaseClass {

   const TABLE_NAME               = "evaluations";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_PORTAL_USER          =  "portal_user";
   const COL_STUDY_TYPE           =  "study_type";
   const COL_STUDY_DATE           =  "study_date";
   const COL_PROCESSING_DATE      =  "processing_date";
   const COL_INSTITUTION_ID       =  "institution_id";
   const COL_INSTITUTION_INSTANCE =  "institution_instance";
   const COL_SUBJECT_ID           =  "subject_id";
   const COL_EVALUATOR_EMAIL      =  "evaluator_email";
   const COL_EVALUATOR_NAME       =  "evaluator_name";
   const COL_EVALUATOR_LASTNAME   =  "evaluator_lastname";
   const COL_PROTOCOL             =  "protocol";
   const COL_RESULTS              =  "results";
   const COL_QC_PARAMS            =  "qc_parameters";
   const COL_QC_GRAPHS            =  "qc_graphs";
   const COL_STUDY_CONFIG         =  "study_configuration";
   const COL_FILE_LINK            =  "file_link";

   function __construct(PDO $con){
      parent::__construct($con);
   }

   function addEvaluation($params){
      
      // Basic verification. 
      $this->mandatory = [self::COL_PORTAL_USER, 
                          self::COL_STUDY_TYPE,
                          self::COL_STUDY_DATE,
                          self::COL_INSTITUTION_ID,
                          self::COL_INSTITUTION_INSTANCE,
                          self::COL_SUBJECT_ID,
                          self::COL_EVALUATOR_EMAIL,
                          self::COL_EVALUATOR_NAME,
                          self::COL_EVALUATOR_LASTNAME,
                          self::COL_PROTOCOL,
                          self::COL_FILE_LINK,
                          self::COL_STUDY_CONFIG,
                          self::COL_RESULTS];
      $this->avoided = [self::COL_KEYID, self::COL_PROCESSING_DATE];

      return $this->insertionOperation($params,"Add Evaluation");

   }

   function getAllSubjectsForInstitutionIDAndPortalUser($institution_id, $portal_user){

      $columns_to_get = [self::COL_SUBJECT_ID];
      $select = new SelectOperation();
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_INSTITUTION_ID,$institution_id);
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PORTAL_USER,$portal_user);
      $this->avoided = [];

      return $this->simpleSelect($columns_to_get,$select,self::COL_STUDY_DATE,self::ORDER_DESC);

   }

   function getAllEvaluationsForASubjectIDList($subject_id_list){

      $columns_to_get = [];
      $select = new SelectOperation();
      $select->addConditionToANDList(SelectColumnComparison::IN,self::COL_SUBJECT_ID,$subject_id_list);
      $this->avoided = [];

      return $this->simpleSelect($columns_to_get,$select,self::COL_STUDY_DATE,self::ORDER_DESC);

   }

   function getEvaluationListForSubjectAndPortalUser($subject_id, $portal_user){
      $columns_to_get = [self::COL_STUDY_TYPE, 
                         self::COL_STUDY_DATE, 
                         self::COL_PROCESSING_DATE,
                         self::COL_PROTOCOL,
                         self::COL_EVALUATOR_NAME,
                         self::COL_EVALUATOR_LASTNAME,
                         self::COL_EVALUATOR_EMAIL,
                         self::COL_KEYID];

      $select = new SelectOperation();
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_SUBJECT_ID,$subject_id);
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PORTAL_USER,$portal_user);
      $this->avoided = [];
      return $this->simpleSelect($columns_to_get,$select,self::COL_STUDY_DATE,self::ORDER_DESC);
   }

   function getEvaluation($keyid,$portal_user){
      $columns_to_get = [];

      $select = new SelectOperation();
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_KEYID,$keyid);
      $select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PORTAL_USER,$portal_user);

      $this->avoided = [];
      return $this->simpleSelect($columns_to_get,$select);
   }

}

?>