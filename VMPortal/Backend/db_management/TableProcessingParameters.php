<?php

include_once ("DBCon.php");
include_once ("TableBaseClass.php");

class TableProcessingParameters extends TableBaseClass {
   
   const TABLE_NAME               = "product_processing_parameters";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_PRODUCT              = "product";
   const COL_PROC_PARAM           = "processing_parameters";
   const COL_FREQ_PARAM           = "qc_parameters";

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
      if (!$operation->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_PRODUCT,$pname)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $operation->getError();
         return false;                  
      }

      $operation->setExtra(SelectExtras::ORDER,self::COL_KEYID);
      $operation->setExtra(SelectExtras::ORDER_DIRECTION,SelectOrderDirection::DESC);
      $operation->setExtra(SelectExtras::LIMIT,1);

      // The current Processing parameters are always the last inserted.
      return $this->simpleSelect($cols_to_get,$operation);

   }



}

?>
