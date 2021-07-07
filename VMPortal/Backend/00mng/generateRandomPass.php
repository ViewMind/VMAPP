<?php 

   $lowercase = array();
   $uppercase = array();
   $numbers   = [0,1,2,3,4,5,6,7,8,9];
   $symbols   = ["!","$","%","&","@","#","=","?","¿","_",":","+","*"];
   $password_size = 20;
   
   foreach (range('a', 'z') as $char){
      $lowercase[] = $char;
      $uppercase[] = strtoupper($char);
   }
   
   // In order number of lower case letters, number of uppercase letter number of numbers and number of symbols
   $pass_compostions =[ 5, 5, 6, 4 ];
   
   // First step take one of each.
   $pass_chars = array();   

   $i = $pass_compostions[0];
   while ($i > 0){
      $index = rand(0,count($lowercase)-1);
      $pass_chars[] = $lowercase[$index];
      $i--;
   }
   
   $i = $pass_compostions[1];
   while ($i > 0){
      $index = rand(0,count($uppercase)-1);
      $pass_chars[] = $uppercase[$index];
      $i--;
   }   
   
   $i = $pass_compostions[2];
   while ($i > 0){
      $index = rand(0,count($numbers)-1);
      $pass_chars[] = strval($numbers[$index]);
      $i--;
   }   
   
   $i = $pass_compostions[3];
   while ($i > 0){
      $index = rand(0,count($symbols)-1);
      $pass_chars[] = $symbols[$index];
      $i--;
   }   
   
   // Putting the password together.
   $passwd = "";
   $count = count($pass_chars);
   while ($count > 0){
     $index = rand(0,count($pass_chars)-1);
     $passwd = $passwd . $pass_chars[$index];
     //unset($pass_chars[$index]);
     array_splice($pass_chars, $index, 1);
     $count--;
   }
   
   echo "PASSWORD: $passwd\n";
   
?>
