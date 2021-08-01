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

   // The permission might be required for any of the inhereted objects.
   protected $permissions;

   // The path of the file to return, if any. 
   protected $file_path_to_return;

   // Data sent with the request. 
   protected $json_data;

   function __construct($srv_name, $headers){
      $this->error = "";         
      $this->returnable_error = "";
      $this->suggested_http_code = 200;
      $this->file_path_to_return = "";

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

      // Saving the headers for use if necessary.
      $this->headers = $headers;

      // Storing the json_data (allready an array)
      $this->json_data = array();

   }

   function setJSONData($jdata){
      $this->json_data = $jdata;
   }

   function setPermissions($permissions){
      $this->permissions = $permissions;
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

   function getFileToReturn(){
      return $this->file_path_to_return;
   }
   
}

?>