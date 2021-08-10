<?php

/**
 * Inserts a version for the specific institution and instance. 
 */

include("../db_management/TableUpdates.php");
include("cli_interface.php");

$section_name = "entry_update";
$required = [TableUpdates::COL_INSTITUTION_ID,
TableUpdates::COL_INSTITUTION_INSTANCE,
TableUpdates::COL_VERSION_STRING,
TableUpdates::COL_EYETRACKER_KEY
];

DBCon::setPointerLocation("configs");  
$params = readAndVerifyData($required,$section_name);

$dbcon = new DBCon();
$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$tup = new TableUpdates($con_main);

$institution_id       = $params[TableUpdates::COL_INSTITUTION_ID];
$institution_instance = $params[TableUpdates::COL_INSTITUTION_INSTANCE];
$version              = $params[TableUpdates::COL_VERSION_STRING];
$et_key               = $params[TableUpdates::COL_EYETRACKER_KEY];

$ans = $tup->addInstance($institution_id,$institution_instance,$version,$et_key);
if ($ans === false){
   echo "Failed on adding the new instance: "  . $tup->getError() . "\n";
   exit();
}

echo "Finsihed\n";

?>