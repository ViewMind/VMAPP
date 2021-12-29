<?php

include_once (__DIR__ . "/../common/TypedString.php");

abstract class SelectColumnComparison extends TypedString {
   const IN                = "IN";
   const IN_NO_BIND        = "IN_NO_BIND";
   const EQUAL             = "=";
   const NOT_EQUAL         = "!=";
   const GT                = ">";
   const GTE               = ">=";
   const LT                = "<";
   const LTE               = "<=";  
   const ISNOTNULL         = "IS NOT NULL";
   const LIKE              = "LIKE";
   //const DATE_BETWEEN      = "BETWEEN";
   const DATE_DIFF_BETWEEN = "DATE_DIFF_BETWEEN";
}

abstract class SelectOrderDirection extends TypedString {
   const ASC  = "ASC";
   const DESC = "DESC";
   const RAND = "RAND()";
}

abstract class SelectExtras extends TypedString {
   const ORDER             = "order";
   const ORDER_DIRECTION   = "order_direction";
   const LIMIT             = "limit";
   const GROUPBY           = "groupby";
   const COUNT             = "count";
   const SUM               = "sum";
}

abstract class UnaryColumnOperationNode extends TypedString {
   const YEAR              = "YEAR";
   const MONTH             = "MONTH";
}

class SelectOperation {

   private const AND = "AND";
   private const OR  = "OR";

   private const NODE_VALUE             = "value";
   private const NODE_OPERATOR          = "operator";
   private const NODE_COLUMN            = "column";
   private const NODE_UNARY_OPERATOR    = "unary_operaton";

   private $operation;   
   private $column_appearce_counter;
   private $error;
   private $bind_param_array;
   private $selectDistinct;
   private $countDistinct;
   private $extras;

   function __construct(){
      $this->operation = array();
      $this->operation[self::AND] = array();
      $this->operation[self::OR]  = array();
      $this->column_appearce_counter = array();
      $this->bind_param_array = array();
      $this->error = "";
      $this->selectDistinct = "";
      $this->countDistinct = false;

      // Extras set up
      $this->extras[SelectExtras::COUNT] = array();
      $this->extras[SelectExtras::SUM]   = array();
      $this->extras[SelectExtras::LIMIT] = -1;
      $this->extras[SelectExtras::ORDER] = "";
      $this->extras[SelectExtras::ORDER_DIRECTION] = SelectOrderDirection::ASC;
      $this->extras[SelectExtras::GROUPBY] = array();

   }

   // Getting any of the extra parameters.
   function getExtra($extra){
      if (SelectExtras::validate($extra)){
         return $this->extras[$extra];
      }
      else return NULL;
   }

   function setExtra($name,$value){
      if (SelectExtras::validate($name)){
         return $this->extras[$name] = $value;
      }      
   }


   // Required for binding to the statement of the PDO.
   function getBindParameterArray(){
      return $this->bind_param_array;
   }
   
   // Required for checking that all columns actually belong to a table. 
   function getColumnsUsed(){
      return array_keys($this->column_appearce_counter);
   }

   function selectDistinct($distinct){
      $this->selectDistinct = $distinct;
   }

   function shouldUseDistinct(){
      return $this->selectDistinct;
   }

   function setCountDistinct(){
      $this->countDistinct = true;
   }

