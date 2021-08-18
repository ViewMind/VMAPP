<?php

   // This is the hieararchical order of the variables in order to access the pdiag object. 
   //$variables = ["parahipocampal_region","exec_func","int_mem_perf","assoc_mem_perf"];
   
   // This should be the same as the CSV column order. 
   $variables = ["int_mem_perf","assoc_mem_perf","exec_func","parahipocampal_region"];
   
   $csv_input = "/home/ariel/repos/viewmind_projects/VMPortal/Backend/pdf_report_generation/doc_and_examples/reference_files/pdiag_3T_table.csv";

   $expected_count = 81;  // 4 Variables of 3 possibilities each give for 81 combinations.

   $expected_column_count = 5;  // 4 Variables in the order above plus the string id column. 

   $lookup = array();

   if (!is_file($csv_input)){
      echo "Could not find pdiag CSV file for Binding 3T: $csv_input\n";
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

      $int_mem_perf   = $cols[0];
      $assoc_mem_perf = $cols[1];
      $exec_func      = $cols[2];
      $parah_region   = $cols[3];
      $string_id      = $cols[4];

      $lookup[$int_mem_perf][$assoc_mem_perf][$exec_func][$parah_region] = $string_id;

   }

   if ($expected_count > 0){
      echo "PDiag Binding 3T: Expected count is non zero after parsing: $expected_count\n";
      exit();
   }
   
   $pdiag["variables"] = $variables;
   $pdiag["text_index"] = $lookup;
   return $pdiag;


?>