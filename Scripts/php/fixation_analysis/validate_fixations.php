<?php

   include_once ("online_fixations.php");

   $input = "gonogo_2021_08_20_18_49_r_hp.csv";
   
   const CSV_TRIAL                 = 0;
   const CSV_TIME                  = 1;
   const CSV_X                     = 2;
   const CSV_Y                     = 3;
   const CSV_NP                    = 4;
   const CSV_MINX                  = 5;
   const CSV_MAXX                  = 6;
   const CSV_MINY                  = 7;
   const CSV_MAXY                  = 8;
   const CSV_DISPX                 = 9;
   const CSV_DISPY                 = 10;
   const CSV_DISP                  = 11;
   const CSV_MAX_DISP              = 12;
   const CSV_DISP_BROKEN           = 13;
   const CSV_FIX_X                 = 14;
   const CSV_FIX_Y                 = 15;
   const CSV_FIX_T                 = 16;
   const CSV_FIX_D                 = 17;

   $minimum_fixation_size = 50;
   $sample_frequency = 120;

   $lines = explode("\n",file_get_contents($input));
   array_shift(($lines)); // Removing the header. 

   // Gettting the maximum dispersion
   $all_cols = explode(",",$lines[0]);
   $max_dispersion = $all_cols[CSV_MAX_DISP];
   echo "MAX DISPERSION: $max_dispersion\n";


   $onlineFix = new OnlineMovingWindowAlgorithm($max_dispersion,$sample_frequency,$minimum_fixation_size);

   $last_trial = -1;
   $fix_counter = 0;

   foreach ($lines as $data_rows){

      $cols = explode(",",$data_rows);
      if (count($cols) < 10) continue;

      $x = $cols[CSV_X];
      $y = $cols[CSV_Y];
      $time = $cols[CSV_TIME];
      $trial = $cols[CSV_TRIAL];
      $fix_x = $cols[CSV_FIX_X];

      //echo "TIME $time\n";
      
      if ($fix_x != "N/A"){
         $fix_y = $cols[CSV_FIX_Y];
         $fix_d = $cols[CSV_FIX_D];            
      }


      // Preparing the fixation. 
      $fix = null;

      if ($last_trial != $trial){
         $last_trial = $trial;
         $fix = $onlineFix->finalizeComputation();         
      }

      $fix = $onlineFix->computeFixationsOnline($x, $y, $time);
      echo $onlineFix->traceString($trial) . "\n";

      if ($fix->valid){
         if ($fix_x != "N/A") {
            $f = new Fixation();
            $f->duration = $fix_d;
            $f->x = $fix_x;
            $f->y = $fix_y;
            $ans = $fix->compareFixation($f);
            if ($ans != ""){
               echo "BAD FIXATION: $ans\n";
               exit();
            }
         }
         else{
            echo "Found fixation not found in reference at time $time\n";
            exit();
         }
         $fix_counter++;         
         $last_fixation = $fix;
      }
      else if ($fix_x != "N/A") {
         if ($cols[CSV_DISP_BROKEN] == 1) {
            echo "Missed Fixation with values $fix_x, $fix_y of $fix_d at time $time\n";
            exit();
         }
      }

   }

   echo "Number of fixations $fix_counter\n";





?> 
