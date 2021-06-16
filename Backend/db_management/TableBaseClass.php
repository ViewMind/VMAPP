<?php

include ("SelectOperation.php");
include_once (__DIR__ . "/../common/echo_out.php");

class TableBaseClass {

   // All tables will have keyid column.
   const COL_KEYID = "keyid";

   // All classes will have a constant identifying the table name and the database where they are located. 
   const TABLE_NAME = "table_name";
   const IN_DB      = "in_db";

   // Order directions
   const ORDER_DESC = "DESC";
   const ORDER_ASC  = "ASC";

   // A map that maps each column to a type required by the bind statement. Will be overwritten by each Child. 
   // const  COLUMN_TYPE_MAP = [];

   // A list of the columns fo the table.
   protected $valid_columns;

   // An array of the last inserted values.
   protected $last_inserted;

   // All tables need to be constructed with a connection to the database
   protected PDO $con;

   // The error message whenever it's produced. 
   protected $error;

   // A mandaroty list of column names which can be configured for each function. 
   protected $mandatory;

   // A list of column names that should NOT be specified for a given function.  
   protected $avoided;

   function __construct(PDO $con){
      $this->con = $con;      
      $this->error = "";
      $this->last_inserted = array();
      $this->avoided = array();
      $this->mandatory = array();

      // Generating the valid columns list. 
      $oClass = new ReflectionClass(static::class);
      $constant_names_and_values = $oClass->getConstants();
      $this->valid_columns = array();
      foreach ($constant_names_and_values as $name => $value){
         if (substr($name,0,4) == "COL_"){
            $this->valid_columns[] = $value;
         }
      }

   }

   function getError(){
      return $this->error;
   }

