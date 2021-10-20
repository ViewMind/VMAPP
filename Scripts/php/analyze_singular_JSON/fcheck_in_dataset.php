<?php

   /**
    * VERY VERY Quick and dirty tool to check on frequency regularity on a specific time frame. 
    */

   //$input_file = "local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29_reprocessed.json";
   $input_file = "local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29_interpolated.json";
   $window = [2948766,2948358];

   $vmdata = json_decode(file_get_contents($input_file),true);

   $studies = $vmdata["studies"];

   $print_status = 0;
   $last = -1;

   foreach ($studies as $study_name => $study_data){
      $trial_list = $study_data["trial_list"];
      foreach ($trial_list as $trial){
         $data = $trial["data"];

         $print_status = 0;
         $last = -1;

         foreach ($data as $dataset_name => $dataset){
            $raw_data = $dataset["raw_data"];
            foreach ($raw_data as $point){
               
               if ($print_status === 0){
                  if ($point["ts"] == $window[0]){
                     $last = $point["ts"];
                     $print_status++; 
                     echo "WINDOW START\n";
                     echo $last . "\n";
                  }
               }
               else if ($print_status === 1){
                  $current = $point["ts"];
                  $diff = $current - $last;
                  $last = $current;
                  echo "$current $diff\n";
                  if ($current == $window[1]){
                     echo "WINDOW END\n";
                     $print_status = 0;
                  }
               }

            }
         }
      }
   }


?> 
