<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableAppPasswordRecovery.php");

include("cli_interface.php");

$section_name = "set_recovery_password";
$required = [TableAppPasswordRecovery::COL_INSTITUTION_ID,TableAppPasswordRecovery::COL_INSTITUTION_INSTANCE];

DBCon::setPointerLocation("configs");  
$params = readAndVerifyData($required,$section_name);

$dbcon = new DBCon();
$con_secure = $dbcon->dbOpenConnection(DBCon::DB_SECURE,DBCon::DB_SERVICE_ADMIN);
if ($con_secure == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$tapr = new TableAppPasswordRecovery($con_secure);
$ans = $tapr->createPassword($params[TableAppPasswordRecovery::COL_INSTITUTION_ID],$params[TableAppPasswordRecovery::COL_INSTITUTION_INSTANCE]);
echo "$ans\n";

?>
