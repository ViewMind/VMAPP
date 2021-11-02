<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TablePortalUsers.php");


/////////////////////////////////////////////////////// INPUTS //////////////////////////////////////////////////

$emails = ["ariel.arelovich@viewmind.ai","gerardo.fernandez@viewmind.ai"];

//$action = "standard";
$action = "admin";
//$action = "custom";
//$action = "add_medrec";

$permissions = array();

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