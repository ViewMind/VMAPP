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
   $cat["0"]  = "11 13*";
   $cat["-1"] = "13 15*";
   $cat["-2"] = "15 17*";
   $cat["-3"] = "17 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "- 11*";
   $cat["yellow"] = "11 17*";
   $cat["red"]    = "17 +";
   $categories[$variable]["color"] = $cat;   

   /////
   $cat = array();
   $variable = "parahipocampal_region";   
   $cat["green"]  = "67* +";
   $cat["yellow"] = "55* 67";
   $cat["red"]    = "- 55";
   $categories[$variable]["color"] = $cat;  

   return $categories;
   
?>
