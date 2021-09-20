<?php

   // This is the hieararchical order of the variables in order to access the pdiag object. 
   //$variables = ["parahipocampal_region","exec_func","int_mem_perf","assoc_mem_perf"];
   
   // This should be the same as the CSV column order. 
   $variables = ["processing_speed","index_error_interference","index_error_facilitated","executive_functions"];
   //processing speed,
   //Inhibitory_alterations_in_Task_with_Interference,
   //Inhibitory_alterations_in_Task_Facilitated,
   //Executive Functions
   
   // Input CSV Table. . 
   $csv_input = "/home/ariel/repos/viewmind_projects/VMPortal/Backend/pdf_report_generation/doc_and_examples/reference_files/pdiag_gonogo.csv";

   $expected_count = 81;  // 4 Variables of 3 possibilities each give for 81 combinations.

   $expected_column_count = 5;  // 4 Variables in the order above plus the string id column. 

   $lookup = array();

   if (!is_file($csv_input)){
      echo "Could not find pdiag CSV file for GoNoGo: $csv_input\n";
      exit();
   }

   $contents = file_get_contents($csv_input);
   
   // Separating by lines. 
   $contents = explode("\n",$contents);
   unset($contents[0]); // Removing the header.

   foreach ($contents as $line){

      $cols = explode(",",$line);
      if (count($cols) != $expected_column_count) continue;

      // Decreasing the count to make sure we account for all possibilities. 
      $expected_count--;

      $proc_speed              = $cols[0];
      $index_err_interf        = $cols[1];
      $index_err_facilitated   = $cols[3];
      $exec_func               = $cols[2];      
      $string_id               = $cols[4];

      $lookup[$proc_speed][$index_err_interf][$index_err_facilitated][$exec_func] = $string_id;

   }

   if ($expected_count > 0){
      echo "PDiag GoNoGo: Expected count is non zero after parsing: $expected_count\n";
      exit();
   }
   
   $pdiag["variables"] = $variables;
   $pdiag["text_index"] = $lookup;
   return $pdiag;


?>