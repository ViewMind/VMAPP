<?php

include_once (__DIR__ . "/../common/TypedString.php");

abstract class SelectColumnComparison extends TypedString {
   const IN                = "IN";
   const EQUAL             = "=";
   const GT                = ">";
   const GTE               = ">=";
   const LT                = "<";
   const LTE               = "<=";   
}

class SelectOperation {

   private const AND = "AND";
   private const OR  = "OR";

   private const NODE_VALUE        = "value";
   private const NODE_OPERATOR     = "operator";
   private const NODE_COLUMN       = "column";

   private $operation;   
   private $column_appearce_counter;
   private $error;
   private $bind_param_array;

   function __construct(){
      $this->operation = array();
      $this->operation[self::AND] = array();
      $this->operation[self::OR]  = array();
      $this->column_appearce_counter = array();
      $this->bind_param_array = array();
      $this->error = "";
   }

   // Required for binding to the statement of the PDO.
   function getBindParameterArray(){
      return $this->bind_param_array;
   }
   
   // Required for checking that all columns actually belong to a table. 
   function getColumnsUsed(){
      return array_keys($this->column_appearce_counter);
   }

   // Any error meessage. 
   function getError(){
      return $this->error;
   }

   /**
    * The where clause that can be generated with this is limited to the form: 
    * WHERE () AND () AND () ... AND ( () OR () OR () OR )
    * Further flexibility would require a tree structure. 
    *
    * This function will create the where clause string with bind names and the binding structure that goes along with it. 
    */

   function makeWhereClause(){
      if (empty($this->operation[self::OR]) && (empty($this->operation[self::AND]))) return "";

      // The OR list is processed first. 
      $or_list = "";
      if (!empty($this->operation[self::OR])){
         $list = array();
         foreach ($this->operation[self::OR] as $node){
            $list[] = $this->makeOperation($node);
         }
         $or_list = implode(" " . self::OR . " ", $list);
         
      }

      $and_list = "";
      if (!empty($this->operation[self::AND])){
         $list = array();
         foreach ($this->operation[self::AND] as $node){
            $list[] = $this->makeOperation($node);
         }

         // IF there is an OR list it's added as simply one more operation. 
         if ($or_list != "") {
            $list[] = "( "  . $or_list . " )";
         }

         $and_list = implode(" " . self::AND . " ", $list);
      }

      if ($and_list == "") return $or_list;
      else return $and_list;

   }

   function makeOperation($node){
      $operation = "";
      $col = $node[self::NODE_COLUMN];

      if ($node[self::NODE_OPERATOR] == SelectColumnComparison::IN){
         // Create the binding list.
         $list = array();
         foreach ($node[self::NODE_VALUE] as $item){
            $bind_name = ":$col" . "_" . $this->column_appearce_counter[$col];
            $list[] = $bind_name;
            $this->bind_param_array[$bind_name] = $item;
            $this->column_appearce_counter[$col]++;
         }
         $operation = $col . " " . SelectColumnComparison::IN . " (" . implode(",",$list) . ")";
      }
      else{
         // When IN is not the operation the logic is do column_name = :column_name_i where i is the counter on how many times the column name has appeared in the statement. 
         $bind_name = ":$col" . "_" . $this->column_appearce_counter[$col];
         $this->bind_param_array[$bind_name] = $node[self::NODE_VALUE];
         $this->column_appearce_counter[$col]++;

         $operation = $col . " "  . $node[self::NODE_OPERATOR] . " " . $bind_name;
      }

      $operation = "(" . $operation . ")";
      return $operation;
   }

   function addConditionToORList($operator, $column, $value){
      return $this->addConditionToList($operator,$column,$value,self::OR);
   }

   function addConditionToANDList($operator, $column, $value){
      return $this->addConditionToList($operator,$column,$value,self::AND);
   }

   private function addConditionToList($operator,$column,$value, $list){
      $node = $this->addCondition($operator,$column,$value);
      if ($node === false) return false;
      $this->operation[$list][] = $node;
      return true;
   }

   private function addCondition($operator, $column, $value){

      // Checking for a valid operator. 
      if (!SelectColumnComparison::validate($operator)) {
         $this->error = "Invalid operator $operator";
         return false;
      }

      // If IN is used a value a array is necessary. 
      if ($operator == SelectColumnComparison::IN){
         if (!is_array($value)){
            $this->error = "IN operator requires a value array";
            return false;
         }
      }
      else if (is_array($value)){
         $this->error = "$operator requires a non array value";
         return false;
      }


      $node[self::NODE_COLUMN]   = $column;
      $node[self::NODE_VALUE]    = $value;
      $node[self::NODE_OPERATOR] = $operator;

      // Add the column to the list of all columns used in the where clause
      if (!array_key_exists($column, $this->column_appearce_counter)) {
         $this->column_appearce_counter[$column] = 0;
      }
      
      return $node;
   }

}

?>
