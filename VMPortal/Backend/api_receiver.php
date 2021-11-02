<?php
header("Access-Control-Allow-Origin: *");  // Anyone can access
header("Content-Type: application/json; charset=UTF-8"); // Will return json data. 
header("Access-Control-Allow-Methods: POST");
header("Access-Control-Max-Age: 3600");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authentication, Authorization , AuthType");

// CORS INSANITY ..... Required for the preflight request. 
$requestMethod = $_SERVER['REQUEST_METHOD'];
// Is this a pre-flight request (the request method is OPTIONS)? Then start output buffering.
if ($requestMethod === 'OPTIONS') {
    ob_start();
}
// If this is a pre-flight request (the request method is OPTIONS)? Then flush the output buffer and exit.
if ($requestMethod === 'OPTIONS') {
    ob_end_flush();
    exit();
}
// END OF CORS INSANITY .....

//////////////////////////////////// INCLUDES ////////////////////////////////
include("./common/config.php");
if (empty(CONFIG)) return; // This means that there was an error loading the configuration file. 

include_once ("common/echo_out.php");
include_once ("common/log_manager.php");
include_once ("api_management/RouteParser.php");
include_once ("api_management/AuthManager.php");
include_once ("db_management/TableSecrets.php");
include_once ("api_management/ObjectPortalUsers.php");
include_once ("api_management/ObjectInstitution.php");
include_once ("api_management/ObjectReports.php");
include_once ("api_management/ObjectSubjects.php");
include_once ("api_management/ObjectMedicalRecord.php");

//////////////////////////////////// LOG SETUP ////////////////////////////////
$auth_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::AUTH_LOG_LOCATION]);
$auth_log->setSource($_SERVER['REMOTE_ADDR']);

$route_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::ROUTING_LOG_LOCATION]);
$route_log->setSource($_SERVER['REMOTE_ADDR']);

$base_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::BASE_STD_LOG]);
$base_log->setSource($_SERVER['REMOTE_ADDR']);

//////////////////////////////////// TOKENIZING THE ROUTE ////////////////////////////////
$route_parser = new RouteParser();

//error_log("REQUEST URL: " . $_SERVER['REQUEST_URI']);

if (!$route_parser->tokenizeURL($_SERVER['REQUEST_URI'],CONFIG[GlobalConfigGeneral::GROUP_NAME][GlobalConfigGeneral::API_PARTS_TO_REMOVE])){
   $error = $route_parser->getError();
   $code = 400; // This was a client error 
   $res[ResponseFields::MESSAGE] = $error;
   $route_log->logError($error);
   $res[ResponseFields::HTTP_CODE] = $code;
   http_response_code($code);
   echo json_encode($res);
   return;
}

//////////////////////////////////// CATCHING ENABLE USER REQUESTS ////////////////////////////////
$route_parts = $route_parser->getRouteParts();
//error_log("Created Route Parser: ");
//error_log(vardumpIntoVar($route_parser->getParameters()));


//error_log(json_encode($route_parts,JSON_PRETTY_PRINT));

if (count($route_parts) == 3){
   // Any number other than 3 it's not of interest to us. 
   if ($route_parts[0] == APIEndpoints::PORTAL_USERS){
      if ($route_parts[1] == PortalUserOperations::ENABLE){         
         $email = $route_parts[2];
         $parameters = $route_parser->getParameters();
         //var_dump($parameters);
         if (array_key_exists(URLParameterNames::ENABLE_TOKEN,$parameters)){
            $ans = AuthManager::EnableUser($email,$parameters[URLParameterNames::ENABLE_TOKEN]);
            if ($ans[ResponseFields::HTTP_CODE] != 200){
               $auth_log->logError("Failed to enable $email: Reason: " . $ans[ResponseFields::CODE]);
            }            
            http_response_code($ans[ResponseFields::HTTP_CODE]);
            echo json_encode($ans);
            return;            
         }         
      }
   }
}

//////////////////////////////////// AUTENTICATION OF REQUEST ////////////////////////////////
// Getting all headers. 
$headers = getallheaders();