   function shouldCountDistinct(){
      return $this->countDistinct;
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
      if (empty($this->operation[self::OR]) && (empty($this->operation[self::AND]))) return "1"; // Returnign this is the same as saying all. 
      
      $this->bind_param_array = array();
      foreach ($this->column_appearce_counter as $col => $counter){
         $this->column_appearce_counter[$col] = 0;
      }

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
      $unary_col_operation = $node[self::NODE_UNARY_OPERATOR];

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
      else if ($node[self::NODE_OPERATOR] == SelectColumnComparison::IN_NO_BIND){
         // This should be selcted wthen the number of items to bind is too large. This will create the query straight up with no binding necessary. 
         $list = array();
         foreach ($node[self::NODE_VALUE] as $item) {
            $list[] = $item;
         }
         $operation = $col . " " . SelectColumnComparison::IN . " (" . implode(",",$list) . ")";
      }
      else if ($node[self::NODE_OPERATOR] === SelectColumnComparison::ISNOTNULL){
         // IN this case the value is ignored. Its just the column name not being null. 
         $operation = $col . " " . SelectColumnComparison::ISNOTNULL;
      }
      else if ($node[self::NODE_OPERATOR] == SelectColumnComparison::DATE_DIFF_BETWEEN){
         // This operation is only usefull for date type columns. It comapres the number of days between 
         $start_number_bind_name = ":$col" . "_" . $this->column_appearce_counter[$col];
         $this->column_appearce_counter[$col]++;
         $end_number_bind_name = ":$col" . "_" . $this->column_appearce_counter[$col];
         $this->bind_param_array[$start_number_bind_name] = $node[self::NODE_VALUE]["min"];
         $this->bind_param_array[$end_number_bind_name] = $node[self::NODE_VALUE]["max"];

         if (array_key_exists("date",$node[self::NODE_VALUE])){
            // When the date key exists comparison is done with the date provided.
            // We must assume that the date has the proper format: YYYY-MM-dd.
            $date = $node[self::NODE_VALUE]["date"];
         }
         else{
            // If date key is not provided, then the current date is used for comparison
            $date = "CURRENT_DATE()";
         }

         $operation = "(DATEDIFF($date, $col) BETWEEN $start_number_bind_name AND $end_number_bind_name)";

      }
      else{
         // When IN is not the operation the logic is do column_name = :column_name_i where i is the counter on how many times the column name has appeared in the statement. 
         $bind_name = ":$col" . "_" . $this->column_appearce_counter[$col];
         $this->bind_param_array[$bind_name] = $node[self::NODE_VALUE];
         $this->column_appearce_counter[$col]++;

         if ($unary_col_operation != ""){            
            $operation = "$unary_col_operation($col)" . " "  . $node[self::NODE_OPERATOR] . " " . $bind_name;
         }
         else{
            $operation = $col . " "  . $node[self::NODE_OPERATOR] . " " . $bind_name;
         }
      }

      $operation = "(" . $operation . ")";
      return $operation;
   }

   function addConditionToORList($operator, $column, $value, $unary_col_operation = ""){
      return $this->addConditionToList($operator,$column,$value,self::OR,$unary_col_operation);
   }

   function addConditionToANDList($operator, $column, $value, $unary_col_operation = ""){
      return $this->addConditionToList($operator,$column,$value,self::AND,$unary_col_operation);
   }

   private function addConditionToList($operator,$column,$value, $list, $unary_col_operation){
      $node = $this->addCondition($operator,$column,$value,$unary_col_operation);
      if ($node === false) return false;
      $this->operation[$list][] = $node;
      return true;
   }

   private function addCondition($operator, $column, $value, $unary_col_operation){

      // Checking for a valid operator. 
      if (!SelectColumnComparison::validate($operator)) {
         $this->error = "Invalid operator $operator";
         return false;
      }

      if ($unary_col_operation != ""){
         if (!UnaryColumnOperationNode::validate($unary_col_operation)){
            $this->error = "Invalid unary column operator $unary_col_operation for condition on column $column";
            return false;
         }
      }

      // If IN is used a value a array is necessary. 
      if (($operator == SelectColumnComparison::IN) || ($operator == SelectColumnComparison::IN_NO_BIND)){
         if (!is_array($value)){
            $this->error = "IN operator requires a value array";
            return false;
         }
      }
      else if ($operator == SelectColumnComparison::DATE_DIFF_BETWEEN){
         // This must be a two value number array where the first value must be higher than the second value. 
         if (!is_array($value)){
            $this->error = "DIFF_BETWEEN operator requires an object";
            return false;
         }

         // At te very list, the comparison configuration must have a min and max parameters
         $must_exist_keys = ["min","max"];
         foreach ($must_exist_keys as $key){
            if (!array_key_exists($key,$value)){
               $this->error = "DIFF_BETWEEN operator requires a the array to have the key $key";
               return false;
            }
         }

         $start = intval($value["min"]);
         $end   = intval($value["max"]);

         if ($start >= $end){
            $this->error = "Bad  range selected for column $column as $start >= $end";
            return false;
         }
      }
      else if (is_array($value)){
         $this->error = "$operator requires a non array value";
         return false;
      }

      $node[self::NODE_COLUMN]         = $column;   
      $node[self::NODE_VALUE]          = $value;
      $node[self::NODE_OPERATOR]       = $operator;
      $node[self::NODE_UNARY_OPERATOR] = $unary_col_operation;

      // Add the column to the list of all columns used in the where clause
      if (!array_key_exists($column, $this->column_appearce_counter)) {
         $this->column_appearce_counter[$column] = 0;
      }
      
      return $node;
   }

}

?>
