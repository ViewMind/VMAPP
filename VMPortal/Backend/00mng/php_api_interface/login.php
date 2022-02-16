<?php

include_once ("api_comm.php");
include_once ("load_inputs.php");

//$credential_file = "./local_credentials.cnf";
//$credential_file = "production_credentials.cnf";
$credential_file = "devapi_credentials.cnf";
$inputs_file = "./input.cnf";


if (str_contains($inputs_file,"local")){
   echo "Using LOCAL Credentials\n";
}

$credentials = json_decode(file_get_contents($credential_file),true);

$user   = $credentials["user"];
$passwd = $credentials["password"];
$api    = $credentials["url"];

// Login
$api = new APIComm($api);
if (!$api->login($user,$passwd,LoginMasks::ADMIN_SITE)){
   echo "Error loggin in. Reason: " . $api->getError() . "\n";
   echo "LAST CMD:\n";
   echo $api->getLastCurlCommand();
   echo "\n";
   exit();
}
else {
   $api->printWelcome();
}


?>