// Generating the message in case it needs to be signed. 
$raw_data = file_get_contents("php://input");
//echo $raw_data . "\n";
$message = $raw_data . $route_parser->getEndpointAndParameters();

$auth_mng = new AuthManager($headers,$_POST,$_FILES,$raw_data);
if (!$auth_mng->authenticate($message)){
   $res[ResponseFields::MESSAGE] = $auth_mng->getReturnableError();
   $res[ResponseFields::HTTP_CODE] = $auth_mng->getSuggestedHTTPCode();
   // PATCH this is done to avoid filling the logs with the annoything missing AuthType Message from the AWS healthchecks. 
   if ($res[ResponseFields::HTTP_CODE] != 200) {
      $auth_log->logError($auth_mng->getError());
   }
   http_response_code($auth_mng->getSuggestedHTTPCode());
   echo json_encode($res);
   return;
}

// Standarized headers to lower case. 
$headers = $auth_mng->getStandarizedHeaders();
$user_info = $auth_mng->getUserInfo();
// if (!array_key_exists(TablePortalUsers::COL_KEYID,$user_info)){
//    $res[ResponseFields::MESSAGE] = "Failed to ascertain user identification";
//    $res[ResponseFields::HTTP_CODE] = $auth_mng->getSuggestedHTTPCode();   
//    $auth_log->logError("Unable to get DB user information. User information does not contain keyid");
//    http_response_code(500);
//    echo json_encode($res);
//    return;
// }

if (!$auth_mng->shouldDoOperation()){
   // The endopoint should be ignored. For now the only time this happens is when a login operation is used to 
   // generate the authentication token.
   $res[ResponseFields::DATA]["token"]       = $auth_mng->getAuthToken();
   $res[ResponseFields::DATA]["id"]          = $user_info[TablePortalUsers::COL_KEYID];
   $res[ResponseFields::DATA]["fname"]       = $user_info[TablePortalUsers::COL_NAME];
   $res[ResponseFields::DATA]["lname"]       = $user_info[TablePortalUsers::COL_LASTNAME];
   $res[ResponseFields::DATA]["permissions"] = $user_info[ComplimentaryDataFields::PERMISSIONS];
   $res[ResponseFields::MESSAGE] = "OK";
   $res[ResponseFields::HTTP_CODE] = 200;
   http_response_code(200);
   echo json_encode($res);
   return;
}


//////////////////////////////////// ROUTING ////////////////////////////////
$permissions     = $auth_mng->getPermissions();
$dbuser          = $auth_mng->getServiceDBUser();
$current_user_id = "";
if (array_key_exists(TablePortalUsers::COL_KEYID, $user_info)) {
   $current_user_id = $user_info[TablePortalUsers::COL_KEYID];
}

// ALL Endopoints are composed of 3 parts: 
// The first part is the OBJECT (the main target of whateve we are goona do)
// The second part is the OPERATION (what it is that we actually want to do)
// The third part is the IDENTIFIER (mainly onto what we are going to operate)

if (count($route_parts) != 3){
   // Badly formed endpoint. 
   $error = "Badly formed endpoint " . $route_parser->getEndpointAndParameters();
   $res[ResponseFields::MESSAGE] = $error;
   $route_log->logError($error . ". RouteParts: " . vardumpIntoVar($route_parts));
   $res[ResponseFields::HTTP_CODE] = 400; // This was a client error
   http_response_code(400);
   echo json_encode($res);
   return;      
}
$object     = $route_parts[0];
$operation  = $route_parts[1];
$identifier = $route_parts[2];

// error_log("ROUTE is $object / $operation / $identifier");

