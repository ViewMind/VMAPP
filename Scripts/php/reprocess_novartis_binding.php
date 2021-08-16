<?php

   $SOURCE = "/home/ariel/Documents/ViewMind/Novartis/989302458";
   $RSCRIPTS = "/home/ariel/repos/viewmind_projects/Scripts/R";
   $RBINDING = "binding<N>.R";
   $RMODEL = "binding<N>_model.RDS";

   $naming_map = [
      "visual_search"              => "VISUAL SEARCH",
      "gaze_dur"                   => "VISUAL WORKING MEMORY",
      "int_mem_perf"               => "INTEGRATIVE MEMORY PERFORMANCE",
      "assoc_mem_perf"             => "ASSOCIATIVE MEMORY PERFORMANCE",
      "exec_func"                  => "EXECUTIVE FUNCTIONS",
      "parahipocampal_region"      => "PARAHIPOCAMPAL REGION PRESERVATION",
      "bc.gazing.encoding"         => "INTEGRATIVE MEMORY VISUAL WORKING MEMORY DURING ENCODING",
      "uc.gazing.encoding"         => "ASSOCIATIVE MEMORY VISUAL WORKING MEMORY DURING ENCODING",
      "bc.saccade.encoding"        => "INTEGRATIVE MEMORY VISUAL SEARCH DURING ENCODING",
      "uc.saccade.encoding"        => "ASSOCIATIVE MEMORY VISUAL SEARCH DURING ENCODING",
      "bc.duration.encoding"       => "INTEGRATIVE MEMORY FIXATION DURATION DURING ENCODING",
      "uc.duration.encoding"       => "ASSOCIATIVE MEMORY FIXATION DURATION DURING ENCODING",
      "bc.num_fixations.encoding"  => "INTEGRATIVE MEMORY EXECUTIVE FUNCTIONS MEMORY DURING ENCODING",
      "uc.num_fixations.encoding"  => "ASSOCIATIVE MEMORY EXECUTIVE FUNCTIONS MEMORY DURING ENCODING",
      "bc.gazing.retrieval"        => "INTEGRATIVE MEMORY VISUAL WORKING MEMORY DURING RECOGNITION",
      "uc.gazing.retrieval"        => "ASSOCIATIVE MEMORY VISUAL WORKING MEMORY DURING RECOGNITION",
      "bc.saccade.retrieval"       => "INTEGRATIVE MEMORY VISUAL SEARCH DURING RECOGNITION",
      "uc.saccade.retrieval"       => "ASSOCIATIVE MEMORY VISUAL SEARCH DURING RECOGNITION",
      "bc.duration.retrieval"      => "INTEGRATIVE MEMORY DURATION DURING RECOGNITION",
      "uc.duration.retrieval"      => "ASSOCIATIVE MEMORY DURATION DURING RECOGNITION",
      "bc.num_fixations.retrieval" => "INTEGRATIVE MEMORY EXECUTIVE FUNCTIONS DURING RECOGNITION",
      "uc.num_fixations.retrieval" => "ASSOCIATIVE MEMORY EXECUTIVE FUNCTIONS DURING RECOGNITION"
   ];

   $to_remove = [ "BEHAVIOURAL RESPONSE" ,"INDEX OF AMYLOID BETA PROTEIN CORRELATION" ];

   $to_replace = ["INDEX OF COGNITIVE IMPAIRMENT" => 
   "OVERALL INDEX OF COGNITIVE IMPAIRMENT"];

   $skip_processing = true;

   function recursiveFileSearchRun($dir,&$files){

      $all_items =  scandir($dir);
      foreach($all_items as $item){
         

         if ($item == "..") continue;
         if ($item == ".") continue;
                  
         $newitem = "$dir/$item";         

         if (is_file($newitem)){ 
            //echo "$newitem\n";           
            $files[] = $newitem;
         }
         else if (is_dir($newitem)){
            recursiveFileSearchRun($newitem,$files);
         }

      }

   }

   function getValue($array,$key){
      $key = explode(".",$key);
      $value = $array;
      foreach ($key as $level){
         if (array_key_exists($level, $value)) {
            $value = $value[$level];
         }
         else {
            echo "level: $level\n";
            false;
         }
      }
      return $value;
   }

   $file_list = [];
   recursiveFileSearchRun($SOURCE,$file_list);
   $binding_to_process =  [];

   /**
    * Putting together the structure for actuall processing the filess. 
    */

   foreach ($file_list as $file){

      // Only insterested in CSV files. 
      if (strpos($file,".csv") === false) continue;

      if (strpos($file, "binding") !== false) {
         $info = pathinfo($file);
         $dirname = $info["dirname"];
         $filename = $info["basename"];

         $binding = array();
         if (array_key_exists($dirname,$binding_to_process)){
            $binding = $binding_to_process[$dirname];
         }

         $key = "";
         $ntargets = "";

         if (strpos($file, "_3_") !== false) {
            $ntargets = "3";
            if (strpos($file,"_bc_") !== false) $key = "bc";
            else if (strpos($file,"_uc_") !== false) $key = "uc";
         }
         else if (strpos($file, "_2_") !== false){
            $ntargets = "2";
            if (strpos($file,"_bc_") !== false) $key = "bc";
            else if (strpos($file,"_uc_") !== false) $key = "uc";            
         }
         
         if ($key == "") {
            echo "Unknown binding file $file\n";
            exit();
         }  

         if (array_key_exists($ntargets,$binding)){
            if (array_key_exists($key,$binding[$ntargets])){
               echo "Found duplicated key in binding: $key for $ntargets for dirname $dirname\n";
               exit();
            }            
         }
         else{
            $binding[$ntargets] = array();
         }

         $binding[$ntargets][$key] = $filename;   
         $binding_to_process[$dirname] = $binding;

      }
   }

   // /**
   //  * Running through processing. 
   //  */

   if (!$skip_processing) {
       $total = count($binding_to_process);
       $counter = 0;

       foreach ($binding_to_process as $dirname => $binding) {
           $counter++;
           echo "Processing binding $counter of $total\n";

           $ntargets = array_keys($binding)[0];
           $uc_file = $dirname . "/" . $binding[$ntargets]["uc"];
           $bc_file = $dirname . "/" . $binding[$ntargets]["bc"];
           //echo "UC: $uc_file\n";
           //echo "BC: $bc_file\n";
           $rscript = $RSCRIPTS . "/" . str_replace("<N>", $ntargets, $RBINDING);
           $model   = $RSCRIPTS . "/" . str_replace("<N>", $ntargets, $RMODEL);
           $output  = $dirname . "/binding$ntargets.json";
           $input   = $bc_file . " " . $uc_file;

           $cmd = "Rscript $rscript $input $model $output 2>&1";
           $temp = $cmd . "\n============================================================\n";
           $routput = array();
           exec($cmd, $routput, $retval);
           $routput[] = "Exit Code: $retval";
           $temp = $temp . implode("\n", $routput);

           if ($retval != 0) {
               echo "\nProcessing failed with command $cmd\n";
               echo "Output: " . implode("\n", $routput) . "\n";
               exit();
           }
       }
   }

 
   /**
    * Processing the existing ini file and JSON file. 
    */

    foreach ($binding_to_process as $dirname => $binding) {
      $ntargets = array_keys($binding)[0];       
      $output  = $dirname . "/binding$ntargets.json";

      $binding_variables = json_decode(file_get_contents($output),true);
      if (json_last_error() != JSON_ERROR_NONE){
         echo "Failed to load results at $dirname\n";
         exit();
      }
      
      // finding the rep file.
      $rep_file = "";
      $allfiles = scandir($dirname);
      foreach ($allfiles as $file){
         if (strpos($file,".rep") !== false){
            $rep_file = $file;
            break;
         }
      }

      // Parsing the rep file.
      $report = parse_ini_file($dirname . "/" . $rep_file);

      // Removing old stuff.
      foreach ($to_remove as $key){
         unset($report[$key]);
      }

      // Everything that wasn't removed was reading. 
      $reading = $report;
      foreach ($report as $key => $value){
         unset($report[$key]);
      }
      $report["READING"] = $reading;

      // var_dump($report);
      // echo "===============================\n";

      // Creating the new names 
      $report["BINDING"] = array();
      foreach ($naming_map as $key => $name){
         $value = getValue($binding_variables,$key);
         if ($value === false){
            echo "Unable to find key $key in $output\n";
            var_dump($binding_variables);
            exit;
         }
         $repline = "";
         if (is_array($value)){
            //$repline = $name . " = " . $value[0] . " SE: " . $value[1] . ";";
            $report["BINDING"][$name] = '"' . $value[0] . " SE: " . $value[1] . '"';
         }
         else{
            //$repline = $name . " = " . $value . ";";
            $report["BINDING"][$name] = $value;
         }
         //echo $repline . "\n";
      }

      // Replacing names
      foreach ($to_replace as $oldname => $newname) {
         if (array_key_exists($oldname,$report)){
            $report[$newname] = $report[$oldname];
            unset($report[$oldname]);
         }
      }
      
      // Recreating the ini file.
      $output_ini = "$dirname/report.ini";
      $lines = [];
      foreach ($report as $group => $group_array){
         $lines[] = "[$group]";
         foreach ($group_array as $key => $value) {
             $line = $key . " = " . $value . ";";
             $lines[] = $line;
         }
      }

      $fid = fopen($output_ini,"w");
      fwrite($fid,implode("\n",$lines));
      fclose($fid);
      //echo "Generate output $output_ini\n";

    }

   

?>
