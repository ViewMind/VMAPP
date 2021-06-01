<?php

//////////////////////////////////// HEADERS ////////////////////////////////
// header("Access-Control-Allow-Origin: *");  // Anyone can access
header("Content-Type: application/json; charset=UTF-8"); // Will return json data. 
// header("Access-Control-Allow-Methods: POST");
// header("Access-Control-Max-Age: 3600");
// header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authentication, AuthType");

//////////////////////////////////// INCLUDES ////////////////////////////////
include("./common/config.php");
if (empty(CONFIG)) return; // This means that there was an error loading the configuration file. 

include_once ("common/echo_out.php");
include_once ("api_management/RouteParser.php");
include_once ("api_management/AuthManager.php");
include_once ("db_management/TableSecrets.php");
include_once ("api_management/ObjectPortalUsers.php");

//////////////////////////////////// TOKENIZING THE ROUTE ////////////////////////////////
$route_parser = new RouteParser();

if (!$route_parser->tokenizeURL($_SERVER['REQUEST_URI'],2)){
   $error = $route_parser->getError();
   $code = 400; // This was a client error 
   $res[ResponseFields::MESSAGE] = $error;
   $res[ResponseFields::HTTP_CODE] = $code;
   http_response_code($code);
   echo json_encode($res);
   return;
}

//////////////////////////////////// AUTENTICATION OF REQUEST ////////////////////////////////
// Getting all headers. 
$headers = getallheaders();

// Generating the message in case it needs to be signed. 
$raw_data = file_get_contents("php://input");
$message = $raw_data . $route_parser->getEndpointAndParameters();

$auth_mng = new AuthManager($headers,$_POST,$_FILES);
if (!$auth_mng->authenticate($message)){
   $res[ResponseFields::MESSAGE] = $auth_mng->getError();
   $res[ResponseFields::HTTP_CODE] = $auth_mng->getSuggestedHTTPCode();
   http_response_code($auth_mng->getSuggestedHTTPCode());
   echo json_encode($res);
   return;
}


//////////////////////////////////// ROUTING ////////////////////////////////
$permissions = $auth_mng->getPermissions();
$dbuser      = $auth_mng->getServiceDBUser();
$route_parts = $route_parser->getRouteParts();

if (count($route_parts) != 3){
   // Badly formed endpoint. 
   $res[ResponseFields::MESSAGE] = "Badly formed endpoint " . $route_parser->getEndpointAndParameters();
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
         $res[ResponseFields::MESSAGE] = "Endpoint $object is not yet implemented";
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;         
      }

      // Creating an object of the proper class.
      $class = ROUTING[$object];
      $operating_object = new $class($dbuser);

      if (!method_exists($operating_object,$operation)){
         // Method not implemented yet. 
         $res[ResponseFields::MESSAGE] = "Endpoint $object/$operation not implemented yet";
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;   
      }

      $ans = $operating_object->$operation($identifier,$route_parser->getParameters());
      
      if ($ans === false){
         // Something went wrong. 
         $res[ResponseFields::MESSAGE] = $operating_object->getError();
         //$res[ResponseFields::MESSAGE] = "WTF";
         $res[ResponseFields::HTTP_CODE] = 500;
         http_response_code(500);
         echo json_encode($res);
         return;           
      }

   }
   else{
      // Access denied. 
      $res[ResponseFields::MESSAGE] = "Access denied to endpoint $object/$operation";
      $res[ResponseFields::HTTP_CODE] = 403;
      http_response_code(403);
      echo json_encode($res);
      return;
   }
}
else{
   // Access denied. 
   $res[ResponseFields::MESSAGE] = "Access denied to endpoint $object";
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

// echoOut("Files",true);
// echoOut($_FILES,true);
// // Post Checking
// echoOut("Post Checking",true);
// echoOut($_POST,true);

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
