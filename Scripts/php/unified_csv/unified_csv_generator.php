<?php
   include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
   include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");
   include_once("nbackms_hitboxes.php");

   
   /// FIRST PASS: CSV Generation as IS. And Raw Data CSV Generation.
   $input_dir = "raw_data";
   
   $json_file_list = scandir($input_dir);

   $csv_list = []; // simply associates a generated CSV file with it's subject id. 

   // Data set number map
   $ds_number_map = [
      Study::GONOGO => [
         DataSetType::UNIQUE => "N/A"
      ],
      Study::NBACKMS => [
         DataSetType::ENCODING_1 => "1",
         DataSetType::ENCODING_2 => "2",
         DataSetType::ENCODING_3 => "3",
         DataSetType::RETRIEVAL_1 => "4",
         DataSetType::RETRIEVAL_2 => "4",
         DataSetType::RETRIEVAL_3 => "4"
      ]
   ];

   $unified_csv_list = array();


   foreach ($json_file_list as $filename){

      if (is_dir($filename)) continue; // Ignore ANY direcotories.
      if (str_contains($filename,"whb")) continue; // Skip files taht are modified form the origina. 
      if (str_ends_with($filename,".csv")) continue;
      if (!str_ends_with($filename,".json")) continue;
      
      echo "Processing file $filename\n";

      $input_json_file = "$input_dir/$filename";
      $output_file = "";

      if (str_contains($filename,"nbackms")){
         // NBackMS requires the computation of the target boxes. 
         $pathinfo = pathinfo($input_json_file);
         $dirname = $pathinfo["dirname"];
         $fname   = $pathinfo["filename"];
         $output_file = "$dirname/$fname-whb.json";
      
         $json = json_decode(file_get_contents($input_json_file),true);
         $h = $json["processing_parameters"]["resolution_height"];
         $w = $json["processing_parameters"]["resolution_width"];
         $json["processing_parameters"]["nback_hitboxes"] = nbackMSHitBoxes($w,$h);
      
         $fid = fopen($output_file,"w");
         fwrite($fid,json_encode($json));
         fclose($fid);
      }
      else {
         $output_file = $input_json_file;
      }

      $csvs = [];
      $vmdc = null;
      $ans = processJSONFileFull($output_file,$csvs,$vmdc);
      if ($ans != ""){
         echo "   Failed to process JSON File: $input_json_file. Reason: $ans\n";
         exit();
      }

      // Getting the internal (i.e. assigned by institution) subject id. 
      $display_id = $vmdc->getSubjectDataValue(SubjectField::INSTITUTION_PROVIDED_ID);
      $age        = $vmdc->getSubjectDataValue(SubjectField::AGE);
      $subject_id = $vmdc->getSubjectDataValue(SubjectField::LOCAL_ID);
   
      echo "   Generated CSVs:\n";
      foreach ($csvs as $study => $filename){
         echo "      $study => $filename\n";
         //$csv_list[] = [$filename, $display_id, $age];

         // Generating the raw data CSV.         
         $raw_data_csv = "raw-data-$filename";      

         $pathinfo = pathinfo($filename);
         $dirname = $pathinfo["dirname"];
         $fname   = $pathinfo["filename"];
         $raw_data_csv = "$dirname/raw_data-$fname.csv";

         $vmdc->setRawDataAccessPathForStudy($study);
         $trial_list = $vmdc->getTrialList();
         $csv_rows = array();
         $csv_rows[] = implode(",",["display_id","age","subject","trial_id","trial_part","timestamp","xr","yr","xl","yl","distance"]);

         if (!array_key_exists($study,$ds_number_map)){
            echo "   Don't know how to form raw data for strudy $study";
            exit();
         }

         foreach ($trial_list as $trial){

            foreach ($ds_number_map[$study] as $dataset => $num){
               $raw_data = $trial["data"][$dataset]["raw_data"];
               //var_dump($raw_data);
               //exit();

               foreach ($raw_data as $point){
                  $row = [];
                  $row[] = $display_id;
                  $row[] = $age;
                  $row[] = $subject_id;
                  $row[] = $trial["ID"];
                  $row[] = $num;
                  $row[] = $point["ts"];
                  $row[] = $point["xr"];
                  $row[] = $point["yr"];
                  $row[] = $point["xl"];
                  $row[] = $point["yl"];
                  $d = sqrt(($point["xr"]-$point["xl"])*($point["xr"]-$point["xl"]) + ($point["yr"]-$point["yl"])*($point["yr"]-$point["yl"]));
                  $row[] = $d;
                  $csv_rows[] = implode(",",$row);
               }

            }

         }

         // Saving the raw data file. 
         $fid = fopen($raw_data_csv,"w");
         fwrite ($fid, implode("\n",$csv_rows));
         fclose($fid);

         // Saving both.
         if (!array_key_exists($study,$unified_csv_list)){
            $unified_csv_list[$study] = array();
            $unified_csv_list[$study]["fix"] = array();
            $unified_csv_list[$study]["raw"] = array();
         }

         $fix["fix"] = $filename;
         $fix["age"] = $age;
         $fix["did"] = $display_id;

         $unified_csv_list[$study]["fix"][] = $fix;
         $unified_csv_list[$study]["raw"][] = $raw_data_csv;

      }

   }

   
   /// SECOND PASS: Creates the actuall unified CSVs. 

   echo "Generating unified CSV ...\n";
   
   // Unifying processed and raw data CSV per study
   
   foreach ($unified_csv_list as $study => $fix_and_rawdata){

      $study_as_file = strtolower($study);
      $study_as_file = str_replace(" ","_",$study_as_file);
      
      $unified_file_name = "$input_dir/$study_as_file.csv";
      $unified_file_name_raw_data = "$input_dir/raw_data_$study_as_file.csv";

      $final_CSV = array();

      $is_first = true;
      foreach($fix_and_rawdata["fix"] as $fixdata){
         $age = $fixdata["age"];
         $did = $fixdata["did"];

         $lines = explode("\n",file_get_contents($fixdata["fix"]));

         if ($is_first){
            $line = $lines[0];
            $is_first = false;
            $line = "display_id,age,$line";
            $lines[0] = $line;
            $start_i = 1;
         }
         else {
            array_shift($lines);
            $start_i = 0;
         }
  
         for ($i = $start_i; $i < count($lines); $i++){
            $line = $lines[$i];
            $lines[$i]  = "$did,$age,$line";
         }

         $final_CSV[] = implode("\n",$lines);

      }

      echo "   Saving unified CSV for study $study as $unified_file_name\n";
      $fid = fopen($unified_file_name,"w");
      fwrite($fid,implode("\n",$final_CSV));
      fclose($fid);


      $final_CSV = array();
      $is_first = true;
      foreach($fix_and_rawdata["raw"] as $csvfile){

         $lines = explode("\n",file_get_contents($csvfile));

         if ($is_first){
            $is_first = false;
            $start_i = 1;
         }
         else {
            array_shift($lines);
            $start_i = 0;
         }

         $final_CSV[] = implode("\n",$lines);

      }  
      

      echo "   Saving unified raw data CSV for study $study as $unified_file_name_raw_data\n";
      $fid = fopen($unified_file_name_raw_data,"w");
      fwrite($fid,implode("\n",$final_CSV));
      fclose($fid);
      $final_CSV = array();      

   }




?>