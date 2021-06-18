<?php

include_once (__DIR__ . "/../common/echo_out.php");
include_once (__DIR__ . "/../common/named_constants.php");
include_once (__DIR__ . "/../common/log_manager.php");
include_once (__DIR__ ."./../common/config.php");

class ObjectBaseClass{

   protected $error;
   protected $returnable_error;

   // The code according to the error found
   protected $suggested_http_code;

   // The connections to each of the DBs
   protected $con_main;
   protected $con_secure;

   // This might contain valueable information required by the functions. 
   protected $headers;

   function __construct($srv_name, $headers){
      $this->error = "";         
      $this->returnable_error = "";
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

      // Creating the base log than can be used for genralities. 
      $this->headers = $headers;

   }
   
   function getSugesstedHTTPCode(){
      return $this->suggested_http_code;
   }
   
   function getError(){      
      return $this->error;
   }
   
   function getReturnableError() {
      if ($this->returnable_error == ""){
         return $this->error;
      }
      else return $this->returnable_error;
   }
   
   protected function getConnectionToDatabase($db){
   }
}

?>