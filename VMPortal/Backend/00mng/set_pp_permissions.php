<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TablePortalUsers.php");


/////////////////////////////////////////////////////// INPUTS //////////////////////////////////////////////////

$emails = ["ariel.arelovich@viewmind.ai"];

//$action = "standard";
$action = "admin";
//$action = "custom";
//$action = "add_medrec";
$action = "add_control_admin";

$permissions = array();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function mergePermissionArrays($permissions, $to_add){
   foreach ($to_add as $object => $operations){
      if (!array_key_exists($object,$permissions)){
         $permissions[$object] = array();
      }

      foreach ($operations as $operation){
         $permissions[$object][] = $operation;
      }

      $permissions[$object] = array_unique($permissions[$object]);

   }
   return $permissions;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DBCon::setPointerLocation("configs");  
$dbcon = new DBCon();
$con = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
if ($con == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$table = new TablePortalUsers($con);

if ($action == "standard"){
   echo "Seting Standard Permissions\n";
   $ans = $table->setStandardUserPermissionsToUserList($emails);
}
else if ($action == "admin"){
   echo "Seting Admin Permissions\n";
   $ans = $table->setAdminPortalPermissionsToUserList($emails);
}
else if ($action == "add_medrec"){
   echo "Adding med rec to each user's permissions\n";
   $ans = $table->getPermissionsForEmailList($emails);
   if ($ans === false){
      echo "Failed getting permissions list: " . $table->getError() . "\n";
      exit();
   }
   foreach ($ans as $row){
      $permissions = json_decode($row[TablePortalUsers::COL_PERMISSIONS],true);
      $permissions = array_merge(TablePortalUsers::getMedRecPermissions(),$permissions);
      $temp = array();
      $temp[] = $row[TablePortalUsers::COL_EMAIL];
      $ans = $table->setPermissionsToUserList($temp,$permissions);
      if ($ans === false){
         echo "Failed setting permissions list to user " . $temp[0] . ". Reason:  " . $table->getError() . "\n";
         exit();
      }   
   }
}
else if ($action == "add_control_admin"){
   echo "Adding master admin to each user's permissions\n";
   $ans = $table->getPermissionsForEmailList($emails);
   if ($ans === false){
      echo "Failed getting permissions list: " . $table->getError() . "\n";
      exit();
   }
   foreach ($ans as $row){
      $permissions = json_decode($row[TablePortalUsers::COL_PERMISSIONS],true);
      $to_add = TablePortalUsers::getMasterAdminPermissions();
      $permissions = mergePermissionArrays($permissions,$to_add);

      $temp[] = $row[TablePortalUsers::COL_EMAIL];
      $ans = $table->setPermissionsToUserList($temp,$permissions);
      if ($ans === false){
         echo "Failed setting permissions list to user " . $temp[0] . ". Reason:  " . $table->getError() . "\n";
         exit();
      }   
   }   
}
else {
   echo "Setting Custom Permissions\n";
   $ans = $table->setPermissionsToUserList($emails,$permissions);
}

if ($ans === false){
   echo "Error doing the update: " . $table->getError() . "\n";
}
else {
   echo "Update finished to emails\n   ";
   echo implode("\n   ",$emails) . "\n";
}



?>