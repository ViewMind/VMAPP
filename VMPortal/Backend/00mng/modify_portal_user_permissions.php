<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TablePortalUsers.php");

// $class = "PortalUserOperations";
// var_dump($class::getConstList());
// exit();

include("cli_interface.php");

$section_name = "add_permissions_to_pp";
$required = ["email", "object", "operation"];

DBCon::setPointerLocation("configs");  
$params = readAndVerifyData($required,$section_name);

/////////////////////////////////////////////

$dbcon = new DBCon();
$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}
$con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
if ($con_secure == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

/////////////////////////////////////////////
// Mapping endopoints to their possible operations.

$OperationsMap = [
   APIEndpoints::INSTITUTION => InstitutionOperations::getConstList(),
   APIEndpoints::PORTAL_USERS => PortalUserOperations::getConstList(),
   APIEndpoints::REPORTS => ReportOperations::getConstList(),
   APIEndpoints::SUBJECTS => SubjectOperations::getConstList()
];

// Verifying the object is with the operation. 
$object = $params["object"];
$operation = $params["operation"];

if (!array_key_exists($object,$OperationsMap)){
   echo "Invalid object $object\n";
   exit();
}

if (!in_array($operation,$OperationsMap[$object])){
   echo "Invalid operation $operation for object $object\n";
   exit();
}

$keyid = $params["email"];

$tpu = new TablePortalUsers($con_secure);

$ans = $tpu->getUsersPermission($keyid);
if ($ans === false){
   echo "Failed getting $keyid permissions. Reason: " . $tpu->getError();
   return;
}

$permissions = $ans[0][TablePortalUsers::COL_PERMISSIONS];
if ($permissions == NULL) $permissions = array();
else $permissions = json_decode($permissions,true);

if (json_last_error() != JSON_ERROR_NONE){
   echo "Failed decoding permssions string\n";
   return;
}

if (!array_key_exists($object,$permissions)){
   $permissions[$object] = array();
}

$permissions[$object][] = $operation;

$ans = $tpu->setUsersPermission($keyid,json_encode($permissions));
if ($ans === FALSE){
   echo "Failed setting $keyid permissions. Reason: " . $tpu->getError();
   return;
}

echo "Finished\n";


?>