   function getLastInserted(){
      return $this->last_inserted;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
    * @brief A general insertion operation where params is an associative array of column names and values. 
    */

   protected function insertionOperation($params,$customized_message){

      $this->error = "";
      $this->last_inserted = array();
      
      if (!$this->validateInputArray($params,$customized_message)) return false;

      // Creating the placeholders for the statement as column names with : so that the straight params can be used. 
      $placeholders = array();
      $columns_to_insert = array_keys($params);
      foreach ($columns_to_insert as $p){
         $placeholders[] = ":$p";
      }

      // Creating the sql string.
      $sql =  "INSERT INTO " . static::class::TABLE_NAME .  " (" . implode(",",$columns_to_insert) . ") VALUES (" . implode(",",$placeholders) . ")";

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $this->last_inserted[] = $this->con->lastInsertId();
      }
      catch (PDOException $e){
         $this->error = "Insertion failure for $customized_message: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      
      
      // Insertion does not return anything, be default:
      return array();

   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
    * @brief Check if a row exists with a given value in a given column. 
    */

   protected function verifyExistsRow($col_to_search,$value){

      if (!in_array($col_to_search,$this->valid_columns)){
         $this->error = "Column name $col_to_search does not exists for " . static::class::TABLE_NAME;
         return false;
      }

      $this->error = "";
      $sql = "SELECT EXISTS(SELECT * from " . static::class::TABLE_NAME . " WHERE $col_to_search = :$col_to_search) AS result";

      $params = array();
      $params[$col_to_search] = $value;

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $row = $stmt->fetch(); // The query will only return one row, always. 
         return [$row["result"]];
      }
      catch (PDOException $e){
         $this->error = "Check exist failure: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      
      
      return true;      

   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
    * INSERTS only when the values are different from the current lastest information of the item. 
    */

   protected function insertOnUpdatedInfo($cols_to_compare,$unique_item_id,$unique_item_column,$latest_column){

      $this->error;

      $cols_to_check = array_keys($cols_to_compare);
      $cols_to_check[] = $unique_item_column;
      $cols_to_check[] = $latest_column;
      
      // Checking the validity of all the columns to get.
      foreach ($cols_to_compare as $col) {
         if (!in_array($col, $this->valid_columns)) {
            $this->error = "$col is not a column of table " . static::class::TABLE_NAME;
            return false;
         }
      }
      
      $cols_to_compare[] = self::COL_KEYID;

      // Doing a select query on the $unique_item_column assuming there is only one $latest column.
      $sql = "SELECT " . implode(",",$cols_to_compare) . " FROM " . static::class::TABLE_NAME . " WHERE " . $latest_column . " = :latest AND $unique_item_column = $:uid";
      
      $bind["latest"] = 1;
      $bind["uid"]    = $unique_item_id;
      
      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($bind);
         $ans = array();
         while($row = $stmt->fetch()){
            $latest[] = $row;
         } 
         return $ans;
      }
      catch (PDOException $e){
         $this->error = "Select failure: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      

      // Checking there is only one answer:
      if (count($latest) > 1){
         $this->error = "Insert On Update Info: More than one row in " . static::class::TABLE_NAME . " for unique ID: $unique_item_id";
         return false;
      }

      // Checking there is something, at all,
      $previous_keyid = "";
      // Default value of true in case there was nothing there before. 
      $should_insert = true;

      if (count($latest) == 1){

         // If we got here, there was something before. 
         $should_insert = false;

         // Getting the previous row identifier and clearing it from the array. 
         $previous_keyid = $latest[self::COL_KEYID];
         unset($latest[self::COL_KEYID]); 

         // Now we do the comparison. 
         foreach ($latest as $col => $value){
            if ($value != $cols_to_compare[$col]){
               $should_insert = true;
               break;
            }
         }

      }

      // We move onto the insertion, if necessary. 
      if ($should_insert) {
      
         // Creating the placeholders for the statement as column names with : so that the straight params can be used. 
         $placeholders = array();
         
         // We need to make sure that we insert the lastest value fo the column and the unique item id. 
         $columns_to_insert[$unique_item_column] = $unique_item_id;
         $columns_to_insert[$latest_column] = 1;

         $columns_to_insert = array_keys($cols_to_compare);
         foreach ($columns_to_insert as $p){
            $placeholders[] = ":$p";
         }
   
         // Creating the sql string.
         $sql =  "INSERT INTO " . static::class::TABLE_NAME .  " (" . implode(",",$columns_to_insert) . ") VALUES (" . implode(",",$placeholders) . ")";
   
         try {
            $stmt = $this->con->prepare($sql);
            $stmt->execute($cols_to_compare);
            $this->last_inserted[] = $this->con->lastInsertId();
         }
         catch (PDOException $e){
            $this->error = "Insertion failure for Insert On Update Info: " . $e->getMessage() . ". SQL: $sql";
            return false;
         }      
                  
      }

      // Finally, if there WAS an insertion AND there WAS previous information we insert the last row. 
      if ($should_insert && ($previous_keyid != "")){

         $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . $latest_column . " = :latest WHERE $unique_item_column = :uid";
         $bind["latest"] = 0;
         $bind["uid"]    = $previous_keyid;

         try {
            $stmt = $this->con->prepare($sql);
            $stmt->execute($bind);
            $ans = array();
            while($row = $stmt->fetch()){
               $latest[] = $row;
            } 
            return $ans;
         }
         catch (PDOException $e){
            $this->error = "Update failure on Insert On Update Inf: " . $e->getMessage() . ". SQL: $sql";
            return false;
         }         

      }

   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
    * SELECT operation on the table. 
    */

   protected function simpleSelect($cols_to_get, SelectOperation $operation, $order_by = "", $order_direction = "ASC" ,$limit = -1){

      $this->error = "";

      if (count($cols_to_get) == 0){
         // Empty column list means get all. 
         $cols_to_get = ["*"];
      }
      else{
          // Checking the validity of all the columns to get.
          foreach ($cols_to_get as $col) {
              if (!in_array($col, $this->valid_columns)) {
                  $this->error = "$col is not a column of table " . static::class::TABLE_NAME;
                  return false;
              }
          }
      }

      // Checking the validity of the columns in the where clause. 
      foreach ($operation->getColumnsUsed() as $col){
         if (!in_array($col,$this->valid_columns)){
            $this->error = "$col is not a column of table " . static::class::TABLE_NAME;
            return false;
         }
      }

      // Creating the SQL Statement. 
      $sql = "SELECT " . implode(",",$cols_to_get) . " FROM " . static::class::TABLE_NAME . " WHERE " . $operation->makeWhereClause();

      // Adding ordering and limit parameters if issued. 
      if ($order_by != ""){
         if (!in_array($order_by,$this->valid_columns)){
            $this->error = "$order_by order by column is not a column of table " . static::class::TABLE_NAME;
            return false;
         }   
         $sql = $sql . " ORDER BY  $order_by";  
         if (($order_direction != self::ORDER_ASC) && ($order_direction != self::ORDER_DESC)){
            $this->error = "Invalid order direction for ORDER BY: $order_direction " . static::class::TABLE_NAME;
            return false;
         }
         $sql = $sql . " " . $order_direction;  
      } 
      if ($limit > -1){
         $sql = $sql . " LIMIT $limit";
      }

      //echoOut($sql,true);
      //echoOut($operation->getBindParameterArray(),true);

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($operation->getBindParameterArray());
         $ans = array();
         while($row = $stmt->fetch()){
            $ans[] = $row;
         } 
         return $ans;
      }
      catch (PDOException $e){
         $this->error = "Select failure: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      
      
      return true;       
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   
   /**
    * @brief Makes sure all colums that should be there exist and that all columns that should NOT be there don't. 
    */

   protected function validateInputArray($associative_array_column_name_value, $operation_name){
      
      foreach ($associative_array_column_name_value as $col_name => $value){
      
         // We verify that the columns are a valid column name. 
         if (!in_array($col_name,$this->valid_columns)){
            $this->error = static::class::TABLE_NAME . " - $operation_name: invalid column: $col_name";
            return false;
         }

         // Then we verify that they are NOT in the list of column names to avoid.
         if (in_array($col_name,$this->avoided)){
            $this->error = static::class::TABLE_NAME . " - $operation_name: $col_name cannot be specified";
            return false;
         }

      }

      // Finally we make sure that all columns that should be present, are present. 
      foreach ($this->mandatory as $mandatory){
         if (!array_key_exists($mandatory,$associative_array_column_name_value)){
            $this->error = static::class::TABLE_NAME . " - $operation_name: $col_name should be specified but it's not present";
         }
      }

      return true;
   }

   


}

?>