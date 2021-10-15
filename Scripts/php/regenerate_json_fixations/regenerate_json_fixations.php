<?php

/**
 * QUICK AND DIRTY.
 * This scripts overwrites the fixations computed for each dataset of an input VM JSON by re computing them with a different md value. 
 * WARNING: This will invalidate some of the QC parameters. 
 */

include_once(__DIR__."/../fixation_analysis/online_fixations.php");
include_once("frequency_interpolator.php");
include_once("qc_computations.php");

///////////////////////////// INPUTS /////////////////////////////////////////
//$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/fixation_analysis/gonogo/htc_files/gonogo_2021_09_07_17_45.json";
//$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_mendoza_nbackrt/nbackrt_2021_10_04_10_20.json";
//$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29.json";
$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_nback_negative/nbackrt_2021_10_14_15_15.json";
$target_md = 3;
$do_frequency_interpolation = false;

/////////////////////////////////////////////////////////////////////////////

function computeFixationsForDataSet($maximum_dispersion,$raw_data,$f,$min_fix_size){

   // Everytime fixations are computed the previous values are resete. 
   $fixations = array();
   $fixations["L"] = array();
   $fixations["R"] = array();      
  
   $onlineFixR = new OnlineMovingWindowAlgorithm($maximum_dispersion,$f,$min_fix_size);
   $onlineFixL = new OnlineMovingWindowAlgorithm($maximum_dispersion,$f,$min_fix_size);
 
   foreach ($raw_data as $raw_data_point) {
      $time = $raw_data_point["ts"];
      $xr   = $raw_data_point["xr"];
      $yr   = $raw_data_point["yr"];
      $yl   = $raw_data_point["yl"];
      $xl   = $raw_data_point["xl"];
      $fix = $onlineFixR->computeFixationsOnline($xr, $yr, $time);
      if ($fix->valid) {         
         $fixations["R"][] = $fix->toStdJSON();
         //[$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];         
      }
      $fix = $onlineFixL->computeFixationsOnline($xl, $yl, $time);
      if ($fix->valid) {
         $fixations["L"][] = $fix->toStdJSON();
         //[$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];
      }
   }

   $fix = $onlineFixR->finalizeComputation();         
   if ($fix->valid) $fixations["R"][] = $fix->toStdJSON();
   $fix = $onlineFixL->finalizeComputation();         
   if ($fix->valid) $fixations["L"][] = $fix->toStdJSON();


   return $fixations;

}


/////////////////////////////////////////////////////////////////////////////

// Computing the output file name.
if (!is_file($input_file)){
   echo "Input file $input_file does not exist\n";
   exit();
}

$pathinfo = pathinfo($input_file);
$fname = $pathinfo["filename"];
$path  = $pathinfo["dirname"];
$output_file = "$path/$fname" . "_reprocessed.json";

echo "INPUT: $input_file\n";

// Checking if frequency interpolation was set.
if ($do_frequency_interpolation){
   echo "FREQUENCY INTERPOLATOR IS ENABLED\n";
   $fi = new FrequencyInterpolator($input_file);
   $input_file = $fi->interpolate();
   echo "   NEW INPUT FILE $input_file\n";
}

echo "OUTPUT FOR REGENERATING FIXATIONS: $output_file\n";

// Loading the data. 
$vmdata = json_decode(file_get_contents($input_file),true);

// Getting the resolution
$w = $vmdata["processing_parameters"]["resolution_width"];
$h = $vmdata["processing_parameters"]["resolution_height"];

// Getting the sampling frequency and minimum fixation duration.
$f   = $vmdata["processing_parameters"]["sample_frequency"];
$mfd = $vmdata["processing_parameters"]["minimum_fixation_length"];


// Computing and storing new fixations parameters. 
$md_px = round(max($w,$h)*$target_md/100);
echo "MD IN PX: $md_px\n";
$vmdata["processing_parameters"]["max_dispersion_window"] = $target_md;
$vmdata["processing_parameters"]["max_disp_window_in_px"] = $md_px;

// Computing the actual new fixations. 
echo "Re computing fixations .... \n";
foreach ($vmdata["studies"] as $study_name => $study_data){
   
   $new_trial_list = array();
   
   foreach ($study_data["trial_list"] as $trial){
   
      foreach ($trial["data"] as $dataset_name => $dataset){
         $fixations = computeFixationsForDataSet($md_px,$dataset["raw_data"],$f,$mfd);
         $trial["data"][$dataset_name]["fixations_l"] = $fixations["L"];
         $trial["data"][$dataset_name]["fixations_r"] = $fixations["R"];         
      }
      $new_trial_list[] = $trial;
      
   }
   $study_data["trial_list"] = $new_trial_list;
   $vmdata["studies"][$study_name] = $study_data;
}

// Finally storing the reprocessed file. 
$fid = fopen($output_file,"w");
fwrite($fid,json_encode($vmdata,JSON_PRETTY_PRINT));
fclose($fid);

echo "Correcting QC Computations\n";
$qc = new QCComputations($output_file);
$output_file = $qc->qcCompute();
echo "   CORRECTED QC File: $output_file\n";

echo "Done\n";

?>
