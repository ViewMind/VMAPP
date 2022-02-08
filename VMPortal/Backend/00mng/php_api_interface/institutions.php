<?php

include_once("login.php");

class  Actions {
   const LIST            = "list_institutions";
   const CREATE          = "create_institution";
   const MODIFY          = "modify_institution";
}

//$action = Actions::LIST;
$action = Actions::CREATE;

$body = loadInputs($action);

if ($action === Actions::LIST){

   $req_url = "institution/operate/0";
   $body["action"] = EndpointBodyActions::LIST;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Listing institutions and their ids\n";
      echo json_encode($ret["data"],JSON_PRETTY_PRINT) . "\n";
   }

}
else if ($action === Actions::CREATE){

   $req_url = "institution/operate/0";
   $body["action"] = EndpointBodyActions::CREATE;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Created institution with name " . $body["institution_name"] . "\n";
   }

}
else if ($action === Actions::MODIFY){

   $req_url = "institution/operate/0";
   $body["action"] = EndpointBodyActions::SET;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Modified institution with name " . $body["institution_name"] . "\n";
   }

}


?>