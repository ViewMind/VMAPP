<?php


class DBCon {

   // The Database services. 
   const DB_SERVICE_DP                     = "srv_dp";
   const DB_SERVICE_ADMIN                  = "srv_admin";
   const DB_SERVICE_PARTNERS               = "srv_partner";

   // The Databasse Names
   const DB_MAIN                           = "vm_main";
   const DB_SECURE                         = "vm_secure";

   // Constants describiing the credential file's structure.
   
   // Main fields
   const DBCNF_MAIN_SRV                    = "services";
   const DBCNF_MAIN_DBS                    = "databases";
   
   // Fields for the services main field
   const DBCNF_SRV_PASSWORD                = "password";
   const DBCNF_SRV_USERNAME                = "username";
   const DBCNF_SRV_DATABASES               = "databases";
    
   // Fields for the databases main field.
   const DBCNF_DB_HOST                     = "host";
   const DBCNF_DB_PORT                     = "port";


   // Variables
   private $credentials;
   private $error;

   // Constants. 
   private const CREDENTIALS_FILE             = "dbconfig.cnf";
   
   // Store the location where the credentials file will be located. 
   private static $POINTER_LOCATION;
   

   private const SRV_ARRAY = [self::DB_SERVICE_DP, 
                              self::DB_SERVICE_ADMIN,
                              self::DB_SERVICE_PARTNERS
   ];

   private const DB_ARRAY  = [self::DB_MAIN,
                              self::DB_SECURE
   ];

   private const DBMAIN_ARRAY = [ self::DBCNF_MAIN_SRV ,
                                  self::DBCNF_MAIN_DBS
   ];

   private const DBSRV_ARRAY = [ self::DBCNF_SRV_PASSWORD ,
                                 self::DBCNF_SRV_USERNAME ,
                                 self::DBCNF_SRV_DATABASES
   ];

   private const DBPARAM_ARRAY = [self::DBCNF_DB_HOST,
                                  self::DBCNF_DB_PORT
   ];
   
   function __construct()
   {
      $file_to_load = self::$POINTER_LOCATION . "/" . self::CREDENTIALS_FILE;

      // The file needs to be thoroughly checked, because if anything is missing it will break a lot of things. 
      if (!is_file($file_to_load)){
         $this->error = "Credentials file not found";
         return;
      }

      // Checking that the file can be correctly interpreted. 
      $this->credentials = json_decode(file_get_contents($file_to_load),true);
      if (!is_array($this->credentials)){
         $this->error = "Fail decoding credentials file: " . json_last_error_msg();
         return;
      }

      // Checking all the main fields are present. 
      foreach (self::DBMAIN_ARRAY as $main){
         if (!array_key_exists($main,$this->credentials)){
            $this->error = "Category $main is missing from credentials field";
            return;
         }
      }

      // Checking all the databases are present.
      foreach (self::DB_ARRAY as $db){
         if (!array_key_exists($db,$this->credentials[self::DBCNF_MAIN_DBS])){
            $this->error = "Missing configuration for database $db";
            return;
         }
      }

      // Checking all the services are present.
      foreach (self::SRV_ARRAY as $srv){
         if (!array_key_exists($srv,$this->credentials[self::DBCNF_MAIN_SRV])){
            $this->error = "Missing credentials for service $srv";
            return;
         }
      }

      // Checking all databases have their respective fields. 
      foreach ($this->credentials[self::DBCNF_MAIN_DBS] as $db => $db_con) {
         foreach (self::DBPARAM_ARRAY as $param) {
            if (!in_array($db,self::DB_ARRAY)){
               $this->error = "Unknown database configuration $db";
            }
            if (!array_key_exists($param, $db_con)) {
                $this->error = "Database configuration $db is missing the $param field";
            }
         }
      }

      // Checking all the services have their respective fields. We also check that all databases for them, are valid. 
      foreach ($this->credentials[self::DBCNF_MAIN_SRV] as $srv => $credentials){
         foreach (self::DBSRV_ARRAY as $conf) {
            if (!array_key_exists($conf,$credentials)){
               $this->error = "Credential field $conf is missing for service $srv";
               return;
            }
            if ($conf == self::DBCNF_SRV_DATABASES){
               foreach ($credentials[$conf] as $dbname){
                  if (!in_array($dbname,self::DB_ARRAY)){
                     $this->error = "Serivce $srv requieres connection to an unknown database: $dbname";
                     return;
                  }
               }
            }
         }
      }

      $this->error = "";
      
   }

   static function setPointerLocation($pl){
      self::$POINTER_LOCATION = $pl;
   }

   function getCredentials(){
      return $this->credentials;
   }

   function getError(){
      return $this->error;
   }

   function dbOpenConnection($dbname,$service){

      if ($this->error != "") return NULL;
   
      $this->error = "";
      
      if (array_key_exists($service,$this->credentials[self::DBCNF_MAIN_SRV])){
         $user   = $this->credentials[self::DBCNF_MAIN_SRV][$service][self::DBCNF_SRV_USERNAME];
         $passwd = $this->credentials[self::DBCNF_MAIN_SRV][$service][self::DBCNF_SRV_PASSWORD];
         $available_endpoints = $this->credentials[self::DBCNF_MAIN_SRV][$service][self::DBCNF_SRV_DATABASES];
      }
      else{
         $this->error = "DBOpenConnection: Unknown service: $service";
         return NULL;
      }
      
      if (in_array($dbname,$available_endpoints)){
         $host = $this->credentials[self::DBCNF_MAIN_DBS][$dbname][self::DBCNF_DB_HOST];
         $port = $this->credentials[self::DBCNF_MAIN_DBS][$dbname][self::DBCNF_DB_PORT];
      }
      else{
         $this->error = "DBOpenConnection: Service $service is unavailable for $dbname";
         return NULL;      
      }
      
      // Creating the connection. 
      $dsn = "mysql:host=$host;dbname=$dbname;charset=utf8mb4;port=$port";
      $options = [
         PDO::ATTR_ERRMODE                  => PDO::ERRMODE_EXCEPTION,
         PDO::ATTR_DEFAULT_FETCH_MODE       => PDO::FETCH_ASSOC,
         PDO::ATTR_EMULATE_PREPARES         => false
      ];
      try {
         $con = new PDO($dsn,$user,$passwd,$options);
      }
      catch (PDOException $e) {
         $this->error = "DBOpenConnection: Cannot establish connection to DB. Error code: " . $e->getCode() . ". Message: " . $e->getMessage();
         return NULL;
      }

      return $con;      
   }
   
   function dbTestConnections(){
      
      $report = array();
      
      foreach ($this->credentials[self::DBCNF_MAIN_SRV] as $srv_name => $srv_credentials){
      
         $dbs = $srv_credentials[self::DBCNF_SRV_DATABASES];
       
         foreach ($dbs as $db){
            
            $temp["error"] = "";
            $temp["status"] = "FAILED";
         
            $con = $this->dbOpenConnection($db,$srv_name);
            if ($con != NULL){
               $temp["error"] = $this->error;               
            }
            else{
               $temp["status"] = "PASSED";
               mysqli_close($con);
            }
            
            $report["DB: $db - SRV: $srv_name"] = $temp;
         
         }
      
      }      
      return $report;   
   }
}
         
?>
