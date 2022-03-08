<?php

class RouteParser {

   private $error;
   private $route_parts;
   private $parameters;
   private $endpoints_and_parameters;

   function __construct(){
      $this->route_parts = array();
      $this->parameters = array();
      $this->error = "";
   }
   
   function tokenizeURL($path,$endpoints_tokens_to_skip){

      //error_log("Path to tokenize " . $path);
   
      $route_and_parameters = explode("?",$path);
      if (count($route_and_parameters) > 2){
         $this->error = "Invalid URL. Found more than one ? in the URL path";
         return false;
      }
      
      $route = $route_and_parameters[0];

      $parameters = "";
      if (count($route_and_parameters) == 2) {
         // Route and Parameters         
         $parameters = $route_and_parameters[1];         
      }

      // Splitting the route. 
      $temp = explode("/",$route);
      $temp2 = array();
      // Removing skippable tokens from the beginning of the route. 
      while ((count($temp) > 0) && ($endpoints_tokens_to_skip > 0)){
         array_shift($temp);
         $endpoints_tokens_to_skip--;
      }
      $this->route_parts = $temp;

      $this->endpoints_and_parameters = implode("/",$temp);
      
      // Parsing the parameters if they exist.
      if ($parameters != ""){
         $this->endpoints_and_parameters = $this->endpoints_and_parameters . "?" . $parameters;
         $parameters = explode("&",$parameters);
         foreach($parameters as $p){
            $name_and_value = explode("=",$p);
            $n = count($name_and_value);
            if ($n == 1){
               // This particular case is considered as enabling a flag. So it is interpreted as name=true
               $this->parameters[$name_and_value[0]] = true;
            }
            else if ($n == 2){
               // Standard name=value
               $this->parameters[$name_and_value[0]] = $name_and_value[1];
            }
            else{
               $this->error = "Malformed parameter $p contains more than one =";
               return false;
            }
         }
      }
      
      return true;
   
   }  

   function getRouteParts(){
      return $this->route_parts;
   }
   
   function getEndpointAndParameters(){
      return $this->endpoints_and_parameters;
   }

   function getParameters(){
      return $this->parameters;
   }

   function getError(){
      return $this->error;
   }

}

?>