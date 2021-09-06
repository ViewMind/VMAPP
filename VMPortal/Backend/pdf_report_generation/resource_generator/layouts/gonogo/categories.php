<?php
   //////////////////////////////////////////////////////////// CATEGORY STRINGS.   
   $categories = array();

   /////
   // $cat = array();
   // $variable = "processing_speed";   
   // $cat["+1"]  = "- 0.99*";
   // $cat["0"]   = "0.99 1.88*";
   // $cat["-1"]  = "1.88 1.91*";
   // $cat["-2"]  = "1.92 1.95*";
   // $cat["-3"]  = "1.95 +";
   // $categories[$variable]["polygon"] = $cat;

   // $cat = array();
   // $cat["green"]  = "- 1.88*";
   // $cat["yellow"] = "1.88 1.95*";
   // $cat["red"]    = "1.95 +";
   // $categories[$variable]["color"] = $cat;

   // Crappy Made Up Values to Get All Greens;
   $cat = array();
   $variable = "processing_speed";   
   $cat["+1"]  = "- 0.1*";
   $cat["0"]   = "0.1 3.10*";
   $cat["-1"]  = "3.10 4*";
   $cat["-2"]  = "4 5*";
   $cat["-3"]  = "5 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 3.10*";
   $cat["yellow"] = "3.10 4.00*";
   $cat["red"]    = "4.00 +";
   $categories[$variable]["color"] = $cat;

   // /////
   // $cat = array();
   // $variable = "processing_speed_interference";   
   // $cat["+1"]  = "- 0.99*";
   // $cat["0"]  = "0.99 1.83*";
   // $cat["-1"] = "1.83 1.87*";
   // $cat["-2"] = "1.87 1.90*";
   // $cat["-3"] = "1.90 +";
   // $categories[$variable]["polygon"] = $cat;  

   // $cat = array();
   // $cat["green"]  = "- 83*";
   // $cat["yellow"] = "83 90*";
   // $cat["red"]    = "90 +";
   // $categories[$variable]["color"] = $cat;   

   // Crappy Made Up Values to Get All Greens;
   $cat = array();
   $variable = "processing_speed_interference";   
   $cat["+1"]  = "- 0.1*";
   $cat["0"]  = "0.1 285*";
   $cat["-1"] = "285 300*";
   $cat["-2"] = "300 400*";
   $cat["-3"] = "400 +";
   $categories[$variable]["polygon"] = $cat;  

   $cat = array();
   $cat["green"]  = "- 285*";
   $cat["yellow"] = "285 300*";
   $cat["red"]    = "300 +";
   $categories[$variable]["color"] = $cat;      

   // /////
   // $cat = array();
   // $variable = "processing_speed_facilitated";   
   // $cat["+1"]  = "- 0.99*";
   // $cat["0"]  = "0.99 1.75*";
   // $cat["-1"] = "1.75 1.80*";
   // $cat["-2"] = "1.80 1.85*";
   // $cat["-3"] = "1.85 +";
   // $categories[$variable]["polygon"] = $cat;   

   // $cat = array();
   // $cat["green"]  = "- 75*";
   // $cat["yellow"] = "75 85*";
   // $cat["red"]    = "85 +";
   // $categories[$variable]["color"] = $cat;   

   // Crappy Made Up Values to Get All Greens;
   $cat = array();
   $variable = "processing_speed_facilitated";   
   $cat["+1"]  = "- 0.1*";
   $cat["0"]  = "0.1 275*";
   $cat["-1"] = "275 300*";
   $cat["-2"] = "300 400*";
   $cat["-3"] = "400 +";
   $categories[$variable]["polygon"] = $cat;  

   $cat = array();
   $cat["green"]  = "- 275*";
   $cat["yellow"] = "275 300*";
   $cat["red"]    = "300 +";
   $categories[$variable]["color"] = $cat;      
   

   /////
   $cat = array();
   $variable = "index_error_interference";   
   $cat["+1"]  = "- 0.02*";
   $cat["0"]  = "0.02 0.31*";
   $cat["-1"] = "0.31 0.36*";
   $cat["-2"] = "0.37 0.40*";
   $cat["-3"] = "1.85 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 0.31*";
   $cat["yellow"] = "0.31 0.40*";
   $cat["red"]    = "0.40 +";
   $categories[$variable]["color"] = $cat;   
  
   /////
   $cat = array();
   $variable = "index_error_facilitated";   
   $cat["+1"]  = "- 0.02*";
   $cat["0"]  = "0.02 0.15*";
   $cat["-1"] = "0.15 0.19*";
   $cat["-2"] = "0.19 0.25*";
   $cat["-3"] = "0.25 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 0.15*";
   $cat["yellow"] = "0.15 0.25*";
   $cat["red"]    = "0.25 +";
   $categories[$variable]["color"] = $cat;  

   /////
   // $cat = array();
   // $variable = "reaction_time_facilitated";   
   // $cat["+1"]  = "- 150*";
   // $cat["0"]  = "150 784*";
   // $cat["-1"] = "784 825*";
   // $cat["-2"] = "825 873*";
   // $cat["-3"] = "873 +";
   // $categories[$variable]["polygon"] = $cat;   

   // $cat = array();
   // $cat["green"]  = "- 784*";
   // $cat["yellow"] = "784 873*";
   // $cat["red"]    = "873 +";
   // $categories[$variable]["color"] = $cat;    

   // Crappy Made Up Values to Get All Greens;
   $cat = array();
   $variable = "reaction_time_facilitated";   
   $cat["+1"]  = "- 0.1*";
   $cat["0"]  = "0.1 1700*";
   $cat["-1"] = "1700 1800*";
   $cat["-2"] = "1800 1900*";
   $cat["-3"] = "1900 +";
   $categories[$variable]["polygon"] = $cat;   

   $cat = array();
   $cat["green"]  = "- 1700*";
   $cat["yellow"] = "1700 1800*";
   $cat["red"]    = "1800 +";
   $categories[$variable]["color"] = $cat;      

   /////
   // $cat = array();
   // $variable = "reaction_time_interference";   
   // $cat["+1"]  = "- 150*";
   // $cat["0"]  = "150 829*";
   // $cat["-1"] = "829 850*";
   // $cat["-2"] = "850 946*";
   // $cat["-3"] = "947 +";
   // $categories[$variable]["polygon"] = $cat;   


   // $cat = array();
   // $cat["green"]  = "- 829*";
   // $cat["yellow"] = "829 946*";
   // $cat["red"]    = "946 +";
   // $categories[$variable]["color"] = $cat;   

   $cat = array();
   $variable = "reaction_time_interference";   
   $cat["+1"]  = "- 0.1*";
   $cat["0"]  = "0.1 1900*";
   $cat["-1"] = "1900 2000*";
   $cat["-2"] = "2000 2100*";
   $cat["-3"] = "2200 +";
   $categories[$variable]["polygon"] = $cat;   


   $cat = array();
   $cat["green"]  = "- 1900*";
   $cat["yellow"] = "2000 2100*";
   $cat["red"]    = "2100 +";
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
