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
                          self::COL_RESULTS];
      $this->avoided = [self::COL_KEYID, self::COL_PROCESSING_DATE];

      return $this->insertionOperation($params,"Add Evaluation");

   }

}

?>