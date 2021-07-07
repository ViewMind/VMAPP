<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableSecrets.php");


include("cli_interface.php");

$section_name = "create_secret";
$required = [TableSecrets::COL_INSTITUTION_ID, 
TableSecrets::COL_INSTITUTION_INSTANCE
];

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
$table_secrets = new TableSecrets($con_secure);
$table_inst    = new TableInstitution($con_main);

$exists = $table_inst->exists($params[TableSecrets::COL_INSTITUTION_ID]);
if ($exists === false){
   echo "Could not verify institution existance: " . $table_inst->getError();
   exit();
}
if ($exists[0] == 0){
   echo "Insitution " . $params[TableSecrets::COL_INSTITUTION_ID] . " does not exist\n";
   exit();
}

$secret = $table_secrets->createNewSecret($params[TableSecrets::COL_INSTITUTION_ID],$params[TableSecrets::COL_INSTITUTION_INSTANCE]);
if ($secret === false){
   echo "Could not generate secret: " . $table_secrets->getError();
   exit();
}
else{
   echo "Generated Secret:\n";
   var_dump($secret);
}





?>