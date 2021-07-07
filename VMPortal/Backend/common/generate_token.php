<?php

   function generateToken($token_length){
      
      // Ensuring we deal with a number and not a string. 
      $token_length = intval($token_length);

      // Needs to be divisible by two.
      if (($token_length % 2) != 0){
         $token_length++;
      }

      $number_of_bytes = $token_length/2;
      if ($number_of_bytes == 0) return false;

      $ans = openssl_random_pseudo_bytes($number_of_bytes);
      return bin2hex($ans);

   }
   // $res = generateToken(0);
   // if ($res !== FALSE){
   //    echo "Token: $res\n";
   // }
   // else echo "Bad Value\n"

?>