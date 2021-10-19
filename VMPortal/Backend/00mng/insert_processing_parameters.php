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

// Quallity control Parameters that depend ONLY on the study
$study_qc = array();

$minimum_duration_reduction_constant = 0.9; // Used to provide some leeway in the minimum duration per trial. 

// Reading parameters 
$study_qc[Study::READING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]             = 150;  // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[Study::READING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 3; 
$study_qc[Study::READING][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 500*$minimum_duration_reduction_constant;  // ms.
$study_qc[Study::READING][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;   // % of trials.
$study_qc[Study::READING][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;   // % of trials. 

// Binding parameters 
$study_qc[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]             = 150;  // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 4; 
$study_qc[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 2750*$minimum_duration_reduction_constant;  // ms.
$study_qc[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;        // % of trials.
$study_qc[MultiPartStudyBaseName::BINDING][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;        // % of trials. 


// NBack RT
$study_qc[Study::NBACKRT][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]             = 30; // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[Study::NBACKRT][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 5; 
$study_qc[Study::NBACKRT][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 1000*$minimum_duration_reduction_constant;  // ms.
$study_qc[Study::NBACKRT][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;        // % of trials.
$study_qc[Study::NBACKRT][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;        // % of trials. 

// NBack VS - For now setting the same exact values as the NBack RT. 
$study_qc[Study::NBACKVS][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]             = 30; // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[Study::NBACKVS][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 5; 
$study_qc[Study::NBACKVS][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 1000*$minimum_duration_reduction_constant;  // ms.
$study_qc[Study::NBACKVS][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;        // % of trials.
$study_qc[Study::NBACKVS][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;        // % of trials. 

// NBack MS - For now setting the same exact values as the NBack RT. 
$study_qc[Study::NBACKMS][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]             = 30; // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[Study::NBACKMS][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 5; 
$study_qc[Study::NBACKMS][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 1000*$minimum_duration_reduction_constant;  // ms.
$study_qc[Study::NBACKMS][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;        // % of trials.
$study_qc[Study::NBACKMS][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;        // % of trials. 


// Go No Go parameters 
$study_qc[Study::GONOGO][QualityControlParamterStudy::MIN_POINTS_PER_TRIAL]              = 100; // DEPRACATED. Will not be in used after version 17.3.0
$study_qc[Study::GONOGO][QualityControlParamterStudy::MIN_FIX_PER_TRIAL]                = 2; 
$study_qc[Study::GONOGO][QualityControlParamterStudy::MIN_TIME_DURATION_PER_TRIAL]      = 500*$minimum_duration_reduction_constant;  // ms.
$study_qc[Study::GONOGO][QualityControlParamterStudy::THRESHOLD_VALID_NUM_DATA_POINTS]  = 85;        // % of trials.
$study_qc[Study::GONOGO][QualityControlParamterStudy::THRESHOLD_VALID_NUM_FIXATIONS]    = 80;        // % of trials. 


////////////////////////////////////// Gazepoint
$name = EyeTrackerCodes::GAZEPOINT;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 15;  // This is a % of the maximum value of the used resolution. 
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;  // ms. // DEPRACATED. It's now computed based on the sampling frequency. 
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 150; // Hz.   
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;  // Pixels. Absolute. 

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE]  = 5;   // USED FOR COMPUTATION BUT RESULTS NOT USED    
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE]  = 8;   // USED FOR COMPUTATION BUT RESULTS NOT USED 
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                         = 20;  // USED FOR COMPUTATION BUT RESULTS NOT USED

$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_MINUS] = 20;   // %.
$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_PLUS]  = 2;    // %.
$fp[QualityControlParamterGlobal::THRESHOLD_NUM_TRIALS_VALID_F]          = 90;   // %.
$fp = array_merge($fp,$study_qc);

$ans = $tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

////////////////////////////////////// HTC Vive Eye Pro. 
$name = EyeTrackerCodes::HTC_VIVE_EYE_PRO;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 3;      // This is a % of the maximum value of the used resolution. 
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;     // ms. 
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 120;    // Hz.     
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;     // Pixels. Absolute.  

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 6;  // USED FOR COMPUTATION BUT RESULTS NOT USED    
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 10; // USED FOR COMPUTATION BUT RESULTS NOT USED 
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                        = 20; // USED FOR COMPUTATION BUT RESULTS NOT USED 

$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_MINUS] = 20;   // %.
$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_PLUS]  = 2;    // %.
$fp[QualityControlParamterGlobal::THRESHOLD_NUM_TRIALS_VALID_F]          = 90;   // %.
$fp = array_merge($fp,$study_qc);

$ans = $tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

////////////////////////////////////// HP Omnicept
$name = EyeTrackerCodes::HP_OMNICEPT;
$pp[ProcessingParameter::MAX_DISPERSION_WINDOW]   = 3;    // This is a % of the maximum value of the used resolution. 
$pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH] = 50;   // ms. 
$pp[ProcessingParameter::SAMPLE_FREQUENCY]        = 120;  // Hz.     
$pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS]  = 80;   // Pixels. Absolute.  

$fp = array();
$fp[QualityControlParamterGlobal::MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 6;   // USED FOR COMPUTATION BUT RESULTS NOT USED    
$fp[QualityControlParamterGlobal::MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE] = 10;  // USED FOR COMPUTATION BUT RESULTS NOT USED 
$fp[QualityControlParamterGlobal::MAX_GLITCHES]                        = 20;  // USED FOR COMPUTATION BUT RESULTS NOT USED

$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_MINUS] = 15;   // %.
$fp[QualityControlParamterGlobal::ALLOWED_SAMPLING_FREQ_VARIATION_PLUS]  = 2;    // %.
$fp[QualityControlParamterGlobal::THRESHOLD_NUM_TRIALS_VALID_F]          = 90;   // %.
$fp = array_merge($fp,$study_qc);

$ans = $tpp->addProductParameters($name,$pp,$fp);
if ($ans === FALSE){
   echo "Failed on inserting $name: " . $tpp->getError() . "\n";
   return;
}

echo "Finished\n";



?>
