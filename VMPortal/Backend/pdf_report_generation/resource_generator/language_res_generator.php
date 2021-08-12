<?php

/**
 * The purpose of this script is simply to generate the JSON Language file.
 * It is a unified file indexe by the laguange code.
 * Each laguange is defined in object separated by directory and report on the laguanges folder.  
 * 
 */

$languages = array();
$output = "../v1/resources/report_strings.json";

$lang_dir_path = "languages";
$lang_dirs = scandir($lang_dir_path);

foreach ($lang_dirs as $lang){

   if ($lang == ".") continue;
   if ($lang == "..") continue;
   
   $path = "$lang_dir_path/$lang";
   if (is_dir($path)){
      $languages[$lang] = array();

      $files = scandir($path);

      foreach ($files as $file){

         if ($file == ".") continue;
         if ($file == "..") continue;

         $file_path = "$path/$file";
      
         if (is_file($file_path)){
            $temp = include($file_path);
            if (array_key_exists("uuid",$temp)){
               $uuid = $temp["uuid"];
               unset($temp["uuid"]);
               $languages[$lang][$uuid] = $temp;
            }
            else{
               echo "Error: Cannot add file $file_path because it does not contain a uuid field\n";
            }
         }

      }
      
   }

}

////////////////////////////////////////////////////////////////// FINALIZING //////////////////////////////////////////////////////////////////
$fid = fopen($output,"w");
fwrite($fid,json_encode($languages,JSON_PRETTY_PRINT));
fclose($fid);

?>
