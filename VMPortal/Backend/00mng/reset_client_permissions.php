<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableSecrets.php");


include("cli_interface.php");

$section_name = "reset_client_permission";
$required = ["doit"];

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

$ts = new TableSecrets($con_secure);
$ans = $ts->resetBasicClientPermissions();
if ($ans === false){
   echo "Failed resetting basic client permissions. Reason: " + $ts->getError();
   exit();
}

echo "Finished\n";


?>