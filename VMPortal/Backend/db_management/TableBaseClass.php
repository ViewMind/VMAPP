<?php

include ("SelectOperation.php");
include_once (__DIR__ . "/../common/echo_out.php");

class TableBaseClass {

   // All tables will have keyid column.
   const COL_KEYID = "keyid";

   // All classes will have a constant identifying the table name and the database where they are located. 
   const TABLE_NAME = "table_name";
   const IN_DB      = "in_db";
   
   protected $REAL_DB_NAME;

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

   // The time consumed by the last query. 
   private $time_for_last_query;

   // The string for the last query issued.
   private $last_query;

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

      $this->time_for_last_query = 0;

   }

   function getLastQueryTime($include_query_string = false){
      if (!$include_query_string) return $this->time_for_last_query ;
      else return $this->time_for_last_query . " ms, for QUERY: " . $this->last_query;
   }

   function getError(){
      return $this->error;
   }

   function getLastInserted(){
      return $this->last_inserted;
   }

   function getLastQueryString(){
      return $this->last_query;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   
   /**
    * @brief A generic simple search that returls all columns of a table of a the rows matching the criteria. 
    * @param search_params is an object where each key is a column and each value is an array of objects, each with with two keys:
    * "comparator" and "value". This array represents all the conditions on the column, for the search. 
    */

   function search($search_params){

      $select = new SelectOperation();
      foreach ($search_params as $col => $comp_and_value_array){
         
         if (!is_array($comp_and_value_array)){
            $this->error = "Search operation requires each value of the object to be a two component object. For $col it's not an array.";
            return false;
         }

         foreach ($comp_and_value_array as $comp_and_value) {
            if (is_array($comp_and_value)) {
                if (!array_key_exists("comparator", $comp_and_value) || !array_key_exists("value", $comp_and_value)) {
                    $this->error = "Search operation requires each value of the object to be a two component object. For $col either 'comparator' or 'value' is missing";
                    return false;
                }
                $select->addConditionToANDList($comp_and_value["comparator"], $col, $comp_and_value["value"]);
            }
            else{
               $this->error = "Search ooperation requires that each value for search parameters be an array with two object. For $col one, of its arrays contained something other than an array.";
               return false;
            }
         }

      }

      return $this->simpleSelect([],$select);

   }    
   
   /**
    * Simple query to count the number of rows in a table.
    */

   public function countNumberOfRows(){
      $sql = "SELECT COUNT(*) AS cnt FROM " . static::class::TABLE_NAME;
      try {
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute();
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         $this->last_query = $sql;
         $row = $stmt->fetch();
         return $row["cnt"];
      }
      catch (PDOException $e){
         $this->error = "Failed counting the number of rows of table " . static::class::TABLE_NAME . ". Reason: "  . $e->getMessage() . ". SQL: $sql";
         return false;
      } 
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

            //echo count($p) . "\n";

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

            //echo "Not an array\n";

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


         $new_params = array();
         //echo "Count is $count\n";
         for ($i = 0; $i < $count; $i++){
            $temp = array();
            foreach ($columns_to_insert as $p) {
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
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $this->last_query = $sql;
         $this->time_for_last_query = (microtime(true) - $time)*1000;
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
         // Simple fix when the value to set is also the value on update
         
         $value_on_update_name = $col_on_update . "_toupdate";
         
         $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . implode(",", $updates) . " WHERE $col_on_update = :$value_on_update_name";
         $params[$value_on_update_name] = $value_on_update;

         //echo "UPDATE QUERY: $sql\n";
         //var_dump($params);
      }

      try {
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);         
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         $this->last_query = $sql;
         $this->last_inserted[] = $this->con->lastInsertId();
      }
      catch (PDOException $e){
         $this->error = "Insertion failure for $customized_message: " . $e->getMessage() . ". SQL: $sql";
         //var_dump($params);
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
      $nparams = array();
      foreach ($params as $col_name => $value){
         $updates[] = "$col_name = :$col_name";
         $nparams[":$col_name"] = $value;
      }

      $sql = "UPDATE " . static::class::TABLE_NAME . " SET " . implode(",", $updates) . " WHERE " . $select->makeWhereClause();

      $bind_from_select = $select->getBindParameterArray();
      $nparams = array_merge($bind_from_select,$nparams);
      //var_dump($nparams);

      try {
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute($nparams);
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         $this->last_query = $sql;
      }
      catch (PDOException $e){
         $this->error = "Update failure for $customized_message: " . $e->getMessage() . ". SQL: $sql";
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
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute($params);
         $row = $stmt->fetch(); // The query will only return one row, always. 
         $this->time_for_last_query = (microtime(true) - $time)*1000;
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
      $time = microtime(true);
      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute($bind);
         $ans = array();
         $this->last_query = $sql;
         while($row = $stmt->fetch()){
            $latest[] = $row;
         } 
      }
      catch (PDOException $e){
         $this->error = "Select failure: " . $e->getMessage() . ". SQL: $sql";
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         return false;
      }      

      // Checking there is only one answer:
      if (count($latest) > 1){
         $this->error = "Insert On Update Info: More than one row in " . static::class::TABLE_NAME . " for unique ID: $unique_item_id";
         $this->time_for_last_query = (microtime(true) - $time)*1000;
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
            $this->last_query = $sql;
            $stmt->execute($cols_to_compare);
            $this->last_inserted[] = $this->con->lastInsertId();
            $this->time_for_last_query = (microtime(true) - $time)*1000;
         }
         catch (PDOException $e){
            $this->error = "Insertion failure for Insert On Update Info: " . $e->getMessage() . ". SQL: $sql";
            $this->time_for_last_query = (microtime(true) - $time)*1000;
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
            $this->last_query = $sql;
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
    * SELECT operation on the table. The SelectOperation contains all the conditions plus extras that could be used such as count, sum, group, ordering, limiting, etc. 
    */

   protected function simpleSelect($cols_to_get, SelectOperation $operation){

      $this->error = "";

      // Getting the extras
      $count_struct      = $operation->getExtra(SelectExtras::COUNT);
      $sum_struct        = $operation->getExtra(SelectExtras::SUM);
      $group_by          = $operation->getExtra(SelectExtras::GROUPBY);
      $order_by          = $operation->getExtra(SelectExtras::ORDER);
      $order_direction   = $operation->getExtra(SelectExtras::ORDER_DIRECTION);
      $limit             = $operation->getExtra(SelectExtras::LIMIT);

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

      // If count struct is not empty, it should be an associative array which replaces the columns in $cols_to_get as COUNT(the_column) as NAME
      foreach ($count_struct as $col => $ref_name){
         $index = array_search($col,$cols_to_get);
         if ($index === false){
            $this->error = "COUNT column $col is not part of the requested columns";
            return false;
         }
         $cols_to_get[$index] = "COUNT($col) as $ref_name";
      }

      // If count struct is not empty, it should be an associative array which replaces the columns in $cols_to_get as SUM(the_column) as NAME
      foreach ($sum_struct as $col => $ref_name){
         $index = array_search($col,$cols_to_get);
         if ($index === false){
            $this->error = "SUM column $col is not part of the requested columns";
            return false;
         }
         $cols_to_get[$index] = "SUM($col) as $ref_name";
      }

      if (!empty($group_by)){
         foreach ($group_by as $g) {
             if (!in_array($g, $this->valid_columns)) {
                 $this->error = "grouping column $g is not a column of table " . static::class::TABLE_NAME;
                 return false;
             }
         }
         $group_by = implode(",",$group_by);
      }
      else $group_by = "";

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

      if ($group_by != ""){
         $sql = $sql . " GROUP BY $group_by ";
      }
      
      //error_log($sql);
      //error_log(json_encode($operation->getBindParameterArray()));

      // Adding ordering and limit parameters if issued. 
      if ($order_by != ""){
         if (!in_array($order_by,$this->valid_columns)){
            $this->error = "$order_by order by column is not a column of table " . static::class::TABLE_NAME;
            return false;
         }   
         if ($order_direction != SelectOrderDirection::RAND) {
            $sql = $sql . " ORDER BY  $order_by";
            if (($order_direction != SelectOrderDirection::ASC) && ($order_direction != SelectOrderDirection::DESC)) {
                $this->error = "Invalid order direction for ORDER BY: $order_direction " . static::class::TABLE_NAME;
                return false;
            }
            $sql = $sql . " " . $order_direction;
         }
         else{
            $sql = $sql . " ORDER BY RAND()";
         }

      } 
      if ($limit > -1){
         $sql = $sql . " LIMIT $limit";
      }

      //echoOut($sql,true);
      //echoOut($operation->getBindParameterArray(),true);

      try {
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute($operation->getBindParameterArray());
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         $this->last_query = $sql;
         //echo "QUERY: $sql\n";
         //var_dump($operation->getBindParameterArray());
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
    * @brief Makes sure all columns that should be there exist and that all columns that should NOT be there don't. 
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

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   
   /**
    * @brief Returns the size of the table in MB. This query does not require any parameters. 
    */

   public function getTableSize(){

      // Query to get the obituary table size in MB. 
      $sql = "SELECT TABLE_NAME AS " . static::class::TABLE_NAME . ", ROUND((DATA_LENGTH + INDEX_LENGTH) / 1024 / 1024) AS size FROM information_schema.TABLES WHERE TABLE_SCHEMA = '".
      $this->REAL_DB_NAME . "' AND TABLE_NAME = '" . static::class::TABLE_NAME . "'";

      try {
         $stmt = $this->con->prepare($sql);
         $stmt->execute();
         $ans = array();
         //echo "Getting table size is all good. SQL: $sql\n";
         while($row = $stmt->fetch()){
            $ans[] = $row;
         } 
         return $ans;
      }
      catch (PDOException $e){
         $this->error = "Get Table Size failure: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      

   }



}

?>
