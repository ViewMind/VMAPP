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

//////////////////////////////////// LOG SETUP ////////////////////////////////
$auth_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::AUTH_LOG_LOCATION]);
$auth_log->setSource($_SERVER['REMOTE_ADDR']);

$route_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::ROUTING_LOG_LOCATION]);
$route_log->setSource($_SERVER['REMOTE_ADDR']);

$base_log = new LogManager(CONFIG[GlobalConfigLogs::GROUP_NAME][GlobalConfigLogs::BASE_STD_LOG]);
$base_log->setSource($_SERVER['REMOTE_ADDR']);

//////////////////////////////////// TOKENIZING THE ROUTE ////////////////////////////////
$route_parser = new RouteParser();

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
$message = $raw_data . $route_parser->getEndpointAndParameters();

$auth_mng = new AuthManager($headers,$_POST,$_FILES);
if (!$auth_mng->authenticate($message)){
   $res[ResponseFields::MESSAGE] = $auth_mng->getReturnableError();
   $res[ResponseFields::HTTP_CODE] = $auth_mng->getSuggestedHTTPCode();
   $auth_log->logError($auth_mng->getError());
   http_response_code($auth_mng->getSuggestedHTTPCode());
   echo json_encode($res);
   return;
}

if (!$auth_mng->shouldDoOperation()){
   // The endopoint should be ignored. For now the only time this happens is when a login operation is used to generate the authentication token.
   $res[ResponseFields::DATA]["token"] = $auth_mng->getAuthToken();
   $res[ResponseFields::DATA]["id"]    = $auth_mng->getUserID();
   $res[ResponseFields::MESSAGE] = "";
   $res[ResponseFields::HTTP_CODE] = 200;
   http_response_code(200);
   echo json_encode($res);
   return;
}


//////////////////////////////////// ROUTING ////////////////////////////////
$permissions = $auth_mng->getPermissions();
$dbuser      = $auth_mng->getServiceDBUser();

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

// Routes will have three parts OBJECT/OPERATION/IDENTFIER. So first is object. 
if (array_key_exists($object,$permissions)){
   
   // It means that there are permissions for this object. Next is the operator.

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
      $operating_object = new $class($dbuser,$headers);

      if (!method_exists($operating_object,$operation)){
         // Method not implemented yet. 
         $res[ResponseFields::MESSAGE] = "Endpoint $object/$operation does not exist";
         $route_log->logError($res[ResponseFields::MESSAGE]);
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;   
      }

      // Calling the object and the operation with the data. 
      // $operating_object->setFileData($_FILES);
      $ans = $operating_object->$operation($identifier,$route_parser->getParameters());
      
      if ($ans === false){
         // Something went wrong. 
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

// If the system got here all went well. 
$res[ResponseFields::DATA] = $ans;
$res[ResponseFields::MESSAGE] = "OK";
$res[ResponseFields::HTTP_CODE] = 200;
http_response_code(200);
echo json_encode($res);
return;

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
