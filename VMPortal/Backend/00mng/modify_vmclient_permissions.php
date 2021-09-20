<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableSecrets.php");

// $class = "PortalUserOperations";
// var_dump($class::getConstList());
// exit();

include("cli_interface.php");

$section_name = "add_permissions";
$required = ["to", "object", "operation"];

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

$list_to_adapt = explode(" ",$params["to"]);

// Creating the connection to the secrets table.
$ts = new TableSecrets($con_secure);

// Expanding into a list unique ids.
$unique_ids = [];
foreach ($list_to_adapt as $uid){
   $temp = explode(".",$uid);
   if (count($temp) != 2){
      echo "Error invalid Institution Instance ID format: $uid\n";
      exit();
   }
   $institution = $temp[0];
   $instance    = $temp[1];

   if (!is_numeric($institution)){
      echo "Error invalid Institution Instance ID format: $uid\n";
      exit();
   }
   

   if ($instance == "x"){

      // We need to get all the instances enabled by       
      $ans = $ts->getEnabledInstancesForInstitution($institution);
      if ($ans === FALSE){
         echo "ERROR: " . $ts->getError() . "\n";
         exit();
      }

      foreach ($ans as $row){    
         $unique_ids[] = $institution . "."  . $row[TableSecrets::COL_INSTITUTION_INSTANCE];
      }

      continue;

   }
   else if (!is_numeric($instance)){
      echo "Error invalid Institution Instance ID format: $uid\n";
      exit();
   }

   // If it got here all checks out and we add it as is. 
   $unique_ids[] = $uid;

}

// We make sure the array's are truly unique 
//var_dump($unique_ids);
$unique_ids = array_unique($unique_ids);

// We get the permissions for each of them. 
$ans = $ts->getPermissionsFrom($unique_ids);
if ($ans === FALSE){
   echo "Error: Could not get permission list for each unique id: " . $ts->getError() . "\n";
   exit();
}

foreach ($ans as $row){
   $permissions = json_decode($row[TableSecrets::COL_PERMISSIONS],true);
   $uid = $row[TableSecrets::COL_UNIQUE_ID];
   $operation_array = array();
   if (array_key_exists($object,$permissions)){
      $operation_array = $permissions[$object];
   }
   $operation_array[] = $operation;
   $permissions[$object] = $operation_array;
   $permissions = json_encode($permissions);
   $ans = $ts->setPermissionOnUniqueID($permissions,$uid);
   if ($ans === FALSE){
      echo "Failed setting new permissions to $uid. Reason: " . $ts->getError() . "\n";
      exit();
   }
}

echo "Finished\n";


?>