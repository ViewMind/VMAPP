<?php
   //////////////////////////////////////////////////////////// CATEGORY STRINGS.   
   $categories = array();

   /////
   $cat = array();
   $variable = "processing_speed";   
   $cat["+1"]  = "- 0.99*";
   $cat["0"]   = "0.99 1.80*";
   $cat["-1"]  = "1.80 1.88*";
   $cat["-2"]  = "1.88 1.93*";
   $cat["-3"]  = "1.93 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["green"]  = "- 1.80*";
   $cat["yellow"] = "1.80 1.93*";
   $cat["red"]    = "1.93 +";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "processing_speed_interference";   
   $cat["+1"]  = "- 49*";
   $cat["0"]  = "49 93*";
   $cat["-1"] = "93 103*";
   $cat["-2"] = "100 110*";
   $cat["-3"] = "110 +";
   $categories[$variable]["polygon"] = $cat;  

   $cat = array();
   $cat["green"]  = "- 93*";
   $cat["yellow"] = "93 110*";
   $cat["red"]    = "110 +";
   $categories[$variable]["color"] = $cat;   

   /////
   $cat = array();
   $variable = "processing_speed_facilitated";   
   $cat["+1"]  = "- 49*";
   $cat["0"]  = "49 85*";
   $cat["-1"] = "85 99*";
   $cat["-2"] = "99 105*";
   $cat["-3"] = "105 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 85*";
   $cat["yellow"] = "85 105*";
   $cat["red"]    = "105 +";
   $categories[$variable]["color"] = $cat;      

   /////
   $cat = array();
   $variable = "index_error_interference";   
   $cat["+1"]  = "- 1*";
   $cat["0"]  = "1 31*";
   $cat["-1"] = "31 38*";
   $cat["-2"] = "39 45*";
   $cat["-3"] = "46 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 31*";
   $cat["yellow"] = "31 45*";
   $cat["red"]    = "45 +";
   $categories[$variable]["color"] = $cat;   
  
   /////
   $cat = array();
   $variable = "index_error_facilitated";   
   $cat["+1"]  = "- 1*";
   $cat["0"]  = "1 15*";
   $cat["-1"] = "15 25*";
   $cat["-2"] = "25 32*";
   $cat["-3"] = "32 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 15*";
   $cat["yellow"] = "15 32*";
   $cat["red"]    = "32 +";
   $categories[$variable]["color"] = $cat;  

   /////
   $cat = array();
   $variable = "reaction_time_facilitated";   
   $cat["+1"]  = "- 150*";
   $cat["0"]  = "150 784*";
   $cat["-1"] = "784 825*";
   $cat["-2"] = "825 873*";
   $cat["-3"] = "873 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 784*";
   $cat["yellow"] = "784 873*";
   $cat["red"]    = "873 +";
   $categories[$variable]["color"] = $cat;       

   /////
   $cat = array();
   $variable = "reaction_time_interference";   
   $cat["+1"]  = "- 150*";
   $cat["0"]  = "150 829*";
   $cat["-1"] = "829 850*";
   $cat["-2"] = "850 946*";
   $cat["-3"] = "947 +";
   $categories[$variable]["polygon"] = $cat;   


   $cat = array();
   $cat["green"]  = "- 829*";
   $cat["yellow"] = "829 946*";
   $cat["red"]    = "946 +";
   $categories[$variable]["color"] = $cat;   
   
   /////
   $cat = array();
   $variable = "executive_functions";   
   $cat["+1"]  = "- 100*";
   $cat["0"]  = "100 320*";
   $cat["-1"] = "320 370*";
   $cat["-2"] = "370 420*";
   $cat["-3"] = "420 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 320*";
   $cat["yellow"] = "320 420*";
   $cat["red"]    = "420 +";
   $categories[$variable]["color"] = $cat;     
     
   return $categories;
   
?>
