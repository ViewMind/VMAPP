<?php

include_once ("../db_management/DBCon.php");
include_once ("../data_processing/DataContainerNames.php");
include_once ("../db_management/TableInstitution.php");
include_once ("../db_management/TableInstitutionUsers.php");
include_once ("../db_management/TablePlacedProducts.php");
include_once ("../db_management/TablePortalUsers.php");
include_once ("../db_management/TableSecrets.php");
include_once ("../db_management/TableProcessingParameters.php");

//include("cli_interface.php");

$dbcon = new DBCon();
$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}

$tpp = new TableProcessingParameters($con_main);

// Gazepoint
$name = "gazepoint";
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 197;
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 150;
$fp = array();

$ans = $tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

// HTC Vive Eye Pro. 
$name = "htcviveeyepro";
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 105;
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 120;
$fp = array();

$tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

echo "Finished\n";



?>