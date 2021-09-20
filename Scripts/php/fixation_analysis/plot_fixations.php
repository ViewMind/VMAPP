<?php

   // Assuming this was the resolution. 
   $resolution_from_code = [
      //1 => [1920,1080],  // GP - Full HD.
      1 => [1366,768],     // GP - HD
      2 => [2316,2376],    // HP
      3 => [1600,900],     // SMI
      4 => [1532,1704],    // HTC. 
   ];

   $target_md = 200;

// For the Reading Files. 
//    $files_to_process = [
//       "reading/gp_files/reading_2021_08_26_07_42_r_gp.raw" => [
//          "f" => 150,
//          "matlab_code" => 1,
//          "suffix" => "gp",
//       ],
//       "reading/hp_files/reading_2021_08_26_07_29_r_hp_full_res.raw" => [
//          "f" => 120,
//          "matlab_code" => 2,
//          "suffix" => "hp",
//       ],
//       "reading/smi_files/jonatan_everti_reading_2018_09_18.raw" => [
//          "f" => 120,
//          "matlab_code" => 3,
//          "suffix" => "smi",
//       ]
//    ];   

   $files_to_process = [
      "binding/gp_files/binding_bc_3_l_2_2019_04_02_09_54.raw" => [
         "f" => 150,
         "matlab_code" => 1,
         "suffix" => "gp",
      ],
      "binding/htc_files/binding_bc_3_l_2_2020_07_14_19_38.raw" => [
        "f" => 120,
        "matlab_code" => 4,
        "suffix" => "htc",
     ]        
   ];  

   

   // Column position for standarized CSV files. 
   const COL_TRIAL_NUMBER = 0;
   const COL_TIME         = 1;
   const COL_XR           = 2;
   const COL_YR           = 3;
   const COL_XL           = 4;
   const COL_YL           = 5;

   const FIX_COL_CODE     = 0;
   const FIX_COL_MD       = 1;
   const FIX_COL_TRIAL    = 2;
   const FIX_COL_DUR      = 3;
   const FIX_COL_X        = 4;
   const FIX_COL_Y        = 5;
   const FIX_COL_XMIN     = 6;
   const FIX_COL_XMAX     = 7;
   const FIX_COL_YMIN     = 8;
   const FIX_COL_YMAX     = 9;
   const FIX_COL_TIME     = 10;

   $image_dir = "fixation_plots";
   shell_exec("rm -rf $image_dir");
   shell_exec("mkdir -p $image_dir");


   //$fix_point_file      = "reading/matlab_fixation_list.fix";
   $fix_point_file      = "binding/matlab_fixation_list.fix";

   //$trials_to_process = [1,2,4,5,6,7,9];                // The Reading Trials. 
   $trials_to_process = [1, 2, 29, 30, 49, 50, 69, 70];   // Trials 1 15 25 35 from Binding. 

   foreach ($files_to_process as $raw_data_point_file => $process_data) {

      $suffix = $process_data["suffix"];
      $current_code = $process_data["matlab_code"];
      $resolution = $resolution_from_code[$current_code];
      $W = $resolution[0];
      $H = $resolution[1];

      $R = $W*0.01;

      // Loading the raw data.
      $lines = explode("\n", file_get_contents($raw_data_point_file));
      $trial_raw_data = array();
      foreach ($trials_to_process as $trial) {
          $trial_raw_data[$trial] = array();
      }
      foreach ($lines as $line) {
          $cols = explode(",", $line);
          if (count($cols) != 6) {
              continue;
          }
          $trial_number = $cols[COL_TRIAL_NUMBER];
          //echo "$trial_number\n";
          if (array_key_exists($trial_number, $trial_raw_data)) {
              $sample = array();
              $sample["time"] = $cols[COL_TIME];
              $sample["xl"] = $cols[COL_XL];
              $sample["xr"] = $cols[COL_XR];
              $sample["yl"] = $cols[COL_YL];
              $sample["yr"] = $cols[COL_YR];
              $trial_raw_data[$trial_number][] = $sample;
          }
      }

      // Loading the fixation files.
      $lines = explode("\n", file_get_contents($fix_point_file));
      $fixations = [];
      foreach ($trials_to_process as $trial) {
          $fixations[$trial] = array();
      }
      foreach ($lines as $line) {
          $cols = explode(" ", $line);
          if (count($cols) != 11) {
              continue;
          }
          
          $trial_number = $cols[FIX_COL_TRIAL];
          $md           = $cols[FIX_COL_MD];
          $code         = $cols[FIX_COL_CODE];

          // We only want the fixations for the ET under analysis anf the specified maximum dispersion.
          if ($current_code != $code) continue;
          if ($md != $target_md) continue;

          if (array_key_exists($trial_number, $trial_raw_data)) {
              $sample = array();
              $sample["x"]    = $cols[FIX_COL_X];
              $sample["y"]    = $cols[FIX_COL_Y];
              $sample["xmin"] = $cols[FIX_COL_XMIN];
              $sample["xmax"] = $cols[FIX_COL_XMAX];
              $sample["ymin"] = $cols[FIX_COL_YMIN];
              $sample["ymax"] = $cols[FIX_COL_YMAX];
              $fixations[$trial_number][] = $sample;
          }
      }
      
      // Doing the actual fixation plotting.
      foreach ($trials_to_process as $trial) {
          $image = imagecreatetruecolor($W, $H);
          // Color definitions
          $blue = imagecolorallocate($image, 0, 0, 255);
          $red = imagecolorallocate($image, 255, 0, 0);
          $gray = imagecolorallocate($image, 200, 200, 200);
          $black = imagecolorallocate($image, 0, 0, 0);
          imagefill($image, 0, 0, $gray);
          foreach ($trial_raw_data[$trial] as $sample) {
              $x = $sample["xr"];
              $y = $sample["yr"];
              imageellipse($image, $x, $y, $R, $R, $blue);
          }
          $nfix = 0;
          foreach ($fixations[$trial] as $sample) {
              $x = $sample["x"];
              $y = $sample["y"];
              $x1 = $sample["xmin"];
              $y1 = $sample["ymin"];
              $x2 = $sample["xmax"];
              $y2 = $sample["ymax"];
              imagefilledellipse($image, $x, $y, $R, $R, $red);
              //echo "$x1 $x2 $y1 $y2\n";
              imagerectangle($image, $x1, $y1, $x2, $y2, $red);
              $nfix++;
          }
          imagestring($image, 5, $W/2, 50, "Number of Fixations: $nfix", $black);
          imagepng($image, "$image_dir/$suffix-$target_md-$trial.png");
      }
   }

?> 
