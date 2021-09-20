<?php


/**
* We need to reference several different sources:
* 
* SOURCE: Language Object. The way to reference this is to use a string that starts denoted with $value.value$ the following string should be the hiearchy of the JSON object for the language.
* So for example #binding.visual_search# Will reference a string the languaje json that is first indexed by the word binding and then by the word visual_search. 
* 
* SOURCE: Report Result Object: Referenced in the exact same way as language by staring a value, object or array reference with report. However the delimiter is &&
* 
* All x dimensions are the effective.x + the value*effective.width;
* All y dimensions are the effective.y + the value*effective.height;
* All width are % of effective with. All heights are % of effective heighth.
* When a position is composed of the array the first value should be an id (string) that refereces the the x + wdith of a previosuly defined element while the second value is interpreted as above. 
* 
*/

function computeStandardDeviations($categories){

   $ret["sd"] = array();
   $ret["error"] = "";

   foreach ($categories as $variable_name => $array_of_categories){

      if (array_key_exists("skip_sd",$array_of_categories)){
         if ($array_of_categories["skip_sd"] === true){
            echo "SD SKIP: $variable_name\n";
            continue;
         }
      }

      if (!array_key_exists("polygon",$array_of_categories)){
         $ret["error"] = "polygon key is missing from the category $variable_name array";
         return $ret;
      }   

      $polygon = $array_of_categories["polygon"];
      if (!array_key_exists("-1",$polygon)){
         $ret["error"] = "polygon for  $variable_name does not have the -1 label";
         return $ret;
      }

      $values = $polygon["-1"];
      $clean_range = str_replace("*","",$values); // Removing the * to indicate inclusion. 

      if (str_contains($clean_range,"-")){
         $ret["error"] = "Range in $variable_name contains -infinity symbol: " . $values;
         return $ret;
      }
      else if (str_contains($clean_range,"+")){
         $ret["error"] = "Range in $variable_name contains +infinity symbol: " . $values;
         return $ret;
      }


      $min_max = explode(" ",$values);
      if (count($min_max) != 2){
         $ret["error"] = "Range in $variable_name provided with unexpected count of " . count($min_max) . ", the range is: " . $values;
         return $ret;
      }
      $min = doubleval($min_max[0]);
      $max = doubleval($min_max[1]);

      if ($min >= $max){
         $ret["error"] = "Standard deviation is 0 or negative for $values of $variable_name. Clean range $clean_range. MIN: $min. MAX: $max";
         return $ret;
      }

      $sd = strval($max - $min);

      //echo "SD is $max - $min = $sd\n";

      $ret["sd"][$variable_name] = $sd;
      
   }

   return $ret;

}

$output_file = "../v1/resources/report_layouts.json";
$layout = array();
$layout_path = "layouts";

$layout_files = scandir($layout_path);

foreach ($layout_files as $file){


   if ($file == ".") continue;
   if ($file == "..") continue;

   $file_path = "$layout_path/$file";
   if (is_file($file_path)){
      //echo "$file_path\n";
      $temp = include($file_path);
      if (array_key_exists("uuid",$temp)){
         $uuid = $temp["uuid"];
         unset($temp["uuid"]);

         // Computing the standar deviations for each category. 
         if (array_key_exists("categories",$temp)){
            $sd = computeStandardDeviations($temp["categories"]);
            if ($sd["error"] != ""){
               echo "Error in SD Computing for $uuid: " . $sd["error"] . "\n";
               exit();
            }
         }
         else{
            echo "Error: $uuid does not have a categories key\n";
            exit();
         }

         // Saving the computed standar deviations. 
         $temp["standard_deviations"] = $sd["sd"];

         $layout[$uuid] = $temp;
      }
      else{
         echo "Error: Cannot add file $file_path because it does not contain a uuid field\n";
      }
   }

}

////////////////////////////////////////////////////////////////// FINALIZING //////////////////////////////////////////////////////////////////
$fid = fopen($output_file,"w");
fwrite($fid,json_encode($layout,JSON_PRETTY_PRINT));
fclose($fid);
 
?>