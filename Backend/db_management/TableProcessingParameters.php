<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableProcessingParameters extends TableBaseClass {
   
   const TABLE_NAME               = "product_processing_parameters";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_PRODUCT              = "product";
   const COL_PROC_PARAM           = "processing_parameters";
   const COL_FREQ_PARAM           = "frequency_parameters";

   function __construct(PDO $con){
      parent::__construct($con);
   }

   function addProductParameters($pname, $pp, $fp){

      if (empty($pname)){
         $this->error = "Cannot add product parameters on empty name";
         return false;
      }
      
      $map[self::COL_PRODUCT] = $pname;
      $map[self::COL_FREQ_PARAM] = json_encode($fp);
      $map[self::COL_PROC_PARAM] = json_encode($pp);
      
      return $this->insertionOperation($map,"Add Product Parameters");

   }

   function getProductParameters($pname){

      $cols_to_get = [self::COL_PROC_PARAM, self::COL_FREQ_PARAM];
      $operation = new SelectOperation();
      $operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PRODUCT,$pname);

      // The current Processing parameters are always the last inserted.
      return $this->simpleSelect($cols_to_get,$operation,self::COL_KEYID,self::ORDER_DESC,1);

   }



}

?>
