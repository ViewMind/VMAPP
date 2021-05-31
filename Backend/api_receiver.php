<?php

include ("RouteParser.php");
include ("echoOut.php");

const DBUG_OUT = true;
$key = "here is a really cool key";

header("Access-Control-Allow-Origin: *");  // Anyone can access
header("Content-Type: application/json; charset=UTF-8"); // Will return json data. 

$route_parser = new RouteParser();

if (!$route_parser->tokenizeURL($_SERVER['REQUEST_URI'],2)){
   $to_print["error"]   = $route_parser->getError();
   echoOut($to_print,DBUG_OUT);
   return;
}

$route = $route_parser->getRouteParts();
if (count($route) == 2){
   if (($route[0] = "get") && ($route[1] == "pdf")){
      $path_to_file = 'test.pdf';
      header("Content-Type: application/pdf");
      header("Content-Transfer-Encoding: Binary");
      header("Content-Length:".filesize($path_to_file));
      header("Content-Disposition: attachment; filename=apdf.pdf");
      readfile($path_to_file);      
      return;
   }
}

// Checking the headers.
echoOut("Headers",DBUG_OUT);
$headers = getallheaders();
$sent_auth_string = $headers["Authentication"];
echoOut($headers,DBUG_OUT);

$to_print["route"] = $route_parser->getRouteParts(); // In this particular api routing the first two values are useless
$to_print["parameters"] = $route_parser->getParameters();
echoOut($to_print,DBUG_OUT);

// Now checking for files
echoOut("File checking",DBUG_OUT);
echoOut($_FILES,DBUG_OUT);

// And finally checking for data. 
echoOut("Data checking",DBUG_OUT);
$raw_data = file_get_contents("php://input");
echoOut($raw_data,DBUG_OUT);

// Creating the authentication string. 
$message = $raw_data . $route_parser->getEndpointAndParameters();
$auth_string = hash_hmac('sha3-512', $message, $key);

// Compare auth string. 
$comparison_result = ($auth_string == $sent_auth_string);
$hash_comparison_result = hash_equals($auth_string,$sent_auth_string);
echoOut("Auth Comparison Results:",DBUG_OUT);
$auth["Computed Hash"] = $auth_string;
$auth["Endpoint And Parameters"] = $route_parser->getEndpointAndParameters();
$auth["Normal Comparison"] = $comparison_result;
$auth["Hash Normal Comparison"] = $hash_comparison_result;
echoOut($auth,DBUG_OUT);

echoOut("Post Checking",DBUG_OUT);
echoOut($_POST,DBUG_OUT);
return;


// // All is good. 
// http_response_code(200);



?> 
