<?php

include_once("login.php");

class  Actions {
   const MOD_PERMISSIONS = 1;
   const CREATE          = 2;
}


//$action = Actions::MOD_PERMISSIONS;
$action = Actions::CREATE;

////////////////////////////// Modifying permissions
$to_add[APIEndpoints::INSTITUTION] = ["operate"];
$to_add[APIEndpoints::INSTANCES] = ["create"];
$to_add[APIEndpoints::PORTAL_USERS] = ["operate"];
$to_remove = array();

////////////////////////////// Creating a new user
$body["name"] = "Juan";
$body["lastname"] = "Poroto";
$body["company"] = "";
$body["email"] = "somone@example.com";
$body["passwd"] = "Valk!1234";
$body["permissions"] = "portal-admin"; // could be 'portal_admin' or 'portal' or 'clean'
$body["user_role"] = "1";
$body["institution_id"] = 1;
$body["force_enable"] = true;

// After the login comes the actual request
if ($action === Actions::MOD_PERMISSIONS){

   // First we get the permissions·
   $data["id"] = $user;
   $data["action"] = EndpointBodyActions::GET;

   $req_url = "portal_users/modify_permissions/0";

   $ret = $api->APICall($req_url,$data);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      //var_dump($ret);
      $permissions = $ret["data"]["permissions"];

      foreach ($to_add as $object => $list_to_add){
         $list = array();
         if (array_key_exists($object,$permissions)){
            $list = $permissions[$object];
         }
         foreach ($list_to_add as $operation){
            $list[] = $operation;
         }
         $list = array_unique($list);
         $permissions[$object] = $list;
      }

      //var_dump($permissions);

      // Setting the permissions
      $data["action"] = EndpointBodyActions::SET;
      $data["role"] = $ret["data"]["role"];
      $data["permissions"] = $permissions;

      $ret = $api->APICall($req_url,$data);
      if ($ret === false){
         finishAndDie($api);
      }
      else {
         echo "Permissions for user $user, successfully modified\n";
      }   

   }

}
if ($action === Actions::CREATE){
   $req_url = "portal_users/operate/0";
   $body["action"] = EndpointBodyActions::CREATE;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Created new user with email " . $body["email"] .  "\n";
      //finishAndDie($api);
   }   

}




?>