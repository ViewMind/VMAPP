<?php

   include_once (__DIR__ . "/../common/echo_out.php");

   class ObjectBaseClass{

      protected $error;

      // The code according to the error found
      protected $suggested_http_code;

      // The connections to each of the DBs
      protected $con_main;
      protected $con_secure;

      function __construct($srv_name){
         $this->error = "";         
         $this->suggested_http_code = 200;

         // Creating the connections. 
         $dbcon = new DBCon();      
         $this->con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,$srv_name);
         if ($this->con_secure == NULL){
            $this->error = "Error creating db connection: " . $dbcon->getError();
         }
   
         $this->con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,$srv_name);
         if ($this->con_main == NULL){
            $this->error = "Error creating db connection: " . $dbcon->getError();
         }         

      }

      function getSugesstedHTTPCode(){
         return $this->suggested_http_code;
      }

      function getError(){
         return $this->error;
      }

      protected function getConnectionToDatabase($db){

      }

   }

?>