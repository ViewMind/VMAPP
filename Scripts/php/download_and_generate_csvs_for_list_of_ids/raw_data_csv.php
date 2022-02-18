<?php

include_once(__DIR__."/../reprocess_legacy_data/processing_common.php");
include_once(__DIR__."/../../../VMPortal/Backend/data_processing/RProcessing.php");

/**
 * @param $csvs - Output (passed as a parameter) from processJSONFileFull.
 * @param $vmdc - ViewMind Data Container also passed as a parameter to procesJSONFileFull. 
 */

class RawDataCSVGen { 

   // Data set number map
   const DS_NUMBER_MAP = [
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
   
   static function generateRawDataCSV($csvs,$vmdc){

      $display_id = $vmdc->getSubjectDataValue(SubjectField::INSTITUTION_PROVIDED_ID);
      $age        = $vmdc->getSubjectDataValue(SubjectField::AGE);
      $subject_id = $vmdc->getSubjectDataValue(SubjectField::LOCAL_ID);
   
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
   
         if (!array_key_exists($study,self::DS_NUMBER_MAP)){
            echo "   Don't know how to form raw data for strudy $study";
            exit();
         }
   
         foreach ($trial_list as $trial){
   
            foreach (self::DS_NUMBER_MAP[$study] as $dataset => $num){
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

      }
   
   }

}

?>