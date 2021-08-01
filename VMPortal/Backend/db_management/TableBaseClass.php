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

   protected function insertionOperation($params,$customized_message, $ignore = false){

      $this->error = "";
      $this->last_inserted = array();
      
      if (!$this->validateInputArray($params,$customized_message)) return false;

      // There are two possible types of inputs. The first has a each element of $params be an array, while the second one is that they are a single value.       
      // Check the type
      // 0 - All single
      // 1 - All arrays single length
      // 2 - Invalid. 
      $valid = -1;
      $count = 0;
      foreach ($params as $key => $p){
         if (is_array($p)){
            if ($valid == -1) {
               $valid = 1;
               $count = count($p);
            }
            else if (($valid != 1) || (($count != count($p)))){
               $valid = 2;
               break;
            }
         }
         else{
            if ($valid == -1) {
               $valid = 0;
            }
            else if ($valid != 0){
               $valid = 2;
               break;
            }
         }
      }

      if ($valid == 2){
         $this->error = "Parameters objects need to be either all arrays of the same size or single values";
         return false;
      }

      // All is good. Setting up the ignore if necessary.
      // Creating the sql string.
      $ignore_str = "";
      if ($ignore){
         $ignore_str = "IGNORE";
      }      

      $placeholders = array();
      $columns_to_insert = array_keys($params);

      if ($valid == 0) {      
         // Creating the placeholders for the statement as column names with : so that the straight params can be used.
         foreach ($columns_to_insert as $p) {
             $placeholders[] = ":$p";
         }
         $sql =  "INSERT $ignore_str INTO " . static::class::TABLE_NAME .  " (" . implode(",", $columns_to_insert) . ") VALUES (" . implode(",", $placeholders) . ")";
      }
      else {

         // Each object in $params is an array in this case. is a list.
         $new_params = array();
         foreach ($columns_to_insert as $p) {
            $temp = array();
            for ($i = 0; $i < count($params[$p]); $i++){
               $new_name =  "$p" . "_" . $i;
               $temp[] = ":$new_name";
               $new_params[$new_name] = $params[$p][$i];
            }
            $placeholders[] = "(" . implode(",",$temp) . ")";
         }

         $sql =  "INSERT $ignore_str INTO " . static::class::TABLE_NAME .  " (" . implode(",", $columns_to_insert) . ") VALUES " . implode(",", $placeholders);
         $params = $new_params;
      }

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
    * @brief General update of a value set on a specific row value. If the $value_on_update is an array. It will update all rows where the values are those specified. 
    */

   protected function updateOperation($params,$customized_message,$col_on_update,$value_on_update){
      
      $this->error = "";
      $this->last_inserted = array();

      
      if (!$this->validateInputArray($params,$customized_message)) return false;
      
      if (!in_array($col_on_update,$this->valid_columns)){
         $this->error = "On update column $col_on_update is not a valid column for table " . static::class::TABLE_NAME;
         return false;
      }

      $updates = array();
      foreach ($params as $col_name => $value){
         $updates[] = "$col_name = :$col_name";
      }

      if (is_array($value_on_update)){
         // This means that we need to update several rows. 
         $list_to_update = array();
         $counter = 0;
         foreach ($value_on_update as $value){
            $name = $col_on_update . "_" . $counter;
            $list_to_update[] = ":$name";
            $params[$name] = $value;
            $counter++;
         }

         $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . implode(",", $updates) . " WHERE $col_on_update IN (" . implode(",",$list_to_update) . ")";
      }
      else{
         // Simple update. 1 row. 
         $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . implode(",", $updates) . " WHERE $col_on_update = :$col_on_update";
         $params[$col_on_update] = $value_on_update;
      }

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $this->last_inserted[] = $this->con->lastInsertId();
      }
      catch (PDOException $e){
         $this->error = "Insertion failure for $customized_message: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      
      
      // Insertion does not return anything
      return array();      
      
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
    * @brief General update of a value set on rows met by conditions given by a SelectOperation Object. 
    */

    protected function simpleUpdate($params,$customized_message,SelectOperation $select){
      
      $this->error = "";
      $this->last_inserted = array();

      
      if (!$this->validateInputArray($params,$customized_message)) return false;

      // Checking the validity of the columns in the where clause. 
      foreach ($select->getColumnsUsed() as $col){
         if (!in_array($col,$this->valid_columns)){
            $this->error = "$col is not a column of table " . static::class::TABLE_NAME;
            return false;
         }
      }

      $updates = array();
      foreach ($params as $col_name => $value){
         $updates[] = "$col_name = :$col_name";
      }

      $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . implode(",", $updates) . " WHERE " . $select->makeWhereClause();

      $bind_from_select = $select->getBindParameterArray();
      $params = array_merge($bind_from_select,$params);
      var_dump($params);

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $this->last_inserted[] = $this->con->lastInsertId();
      }
      catch (PDOException $e){
         $this->error = "Insertion failure for $customized_message: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      
      
      // Insertion does not return anything
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
      foreach ($cols_to_check as $col) {
         if (!in_array($col, $this->valid_columns)) {
            $this->error = "$col is not a column of table " . static::class::TABLE_NAME;
            return false;
         }
      }
      
      $cols_to_check[] = self::COL_KEYID;

      // Doing a select query on the $unique_item_column assuming there is only one $latest column.
      $sql = "SELECT " . implode(",",$cols_to_check) . " FROM " . static::class::TABLE_NAME 
      . " WHERE " . $latest_column . " = :latest AND $unique_item_column = :uid";
      
      $bind["latest"] = 1;
      $bind["uid"]    = $unique_item_id;
      
      $latest = array();
      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($bind);
         $ans = array();
         while($row = $stmt->fetch()){
            $latest[] = $row;
         } 
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

         // Transforming from one element list to associative array.
         $latest = $latest[0];

         // Getting the previous row identifier and clearing it from the array. 
         $previous_keyid = $latest[self::COL_KEYID];
         unset($latest[self::COL_KEYID]); 

         // Now we do the comparison. 
         foreach ($cols_to_compare as $col => $value){
            if ($value != $latest[$col]){
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
         $cols_to_compare[$unique_item_column] = $unique_item_id;
         $cols_to_compare[$latest_column] = 1;

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

         $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . $latest_column . " = :latest WHERE " . self::COL_KEYID . " = :keyid";
         
         $bind = array();
         $bind["latest"] = 0;
         $bind["keyid"]  = $previous_keyid;

         //echo "Doing $sql WITH Previous Keyid $previous_keyid\n";

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
      $distinct = "";
      if ($operation->shouldUseDistinct()){
         $distinct = " DISTINCT ";
      }

      $sql = "SELECT $distinct" . implode(",",$cols_to_get) . " FROM " . static::class::TABLE_NAME . " WHERE " . $operation->makeWhereClause();

      //error_log($sql);
      //error_log(json_encode($operation->getBindParameterArray()));

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