// Routes will have three parts OBJECT/OPERATION/IDENTFIER. So first is object. 
if (array_key_exists($object,$permissions)){
   
   // It means that there are permissions for this object. Next is the operation.

   // echoOut($permissions[$object]);

   if (in_array($operation,$permissions[$object])){

      if (!array_key_exists($object,ROUTING)){
         // Not implemented yer
         $res[ResponseFields::MESSAGE] = "Endpoint $object does not exist";
         $route_log->logError($res[ResponseFields::MESSAGE]);
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;         
      }

      // Creating an object of the proper class.
      $class = ROUTING[$object];
      $operating_object = new $class($dbuser,$headers, $auth_mng->getRawDataArray());

      if (!method_exists($operating_object,$operation)){
         // Method not implemented yet. 
         $res[ResponseFields::MESSAGE] = "Endpoint $object/$operation does not exist";
         $route_log->logError($res[ResponseFields::MESSAGE]);
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;   
      }

      // Calling the object and the operation with the data, permissions and user identification (keyid, not username/email)

      $operating_object->setPermissions($permissions);
      $operating_object->setJSONData($auth_mng->getRawDataArray());
      $operating_object->setPortalUserUID($current_user_id);
      $operating_object->setPortalUserInfo($user_info);

      $ans = $operating_object->$operation($identifier,$route_parser->getParameters());
      
      if ($ans === false){
         // Something went wrong. 
         // error_log("Returning when something went wrong. Sending code: " . $operating_object->getSugesstedHTTPCode() . " and message " . $operating_object->getReturnableError());
         $base_log->logError($operating_object->getError());
         $res[ResponseFields::MESSAGE] = $operating_object->getReturnableError();
         $res[ResponseFields::HTTP_CODE] = $operating_object->getSugesstedHTTPCode();
         http_response_code($operating_object->getSugesstedHTTPCode());
         echo json_encode($res);
         return;           
      }

   }
   else{
      // Access denied. 
      $res[ResponseFields::MESSAGE] = "Access denied to endpoint $object/$operation";
      $route_log->logError($res[ResponseFields::MESSAGE]);
      $res[ResponseFields::HTTP_CODE] = 403;
      http_response_code(403);
      echo json_encode($res);
      return;
   }
}
else{
   // Access denied. 
   $res[ResponseFields::MESSAGE] = "Access denied to endpoint $object";
   $route_log->logError($res[ResponseFields::MESSAGE] . ". Route Parts: " . vardumpIntoVar($route_parts) . " Permissions: " . vardumpIntoVar($permissions));
   $res[ResponseFields::HTTP_CODE] = 403;
   http_response_code(403);
   echo json_encode($res);
   return;
}

// Two possible answers here. We either need to return a file, in which case the response to this will be the file path, or we don't.
$file_path_to_return = $operating_object->getFileToReturn();

if ($file_path_to_return != ""){

   // Wiil assume that the file has an extension that will determine it's type. 
   $path_parts = pathinfo($file_path_to_return);
   $extension = $path_parts["extension"];
   $basename = $path_parts["basename"];

   header("Access-Control-Expose-Headers: Content-Disposition"); // Required so the Content Disposition field can be read in the client. 
   header("Content-Type: application/$extension");
   header("Content-Transfer-Encoding: Binary");
   header("Content-Length:".filesize($file_path_to_return));
   header("Content-Disposition: attachment; filename=$basename");
  
   // Cleaning what's going to be outputted. Otherwise the file might be corrupted. 
   ob_clean();
   flush();
   readfile($file_path_to_return);   
   exit();

}
else{
   // If the system got here all went well.
   $res[ResponseFields::DATA] = $ans;
   $res[ResponseFields::MESSAGE] = "OK";
   $res[ResponseFields::HTTP_CODE] = $operating_object->getSugesstedHTTPCode();
   http_response_code($operating_object->getSugesstedHTTPCode());
   echo json_encode($res);
   return;
}

// How to return a file 
// $route = $route_parser->getRouteParts();
// if (count($route) == 2){
//    if (($route[0] = "get") && ($route[1] == "pdf")){
//       $path_to_file = 'test.pdf';
//       header("Content-Type: application/pdf");
//       header("Content-Transfer-Encoding: Binary");
//       header("Content-Length:".filesize($path_to_file));
//       header("Content-Disposition: attachment; filename=apdf.pdf");
//       readfile($path_to_file);      
//       return;
//    }
// }


?> 
