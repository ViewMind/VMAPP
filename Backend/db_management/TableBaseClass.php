<?php

class TableBaseClass {

   // All tables will have keyid column.
   const COL_KEYID = "keyid";

   // All classes will have a constant identifying the table name and the database where they are located. 
   const TABLE_NAME = "table_name";
   const IN_DB      = "in_db";

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