<?php

include_once ("../db_management/DBCon.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableInstitutionUsers.php");
include_once ("../db_management/TablePlacedProducts.php");
include_once ("../db_management/TablePortalUsers.php");
include_once ("../db_management/TableSecrets.php");

///////////////////////// INPUT PARAMETERS //////////////////////////////
$params[TableInstitution::COL_EMAIL] = "ariel.arelovich@viewmind.ai";
$params[TableInstitution::COL_INSTITUTION_NAME] = "Test Institution";
// Can add more below.
////////////////////////////////////////////////////////////////////////

$dbcon = new DBCon();
if ($dbcon->getError() != ""){
   echo "Error creating db connection: " . $dbcon->getError();
   exit();
}

$con = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

echo "Creating the institution\n";

$table_inst = new TableInstitution($con);
$ans = $table_inst->createInstitution($params);

if ($ans === false){
   echo "Error creating institution: " . $table_inst->getError() . "\n";
}

echo "Created institution with id: " . $table_inst->getLastInserted()[0] . "\n";


?>