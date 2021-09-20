<?php
   //////////////////////////////////////////////////////////// CATEGORY STRINGS.   
   $categories = array();
   
   /////
   $cat = array();
   $variable = "visual_search";   
   $cat["+1"]  = "- 2*";
   $cat["0"]  = "2 3.5*";
   $cat["-1"] = "3.5 5*";
   $cat["-2"] = "5 7*";
   $cat["-3"] = "7 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "- 3.5*";
   $cat["yellow"] = "3.5 7*";
   $cat["red"]    = "7 +";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "gaze_dur";   
   $cat["+1"]  = "950 +";
   $cat["0"]  = "800* 950";
   $cat["-1"] = "550* 800";
   $cat["-2"] = "400* 550";
   $cat["-3"] = "- 400";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "800* +";
   $cat["yellow"] = "400* 800";
   $cat["red"]    = "- 400";
   $categories[$variable]["color"] = $cat;   

   /////
   $cat = array();
   $variable = "int_mem_perf";   
   $cat["+1"]  = "73* +";
   $cat["0"]  = "68* 73";
   $cat["-1"] = "63* 68";
   $cat["-2"] = "59* 63";
   $cat["-3"] = "- 59";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "68* +";
   $cat["yellow"] = "59* 68";
   $cat["red"]    = "- 59";
   $categories[$variable]["color"] = $cat;   

   
  
   /////
   $cat = array();
   $variable = "assoc_mem_perf";   
   $cat["+1"]  = "72* +";
   $cat["0"]  = "67* 72";
   $cat["-1"] = "62* 67";
   $cat["-2"] = "58* 62";
   $cat["-3"] = "- 58";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "67* +";
   $cat["yellow"] = "58* 67";
   $cat["red"]    = "- 58";
   $categories[$variable]["color"] = $cat;   

   /////
   $cat = array();
   $variable = "exec_func";   
   $cat["+1"]  = "- 11*";
   $cat["0"]  = "11 16*";
   $cat["-1"] = "16 19*";
   $cat["-2"] = "19 21*";
   $cat["-3"] = "21 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "- 16*";
   $cat["yellow"] = "16 21*";
   $cat["red"]    = "21 +";
   $categories[$variable]["color"] = $cat;   

   /////
   $cat = array();
   $variable = "parahipocampal_region";   
   $cat["green"]  = "63* +";
   $cat["yellow"] = "45* 63";
   $cat["red"]    = "- 45";
   $categories[$variable]["color"] = $cat;  
   $categories[$variable]["skip_sd"] = true; // The flag tells the parser script to skip the SD computation for this variable. 

   return $categories;
   
?>
