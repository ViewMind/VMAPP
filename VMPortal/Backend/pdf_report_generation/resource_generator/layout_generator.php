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
