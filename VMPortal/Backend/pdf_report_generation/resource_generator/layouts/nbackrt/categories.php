<?php
   //////////////////////////////////////////////////////////// CATEGORY STRINGS.   
   $categories = array();

   /////
   $cat = array();
   $variable = "visual_exploration_efficiency";   
   $cat["+1"]  = "6* +";
   $cat["0"]   = "4* 6";
   $cat["-1"]  = "3.5* 4";
   $cat["-2"]  = "2.9* 3.5";
   $cat["-3"]  = "- 2.9";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "6* +";
   $cat["green"]  = "4* 6";
   $cat["yellow"] = "2.9* 4";
   $cat["red"]    = "- 2.9";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "visual_working_memory";   
   $cat["+1"]  = "- 1600*";
   $cat["0"]   = "1600 2400*";
   $cat["-1"]  = "2400 2550*";
   $cat["-2"]  = "2550 2700*";
   $cat["-3"]  = "2700 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "- 1600";
   $cat["green"]  = "1600* 2400";
   $cat["yellow"] = "2400* 2700";
   $cat["red"]    = "2700* +";
   $categories[$variable]["color"] = $cat;

   
   /////
   $cat = array();
   $variable = "correct_responses";   
   $cat["+1"]  = "82* +";
   $cat["0"]   = "60* 82";
   $cat["-1"]  = "55* 60";
   $cat["-2"]  = "50* 55";
   $cat["-3"]  = "- 49";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "81* +";
   $cat["green"]  = "60 81";
   $cat["yellow"] = "50 60*";
   $cat["red"]    = "- 50*";
   $categories[$variable]["color"] = $cat;


   /////
   $cat = array();
   $variable = "working_memory";   
   $cat["+1"]  = "76* +";
   $cat["0"]   = "58* 76";
   $cat["-1"]  = "49* 58";
   $cat["-2"]  = "45* 49";
   $cat["-3"]  = "- 45";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "76* +";
   $cat["green"]  = "58 76";
   $cat["yellow"] = "45 58*";
   $cat["red"]    = "- 45*";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "inhibitory_processes";   
   $cat["+1"]  = "- 2*";
   $cat["0"]   = "2 18*";
   $cat["-1"]  = "18 30*";
   $cat["-2"]  = "30 45*";
   $cat["-3"]  = "45 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "- 3*";
   $cat["green"]  = "3 18*";
   $cat["yellow"] = "18 45";
   $cat["red"]    = "45* +";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "decoding_efficiency";   
   $cat["+1"]  = "- 499*";
   $cat["0"]   = "499 650*";
   $cat["-1"]  = "650 800*";
   $cat["-2"]  = "800 990*";
   $cat["-3"]  = "990 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "- 500*";
   $cat["green"]  = "500 650*";
   $cat["yellow"] = "650 990";
   $cat["red"]    = "990* +";
   $categories[$variable]["color"] = $cat;

   /////
   $cat = array();
   $variable = "enconding_efficiency";   
   $cat["+1"]  = "- 259*";
   $cat["0"]   = "259 300*";
   $cat["-1"]  = "300 380*";
   $cat["-2"]  = "380 430*";
   $cat["-3"]  = "430 +";
   $categories[$variable]["polygon"] = $cat;

   $cat = array();
   $cat["blue"]   = "- 260*";
   $cat["green"]  = "260 300*";
   $cat["yellow"] = "300 430";
   $cat["red"]    = "430* +";
   $categories[$variable]["color"] = $cat;
   
   return $categories;
   
?>
