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
DBCon::setPointerLocation("configs");  
$dbcon = new DBCon();
$con_main = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);
if ($con_main == NULL){
   echo "Error creating db connection: " . $dbcon->getError() . "\n";
   exit();
}


$tpp = new TableProcessingParameters($con_main);

// Gazepoint
$name = EyeTrackerCodes::GAZEPOINT;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 197;
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 150;
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 5;
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 8;
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                        = 20; 

// Reading parameters 
$fp[Study::READING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[Study::READING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 150;

// Binding parameters 
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 150;

// NBack RT
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 30;

// NBack MS 
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 30;

// Go No Go parameters 
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 100;


$ans = $tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

// HTC Vive Eye Pro. 
$name = EyeTrackerCodes::HTC_VIVE_EYE_PRO;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 105;
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 120;
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 6;
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 10;
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                        = 20; 

// Reading parameters 
$fp[Study::READING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[Study::READING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 120;

// Reading parameters 
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 120;

// NBack RT
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 25;

// NBack MS 
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 25;

// Go No Go parameters 
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 80;

$tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

// HP Omnicept
$name = EyeTrackerCodes::HP_OMNICEPT;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 105;
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 120;
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 6;
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 10;
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                        = 20; 

// Reading parameters 
$fp[Study::READING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[Study::READING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 120;

// Reading parameters 
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 3;
$fp[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 120;

// NBack RT
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKRT][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 25;

// NBack MS 
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::NBACKMS][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 25;

// Go No Go parameters 
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]    = 2;
$fp[Study::GONOGO][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL] = 80;

$tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}


echo "Finished\n";



?>