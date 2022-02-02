<?php

$files = [
"/home/ariel/repos/viewmind_projects/QtApplications/CommonClasses/Experiments/binding/descriptions/bc_3.dat",
"/home/ariel/repos/viewmind_projects/QtApplications/CommonClasses/Experiments/binding/descriptions/bc.dat",
"/home/ariel/repos/viewmind_projects/QtApplications/CommonClasses/Experiments/binding/descriptions/uc_3.dat",
"/home/ariel/repos/viewmind_projects/QtApplications/CommonClasses/Experiments/binding/descriptions/uc.dat"];


$colors = array();

foreach ($files as $file){
   $lines = explode("\n",file_get_contents($file));
   
   // removing the first line.
   array_shift($lines);
   
   $state = 0;
   
   foreach ($lines as $line){
   
      //echo "$state: $line\n";
   
      if (in_array($state,[2,3,5,6])){
         $colors_line = explode(" ",$line);
         foreach ($colors_line as $color){
            if ($color == "") continue;
            $colors[] = $color;
         }
      }
   
      $state++;
      $state = $state % 7;
      
   }
}

$colors = array_unique($colors);
var_dump($colors);

?>