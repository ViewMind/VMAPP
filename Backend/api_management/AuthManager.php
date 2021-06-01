<?php

   include_once (__DIR__ . "/../common/named_constants.php");
   include_once (__DIR__ . "/../common/echo_out.php");
   include_once (__DIR__ . "/../db_management/DBCon.php");
   include_once (__DIR__ . "/../db_management/TableSecrets.php");

   class AuthManager {
      
      private $headers;      
      private $error;
      private $auth_type;
      private $institution_id;
      private $institution_instance;
      private $username;
      private $password;
      private $token;
      private $files;
      private $http_code;
      private $post_raw_data;
      private $permissions;
      private $dbuser;

      const FILE_STRUCT_TMP_NAME           = "tmp_name";

      public function __construct($headers, $post_data, $files){
         $this->headers = $headers;
         $this->error = "";
         $this->dbuser = "";
         $this->files = $files;
         $this->permissions = array();
         
         // We need to verify that the authentication field is present .... 
         if (!array_key_exists(HeaderFields::AUTH_TYPE,$headers)){
            $this->error = "Missing AuthType field in headers";
            return;
         }

         // And that is an expected value
         $this->auth_type = $headers[HeaderFields::AUTH_TYPE];

         if (!AuthValues::validate($this->auth_type)){
            $this->error = "Invalid authetication type: " . $this->auth_type;
            return;
         }

         if ($this->auth_type == AuthValues::VMCLIENT){
            
            // We need a signature header in this case. 
            $array = [HeaderFields::SIGNATURE, HeaderFields::AUTHENTICATION ];
            foreach ($array as $header) {
               if (!array_key_exists($header, $headers)) {
                   $this->error = "Missing $header header for selected authentication method";
                   return;
               }
            }

            $array = [POSTFields::INSTITUTION_ID, POSTFields::INSTITUION_INSTANCE ];
            foreach ($array as $field) {
               if (!array_key_exists($field, $post_data)) {
                   $this->error = "Missing POST field $field for selected authentication method";
                   return;
               }
            }

            $this->institution_id = $post_data[POSTFields::INSTITUTION_ID];
            $this->institution_instance = $post_data[POSTFields::INSTITUION_INSTANCE];
            $this->post_raw_data = json_encode($post_data);

         }

         else if ($this->auth_type == AuthValues::VMPARTNER){

            /// TODO.

         }

      }

      function getSuggestedHTTPCode(){
         return $this->http_code;
      }

      function getError(){
         return $this->error;
      }

      function getPermissions(){
         return $this->permissions;
      }

      function getServiceDBUser(){
         return $this->dbuser;
      }

      // function setUsernameAndPassword($user,$pass){
      //    $this->username = $user;
      //    $this->password = $pass;
      // }

      // function setInstitutionParameters($institution_id,$institution_instance){
      //    $this->institution_instance = $institution_instance;
      //    $this->institution_id = $institution_id;
      // }

      // function setToken($token){
      //    $this->token = $token;
      // }

      function authenticate($message = ""){

         if ($this->error != "") return false;
         
         $this->http_code = 401; // Unauthorized by default. 
         
         if ($this->auth_type == AuthValues::VMCLIENT){
            return $this->authenticateSignature($message);
         }
         $this->error = "Unimplemented authentication method " . $this->auth_type;
         return false;

      }

      private function authenticateSignature(&$message){
         $signature = $this->headers[HeaderFields::SIGNATURE];
         $client_key = $this->headers[HeaderFields::AUTHENTICATION];

         // We need to get the secret from the appropiate table. 
         $dbcon = new DBCon();
         $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_DP);
         if ($con_secure == NULL){
            $this->http_code = 500;
            $this->error = "Error creating db connection: " . $dbcon->getError();
            return false;
         }

         $table_secrets = new TableSecrets($con_secure);
         $key_secret_permissions = $table_secrets->getKeySecretAndPermissions($this->institution_id,$this->institution_instance);
         if ($key_secret_permissions === false){
            $this->http_code = 500;
            $this->error = "Could not get key and secret: " . $table_secrets->getError();
            return false;
         }

         if (count($key_secret_permissions) == 0){
            // This means that all worked but no info was found, hence the intitution and instance are not registered. 
            $this->http_code = 400; // Client problem. 
            $this->error = "Institution " . $this->institution_id . " with instance " . $this->institution_instance . " are not registered";
            return false;
         }

         // If we got here, it means that the institution and instance they have a secret and key.
         $key = $key_secret_permissions[0][TableSecrets::COL_SECRET_KEY];
         $secret = $key_secret_permissions[0][TableSecrets::COL_SECRET];
         $this->permissions = json_decode($key_secret_permissions[0][TableSecrets::COL_PERMISSIONS],true);
         if ($this->permissions == NULL){
            $this->http_code = 500; // This should not happen. It's our problem. 
            $this->error = "Permissions array is badly formed. JSON Decode error: " . json_last_error_msg() . "\nPermissions String: " 
            . $key_secret_permissions[0][TableSecrets::COL_PERMISSIONS];
            return false;
         }

         if (!hash_equals($key,$client_key)){
            $this->http_code = 401; // Authentication problem. 
            $this->error = "Authentication failed due key mismatch";
            return false;
         }
         
         // Now we finallly compute the auth string. 

         // We first need to append the raw data of the post. 
         $message = $message . $this->post_raw_data;

         // Then we append the raw file data. 
         foreach ($this->files as $fkey => $fstruct){
            $message = $message . file_get_contents($fstruct[self::FILE_STRUCT_TMP_NAME]);
         }

         //echoOut($message,true);

         $auth_string = hash_hmac('sha3-512', $message, $secret);
         if (!hash_equals($auth_string,$signature)){
            $this->http_code = 401; // Authentication problem. 
            $this->error = "Authentication failed due signature mismatch";
            return false;
         }

         // Finally we set the db user. If the signature method is being used this is the definitely a VM cliet so that service is set. 
         $this->dbuser = DBCon::DB_SERVICE_DP;

         return true;
      }

   }
   


?> 
