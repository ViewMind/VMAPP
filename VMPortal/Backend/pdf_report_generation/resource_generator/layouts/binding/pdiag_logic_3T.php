<?php

   // This is the hieararchical order of the variables in order to access the pdiag object. 
   $variables = ["parahipocampal_region","exec_func","int_mem_perf","assoc_mem_perf"];

   $lookup = array();

   // Set 1 of 9
   $lookup["red"]["red"]["red"]["red"]                  = 2;
   $lookup["red"]["red"]["red"]["yellow"]               = 3;
   $lookup["red"]["red"]["red"]["green"]                = 3;
   
   $lookup["red"]["red"]["yellow"]["red"]               = 2;
   $lookup["red"]["red"]["yellow"]["yellow"]            = 2;
   $lookup["red"]["red"]["yellow"]["green"]             = 3;
   
   $lookup["red"]["red"]["green"]["red"]                = 2;
   $lookup["red"]["red"]["green"]["yellow"]             = 2;
   $lookup["red"]["red"]["green"]["green"]              = 3;
   
   // Set 2 of 9   
   $lookup["red"]["yellow"]["red"]["red"]               = 2;
   $lookup["red"]["yellow"]["red"]["yellow"]            = 3;
   $lookup["red"]["yellow"]["red"]["green"]             = 3;
   
   $lookup["red"]["yellow"]["yellow"]["red"]            = 2;
   $lookup["red"]["yellow"]["yellow"]["yellow"]         = 1;
   $lookup["red"]["yellow"]["yellow"]["green"]          = 3;
   
   $lookup["red"]["yellow"]["green"]["red"]             = 2;
   $lookup["red"]["yellow"]["green"]["yellow"]          = 7;
   $lookup["red"]["yellow"]["green"]["green"]           = 7;
   
   // Set 3 of 9   
   $lookup["red"]["green"]["red"]["red"]                = 2;
   $lookup["red"]["green"]["red"]["yellow"]             = 2;
   $lookup["red"]["green"]["red"]["green"]              = 5;
   
   $lookup["red"]["green"]["yellow"]["red"]             = 4;
   $lookup["red"]["green"]["yellow"]["yellow"]          = 7;
   $lookup["red"]["green"]["yellow"]["green"]           = 7;
   
   $lookup["red"]["green"]["green"]["red"]              = 4;
   $lookup["red"]["green"]["green"]["yellow"]           = 7;
   $lookup["red"]["green"]["green"]["green"]            = 7;


   // Set 4 of 9
   $lookup["yellow"]["red"]["red"]["red"]               = 2;
   $lookup["yellow"]["red"]["red"]["yellow"]            = 3;
   $lookup["yellow"]["red"]["red"]["green"]             = 3;

   $lookup["yellow"]["red"]["yellow"]["red"]            = 2;
   $lookup["yellow"]["red"]["yellow"]["yellow"]         = 3;
   $lookup["yellow"]["red"]["yellow"]["green"]          = 3;

   $lookup["yellow"]["red"]["green"]["red"]             = 2;
   $lookup["yellow"]["red"]["green"]["yellow"]          = 3;
   $lookup["yellow"]["red"]["green"]["green"]           = 3;

   // Set 5 of 9
   $lookup["yellow"]["yellow"]["red"]["red"]            = 2;
   $lookup["yellow"]["yellow"]["red"]["yellow"]         = 7;
   $lookup["yellow"]["yellow"]["red"]["green"]          = 7;

   $lookup["yellow"]["yellow"]["yellow"]["red"]         = 7;
   $lookup["yellow"]["yellow"]["yellow"]["yellow"]      = 6;
   $lookup["yellow"]["yellow"]["yellow"]["green"]       = 6;

   $lookup["yellow"]["yellow"]["green"]["red"]          = 7;
   $lookup["yellow"]["yellow"]["green"]["yellow"]       = 6;
   $lookup["yellow"]["yellow"]["green"]["green"]        = 6;

   // Set 6 of 9
   $lookup["yellow"]["green"]["red"]["red"]             = 5;
   $lookup["yellow"]["green"]["red"]["yellow"]          = 7;
   $lookup["yellow"]["green"]["red"]["green"]           = 7;

   $lookup["yellow"]["green"]["yellow"]["red"]          = 7;
   $lookup["yellow"]["green"]["yellow"]["yellow"]       = 6;
   $lookup["yellow"]["green"]["yellow"]["green"]        = 6;

   $lookup["yellow"]["green"]["green"]["red"]           = 7;
   $lookup["yellow"]["green"]["green"]["yellow"]        = 6;
   $lookup["yellow"]["green"]["green"]["green"]         = 6;   


   // Set 7 of 9
   $lookup["green"]["red"]["red"]["red"]                = 2;
   $lookup["green"]["red"]["red"]["yellow"]             = 3;
   $lookup["green"]["red"]["red"]["green"]              = 3;

   $lookup["green"]["red"]["yellow"]["red"]             = 2;
   $lookup["green"]["red"]["yellow"]["yellow"]          = 3;
   $lookup["green"]["red"]["yellow"]["green"]           = 3;

   $lookup["green"]["red"]["green"]["red"]              = 4;
   $lookup["green"]["red"]["green"]["yellow"]           = 4;
   $lookup["green"]["red"]["green"]["green"]            = 3;

   // Set 8 of 9 
   $lookup["green"]["yellow"]["red"]["red"]             = 2;
   $lookup["green"]["yellow"]["red"]["yellow"]          = 7;
   $lookup["green"]["yellow"]["red"]["green"]           = 7;

   $lookup["green"]["yellow"]["yellow"]["red"]          = 7;
   $lookup["green"]["yellow"]["yellow"]["yellow"]       = 6;
   $lookup["green"]["yellow"]["yellow"]["green"]        = 6;

   $lookup["green"]["yellow"]["green"]["red"]           = 7;
   $lookup["green"]["yellow"]["green"]["yellow"]        = 6;
   $lookup["green"]["yellow"]["green"]["green"]         = 6;

   // Set 9 of 9 
   $lookup["green"]["green"]["red"]["red"]              = 5;
   $lookup["green"]["green"]["red"]["yellow"]           = 7;
   $lookup["green"]["green"]["red"]["green"]            = 7;

   $lookup["green"]["green"]["yellow"]["red"]           = 7;
   $lookup["green"]["green"]["yellow"]["yellow"]        = 6;
   $lookup["green"]["green"]["yellow"]["green"]         = 6;

   $lookup["green"]["green"]["green"]["red"]            = 7;
   $lookup["green"]["green"]["green"]["yellow"]         = 6;
   $lookup["green"]["green"]["green"]["green"]          = 6;   
   
   $pdiag["variables"] = $variables;
   $pdiag["text_index"] = $lookup;
   return $pdiag;


?>