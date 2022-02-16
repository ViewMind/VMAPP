<?php

include_once("login.php");
include_once("../../db_management/TablePortalUsers.php");

class  Actions {
   const MOD_PERMISSIONS = "modify_user_permissions";
   const CREATE          = "create_new_user";
   const MODIFY          = "modify_user";
   const LINK            = "link_user_to_institution";
   const LIST            = "list_portal_users";
   const GET_PERMISSIONS = "permissions";
}

//$action = Actions::CREATE;
$action = Actions::MOD_PERMISSIONS;
//$action = Actions::MODIFY;
//$action = Actions::LINK;
//$action = Actions::LIST;
//$action = Actions::GET_PERMISSIONS;

$body = loadInputs($action);

////////////////////////////// Modifying permissions is done from here. 
$to_add[APIEndpoints::INSTITUTION] = ["operate"];
// $to_add[APIEndpoints::INSTANCES] = ["create","list","update"];
// $to_add[APIEndpoints::PORTAL_USERS] = ["operate"];
$to_remove = array();

////////////////////////////// Creating a new user

// After the login comes the actual request
if (($action === Actions::MOD_PERMISSIONS) || ($action === Actions::GET_PERMISSIONS)){

   // First we get the permissions·
   $body["action"] = EndpointBodyActions::GET;

   $req_url = "portal_users/modify_permissions/0";

   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      //var_dump($ret);
      $permissions = $ret["data"]["permissions"];

      if ($action === Actions::GET_PERMISSIONS){
         echo "Permsions for user " . $body["id"] . "\n";
         echo json_encode($permissions,JSON_PRETTY_PRINT) . "\n";
         exit();
      }

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

      // Setting the permissions
      $body["action"] = EndpointBodyActions::SET;
      $body["role"] = $ret["data"]["role"];
      $body["permissions"] = $permissions;

      echo json_encode($body,JSON_PRETTY_PRINT) . "\n";
      exit();

      $ret = $api->APICall($req_url,$body);
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
else if ($action === Actions::MODIFY){
   $req_url = "portal_users/operate/0";
   $body["action"] = EndpointBodyActions::SET;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Modified user with id '" . $body["id"] .  "'\n";
   }   
}
else if ($action === Actions::LINK){
   $req_url = "portal_users/operate/0";
   $body["action"] = EndpointBodyActions::LINK;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      echo "Linked user '" . $body["portal_user"] .  "' with institution '" . $body["institution_id"] . "'\n";
   }   
}
else if ($action === Actions::LIST){
   $req_url = "portal_users/operate/0";
   $body["action"] = EndpointBodyActions::LIST;
   $ret = $api->APICall($req_url,$body);
   if ($ret === false){
      finishAndDie($api);
   }
   else {
      $data = $ret["data"];
      $keyid_norm = 9;
      $name_norm  = 50;
      foreach ($data as $row){
         $name = $row[TablePortalUsers::COL_NAME];
         $lastname = $row[TablePortalUsers::COL_LASTNAME];
         $email = $row[TablePortalUsers::COL_EMAIL];
         $id = $row[TablePortalUsers::COL_KEYID];

         while (strlen($id) < $keyid_norm) $id = "$id ";
         $dispname = "$lastname, $name";
         while (strlen($dispname) < $name_norm) $dispname = "$dispname ";

         echo "$id $dispname $email\n";
      }
   }   
}




?>