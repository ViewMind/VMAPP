<?php

   include_once (__DIR__ . "/../common/named_constants.php");
   include_once (__DIR__ . "/../common/echo_out.php");
   include_once (__DIR__ . "/../db_management/DBCon.php");
   include_once (__DIR__ . "/../db_management/TableSecrets.php");
   include_once (__DIR__ . "/../db_management/TablePortalUsers.php");

   class AuthManager {
      
      private $headers;      
      private $error;
      private $returnable_error;
      private $auth_type;
      private $login_type;
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
      private $should_do_operation;
      private $user_info;
      private $json_data;

      private const PERMISSION_KEYS = [

         FrontEndPermissions::SEE_ALL_OWN_INSTITUTION => [
            APIEndpoints::SUBJECTS => [SubjectOperations::LIST_ALL_OWN_INST],
            APIEndpoints::REPORTS => [ReportOperations::GET_OWN_INSTITUTION, ReportOperations::LIST_ALL_OWN_INST]
         ],

         FrontEndPermissions::LOAD_MEDICAL_RECORDS => [
            APIEndpoints::MEDICAL_RECORDS => [MedRecordsOperations::GET, MedRecordsOperations::LIST, MedRecordsOperations::MODIFY]
         ],

         FrontEndPermissions::ADMIN_DASHBOARD => [
            APIEndpoints::REPORTS => [ReportOperations::ADMIN_LIST]
         ]

      ];

      public function __construct($headers, $post_data, $files, &$raw_data){
         $this->error = "";
         $this->dbuser = "";
         $this->files = $files;
         $this->permissions = array();
         $this->http_code = 500; // If I forget the code, then it is a server error.
         $this->returnable_error = "";
         $this->should_do_operation = true;
         $this->user_info = array();


         //error_log(json_encode($post_data));
         //error_log(json_encode($raw_data));

         // This is to solve a problem where for an unknown reason AWS Apache changes the Header Casing and hence they are not recognized. 
         foreach ($headers as $headername => $value){
            $this->headers[strtolower($headername)] = $value;
         }
         $headers = $this->headers;
         
         // We need to verify that the authentication field is present .... 
         if (!array_key_exists(HeaderFields::AUTH_TYPE,$headers)){            
            $this->error = "Missing AuthType field in headers";
            $this->http_code = 200;
            return;
         }


         // We store the auth type and login type. The login type tells us which page 
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
                   $this->http_code = 401; // Authentication problem. 
                   $this->error = "Missing $header header for selected authentication method";
                   return;
               }
            }

            // Institution ID can camo from either POST fields or JSON data Fields.
            $array = [POSTFields::INSTITUTION_ID, POSTFields::INSTITUION_INSTANCE ];
            $missing_in_post = false;
            $missing_in_data = false;
            $raw_data_array = array();
            foreach ($array as $field) {
               if (!array_key_exists($field, $post_data)) {
                   $missing_in_post = true;
                   break;
               }
            }

            if ($missing_in_post){
               // We attempt to get the fields by parsing the raw data, which is required by some operation that send a JSON.
               $raw_data_array = json_decode($raw_data,true);
               if (is_array($raw_data_array)){
                  foreach ($array as $field) {
                     if (!array_key_exists($field, $raw_data_array)) {
                        $missing_in_data = true;
                        break;
                     }
                  }                        
               }
               else $missing_in_data = true;
            }

            if (!$missing_in_post){
               $this->institution_id = $post_data[POSTFields::INSTITUTION_ID];
               $this->institution_instance = $post_data[POSTFields::INSTITUION_INSTANCE];   
            }
            else if (!$missing_in_data){
               $this->institution_id = $raw_data_array[POSTFields::INSTITUTION_ID];
               $this->institution_instance = $raw_data_array[POSTFields::INSTITUION_INSTANCE];
               $this->json_data = $raw_data_array;
            }
            else{
               $this->http_code = 401; // Authentication problem. 
               $this->error = "Missing POST field $field for selected authentication method";
               return;
            }


            if (!is_numeric(($this->institution_id))){
               $this->http_code = 401;
               $this->error = "Invalid institution value " . $this->institution_id;
               return;
            }

            if (!is_numeric(($this->institution_instance))){
               $this->http_code = 401;
               $this->error = "Invalid institution instance " . $this->institution_instance;
               return;
            }

            $this->post_raw_data = json_encode($post_data);

         }

         else if ($this->auth_type == AuthValues::VMPARTNER){
            $authorization = $headers[HeaderFields::AUTHORIZATION];
            $parts = explode(":",$authorization);
            if (count($parts) != 2){
               $this->returnable_error = "Failed to authenticate user";
               $this->error = "Invalid format for authorization value for auth type VMPartner. Number of : separated parts: " . count($parts) . " instead of 2";
               $this->error = $this->error . "\nAuthorization String: $authorization";                
               return;               
            }
            $this->user_id = $parts[0];
            $this->token = $parts[1];
            $this->json_data = json_decode($raw_data,true);
         }
         else if ($this->auth_type == AuthValues::VMLOGIN){

            // And the same goes for the login mask
            if (!array_key_exists(HeaderFields::LOGIN_TYPE,$headers)){            
               $this->error = "Missing LoginType field in headers. Headers present are: " . implode(",",array_keys($headers));
               $this->returnable_error = "Unindetified type of login";
               $this->http_code = 403;
               return;
            }

            // The login type is used as a mask to enable different portal users login into different web apps. 
            $this->login_type = $headers[HeaderFields::LOGIN_TYPE];

            if (isset($_SERVER['PHP_AUTH_USER'])) {
               $this->username = $_SERVER['PHP_AUTH_USER'];
               $this->password = $_SERVER['PHP_AUTH_PW'];
            }
            else{
               $this->error = "Login AuthType was set but no username was found\n";
               $this->returnable_error = "Authentication failed";
               return;
            }
         }

      }

      function getSuggestedHTTPCode(){
         return $this->http_code;
      }

      function getError(){
         return $this->error;
      }

      function getReturnableError(){
         if ($this->returnable_error == "") return $this->error;
         return $this->returnable_error;
      }

      function getPermissions(){
         return $this->permissions;
      }

      function getServiceDBUser(){
         return $this->dbuser;
      }

      function shouldDoOperation(){
         return $this->should_do_operation;
      }

      function getAuthToken(){
         return $this->token;
      }

      function getUserInfo(){
         return $this->user_info;
      }

      function getStandarizedHeaders(){
         return $this->headers;
      }

      function getRawDataArray(){
         return $this->json_data;
      }

      function authenticate($message = ""){

         if ($this->error != "") return false;
         
         $this->http_code = 401; // Unauthorized by default. 
         
         if ($this->auth_type == AuthValues::VMCLIENT){
            return $this->authenticateSignature($message);
         }
         else if ($this->auth_type == AuthValues::VMLOGIN){
            $this->should_do_operation = false; // During login no operation is performed and the endopoint is ignored. 
            return $this->login();
         }
         else if ($this->auth_type == AuthValues::VMPARTNER){
            return $this->authenticateToken();
         }
         $this->error = "Unimplemented authentication method " . $this->auth_type;
         return false;

      }

      private function login(){
         
         // We need to get the password for comparison. 
         $dbcon = new DBCon();
         $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
         if ($con_secure == NULL){
            $this->http_code = 500;
            $this->error = "Error creating db connection: " . $dbcon->getError();
            $this->returnable_error = ReturnableError::DATABASE_CON;
            return false;
         }

         $tpu = new TablePortalUsers($con_secure);

         // Getting the user info for the appropiate email. 
         $ans = $tpu->getInfoForUser($this->username,true);
         if ($ans === FALSE){
            $this->http_code = 500;
            $this->error = "Error while getting information for " . $this->username . ": " . $tpu->getError();
            $this->returnable_error = "Invalid username or password";
            return false;
         }

         if (count($ans) != 1){
            $this->http_code = 401;
            $this->error = "Unknown user ". $this->username;
            $this->returnable_error = "Invalid username or password";
            return false;
         }

         $user_info = $ans[0];

         // User info will be returned. 
         $this->user_info = $user_info;

         // Checking the role value to see if the user can, in effect login to the requested site. 
         $role = intval($user_info[TablePortalUsers::COL_USER_ROLE]);
         $this->login_type = intval($this->login_type);
         error_log("Doing $role LOGIC AND " . $this->login_type . " Result is " . ($role & $this->login_type));
         if (($role & $this->login_type) !== $this->login_type){
            $this->http_code = 403;
            $this->error = "User " . $this->username . " is attempting to login to a website to which they are not allowed. Requestesd site: " . $this->login_type . " User role: $role";
            $this->returnable_error = "This user is not allowed to login to this site";
            return false;
         }

         $this->permissions = json_decode($user_info[TablePortalUsers::COL_PERMISSIONS],true);
         $this->user_info[ComplimentaryDataFields::PERMISSIONS] = $this->setUserPermission();

         // Checking that the user is a valid one. 
         if ($user_info[TablePortalUsers::COL_ENABLED] != TablePortalUsers::ENABLED){
            $this->http_code = 401;
            $this->error = "User ". $this->username . " is attempting to login but its enable status is not enabled but: " . $user_info[TablePortalUsers::COL_ENABLED];
            $this->returnable_error = "Invalid username or password";
            return false;
         }

         // Now verifying the password.
         if (!password_verify($this->password,$user_info[TablePortalUsers::COL_PASSWD])){
            $this->http_code = 401;
            $this->error = "Invalid password for user " . $this->username;
            $this->returnable_error = "Invalid username or password";
            return false;
         }

         // All went well so we generate the token to be returned. 
         $ans = $tpu->setUserAuthToken($this->username);
         if ($ans === FALSE){
            $this->http_code = 401;
            $this->error = "Could not set user " . $this->username . " auth token. Reason: " . $tpu->getError();
            $this->returnable_error = "Invalid username or password";
            return false;            
         }

         // Storing the token. 
         $this->token = $tpu->getGeneratedAuthToken();

         return true;

      }

      private function authenticateSignature(&$message){
         $signature = $this->headers[HeaderFields::SIGNATURE];
         $client_key = $this->headers[HeaderFields::AUTHENTICATION];
         $this->returnable_error = "";

         //echo $message;

         // We need to get the secret from the appropiate table. 
         $dbcon = new DBCon();
         $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_DP);
         if ($con_secure == NULL){
            $this->http_code = 500;
            $this->error = "Error creating db connection: " . $dbcon->getError();
            $this->returnable_error = ReturnableError::DATABASE_CON;
            return false;
         }

         $table_secrets = new TableSecrets($con_secure);
         //error_log("Getting secret key and permissions for " . $this->institution_id . "." . $this->institution_instance);
         $key_secret_permissions = $table_secrets->getKeySecretAndPermissions($this->institution_id,$this->institution_instance);
         //error_log("GOT: " . json_encode($key_secret_permissions));
         if ($key_secret_permissions === false){
            $this->http_code = 500;
            $this->error = "Could not get key and secret: " . $table_secrets->getError();
            $this->returnable_error = ReturnableError::DATABASE_QUERY;
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
            $this->returnable_error = ReturnableError::DATABASE_QUERY;
            return false;
         }

         if (!hash_equals($key,$client_key)){
            $this->http_code = 401; // Authentication problem. 
            $this->returnable_error = "Authentication failed due key mismatch";
            $this->error = "Authentication failed due key mismatch. DB Key: $key. Client provided key: $client_key from " . $this->institution_id . "." . $this->institution_instance;
            return false;
         }
         
         // Now we finallly compute the auth string. 

         // We first need to append the raw data of the post. But only if JSON data was not sent. Otherwise we are appending it twice. 
         if (empty($this->json_data)) {
             $message = $message . $this->post_raw_data;
         }

         // Then we append the raw file data. AND check no errors. If there are errors there is no point in moving on. 
         foreach ($this->files as $fkey => $fstruct){            
            
            if ($fstruct[FileStructNames::ERROR] != UPLOAD_ERR_OK){

               $this->error = "Failed to file upload. Reason: ";

               switch ($fstruct[FileStructNames::ERROR]){
                  case UPLOAD_ERR_CANT_WRITE:
                     $this->error = $this->error . "Failed to write upload to disk";
                     break;
                  case UPLOAD_ERR_INI_SIZE:
                     $this->error = $this->error . "The uploaded file exceeds the upload_max_filesize directive in php.ini";
                     break;
                  case UPLOAD_ERR_EXTENSION:
                     $this->error = $this->error . "PHP Extension stopped file upload";
                     break;
                  case UPLOAD_ERR_FORM_SIZE:
                     $this->error = $this->error . "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form";
                     break;
                  case UPLOAD_ERR_PARTIAL:
                     $this->error = $this->error . "The uploaded file was only partially uploaded";
                     break;
                  case UPLOAD_ERR_NO_TMP_DIR:
                     $this->error = $this->error . "Missing temporary folder for file upload";
                     break;
                  case UPLOAD_ERR_EXTENSION:
                     $this->error = $this->error . "PHP Extension stopped file upload";
                     break;
                  default:
                     $this->error = $this->error . ". Unknown error code: " . $fstruct[FileStructNames::ERROR];
                     break;
               }

               $this->returnable_error = "File upload failed";
               $this->http_code = 500;
               return false;
            }

            $message = $message . file_get_contents($fstruct[FileStructNames::TMP_LOCATION]);

         }

         //echoOut($message,true);
         //echo $message . "\n";

         $auth_string = hash_hmac('sha3-512', $message, $secret);
         if (!hash_equals($auth_string,$signature)){
            $this->http_code = 401; // Authentication problem. 
            $this->returnable_error = "Authentication failed due signature mismatch";
            $msg = "DB Key: $key\n";
            $msg = $msg . "Secret: $secret\n";
            $msg = $msg . "Received key: $client_key\n";
            $this->error = "KEY LOG:\n$msg";
            return false;
         }

         // Finally we set the db user. If the signature method is being used this is the definitely a VM cliet so that service is set. 
         $this->dbuser = DBCon::DB_SERVICE_DP;

         return true;
      }

      private function authenticateToken(){
         
         // We need to get the password for comparison. 
         $dbcon = new DBCon();
         $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
         if ($con_secure == NULL){
            $this->http_code = 500;
            $this->error = "Error creating db connection: " . $dbcon->getError();
            $this->returnable_error = ReturnableError::DATABASE_CON;
            return false;
         }

         $con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
         if ($con_main == NULL){
            $this->http_code = 500;
            $this->error = "Error creating db connection: " . $dbcon->getError();
            $this->returnable_error = ReturnableError::DATABASE_CON;
            return false;
         }


         $tu = new TablePortalUsers($con_secure);
         $ans = $tu->getInfoForUser($this->user_id,true);
         if ($ans === FALSE){
            $this->http_code = 500;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason: " . $tu->getError();
            return false;
         }

         //error_log("We got info for user");

         if (count($ans) != 1){
            $this->http_code = 401;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason could not find user with that id";
            return false;
         }

         $user_info = $ans[0];
         // Setting the permissions
         $this->permissions = json_decode($user_info[TablePortalUsers::COL_PERMISSIONS],true);
         // Setting the front end permissions
         $this->user_info[ComplimentaryDataFields::PERMISSIONS] = $this->setUserPermission();

         $tiu = new TableInstitutionUsers($con_main);
         $ans = $tiu->getInstitutionsForUser($user_info[TableBaseClass::COL_KEYID]);
         if ($ans === FALSE){
            $this->http_code = 500;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason: " . $tu->getError();
            return false;
         }

         if (count($ans) < 1){
            $this->http_code = 500;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason: " . $tiu->getError();
            return false;
         }

         // This way the user info has the associated institutions for it. 
         $user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS] = array();
         foreach ($ans as $row){
            $user_info[ComplimentaryDataFields::ASSOCIATED_INSTITUTIONS][] = $row[TableInstitutionUsers::COL_INSTITUTION_ID];
         }

         //error_log("Found user with id");

         // And storing any other user information that might be necessary. 
         $this->user_info = $user_info;

         if (json_last_error() != JSON_ERROR_NONE){
            $this->http_code = 500;
            $this->returnable_error = "Bad permissions";
            $this->error = "Failed to decode JSON for permissiosn for user " . $this->user_id . ". JSON string: " . $user_info[TablePortalUsers::COL_PERMISSIONS];
            return false;
         }
         $this->dbuser = DBCon::DB_SERVICE_PARTNERS;

         if (!hash_equals($user_info[TablePortalUsers::COL_TOKEN],$this->token)){
            $this->http_code = 401;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason: Invalid token";
            return false;
         }

         //error_log("Token checks");

         $expiration = new DateTime($user_info[TablePortalUsers::COL_TOKEN_EXPIRATION]);
         if (new DateTime() > $expiration){
            $this->http_code = 401;
            $this->error = "Failed in authentication of partner " . $this->user_id . ". Reason: Expired token";
            return false;
         }

         //error_log("Expiration checks");

         return true;

      }

      static function EnableUser($email,$authtoken){

         $dbcon = new DBCon();
         $con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
         
         $ans[ResponseFields::HTTP_CODE] = 200;
         $ans[ResponseFields::MESSAGE] = "";
         $ans[ResponseFields::CODE] = "";
         $ans[ResponseFields::DATA] = "";

         if ($con_secure == NULL){
            $ans[ResponseFields::HTTP_CODE] = 500;
            $ans[ResponseFields::CODE] = "Error creating db connection: " . $dbcon->getError();
            $ans[ResponseFields::MESSAGE] = "Internal server error";
            return $ans;
         }

         $tpu = new TablePortalUsers($con_secure);

         $ret = $tpu->confirmPortalUser($email,$authtoken);
         if ($ret === FALSE){
            $ans[ResponseFields::HTTP_CODE] = 401;
            $ans[ResponseFields::CODE] = "Could not confirm user $email. Reason: " . $tpu->getError();
            $ans[ResponseFields::MESSAGE] = $tpu->getError();
            return $ans;
         }

         return $ans;

      }

      /**
       * The permissions returned here as simply issued to determine what to show in the front end. 
       * The backend determines and verifies, each time, that an operation occurs, that the user actually can do what it requires.
       * The way verification is carried out is that each front end permission has an associated list of objects and operations for these objects.
       * If the DB permissions object has all of these for a given permission, then this front end permission is set to true. Otherwise it is set to false. 
       * The code was written witht he possibility that the requirements for a single  permission are multiple objects wiht multiple operations required for each object. 
       * 
       * WARNING: This function requires the the permissions object of THIS objects be set before being called. 
       * 
       */

      private function setUserPermission(){
         
         $permissions = array();

         //error_log(json_encode($this->permissions));

         foreach (self::PERMISSION_KEYS as $front_end_permission_name => $required_keys){
            
            $permissions[$front_end_permission_name] = true;
            
            foreach ($required_keys as $object_name => $operations_array){
               
               if (array_key_exists($object_name,$this->permissions)){
                  
                  foreach ($operations_array as $operation){
                     if (!in_array($operation,$this->permissions[$object_name])){
                        $permissions[$front_end_permission_name] = false;
                        //error_log("Operation '$operation' does not exist in object '$object_name'");
                        break;
                     }
                  }

               }

               else {
                  //error_log("Object '$object_name' does not exist in permissions");
                  $permissions[$front_end_permission_name] = false;
                  break;
               }

               if (!$permissions[$front_end_permission_name]) break;

            }
            
         }

         //error_log("Returning " . json_encode($permissions));

         return $permissions;

      }

   }
   


?> 
