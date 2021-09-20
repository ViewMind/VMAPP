<?php

function echoOut($var,$dbug = false){
   if ($dbug) {
      ob_start();
      var_dump($var);
      $text = ob_get_clean();
      echo "$text";
   }
   else{
      $out = json_encode($var);
      if (json_last_error() != JSON_ERROR_NONE){
         $var["error_message"] = "Failed to encode JSON output";
         echo json_encode($var);
      }
      else echo $out;
   }
}

function vardumpIntoVar($var){
   ob_start();
   var_dump($var);
   $text = ob_get_clean();
   return $text;   
}


?>