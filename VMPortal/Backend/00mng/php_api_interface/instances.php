<?php

include_once("login.php");

class  Actions {
   const LIST            = "list_instances";
   const CREATE          = "new_instance";
}

/***
 * WARNING: When creating delete the version field from inputs in order to use the latest version. 
 */

$action = Actions::LIST;
//$action = Actions::CREATE;

$body = loadInputs($action);

if ($action == Actions::LIST){
   $req_url = "instances/list/0";
   $ret = $api->APICall($req_url);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      $data = $ret["data"];
      foreach ($data as $instid => $struct){
         echo $struct["name"] . "\n";
         foreach ($struct["versions"] as $instance => $version){
            echo "   $instance -> $version\n";
         }
      }
      echo "\n";
   }
}
else if ($action === Actions::CREATE){
   $id = $body["institution"];
   unset($body["institution"]);
   $req_url = "instances/create/$id?";
   $url_params = array();
   foreach ($body as $name => $value){
      $url_params[] = "$name=$value";
   }
   $req_url = $req_url . implode("&",$url_params);

   echo "$req_url\n";
   //exit();

   $ret = $api->APICall($req_url);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      $data = $ret["data"];
      echo "Configuration file contents\n";
      echo "======\n";
      echo $data["vmconfiguration"] . "\n";
      unset($data["vmconfiguration"]);
      echo "======\n";
      echo json_encode($data,JSON_PRETTY_PRINT);
      echo "\n";
   }
}


?>