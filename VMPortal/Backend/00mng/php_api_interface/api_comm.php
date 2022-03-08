<?php

include_once("../../common/named_constants.php");

class APIComm {

   private $url;
   private $token;
   private $user;
   private $error;
   private $login_info;
   private $login_type;
   private $last_curl_command;

   function __construct($url){
      $this->url = $url;
      $this->error = "";
   }

   function getError(){
      return $this->error;
   }

   function printWelcome(){
      echo "CONNECTED VIA API: " . $this->url . "\n";
      echo json_encode($this->login_info,JSON_PRETTY_PRINT);
      echo "\n";
   }

   function getLastCurlCommand(){
      return $this->last_curl_command;
   }

   function login($user,$password,$site){

      $headers = array();
      $headers[] = '-H "AuthType: Login"';  // Instructs the API to do a Login operation which will generate the bearer token.
      $headers[] = '-H "LoginType: ' . $site  . '"'; // The site to which we are login. 
      $auth = "$user:$password";         // Login is done using standard Basic Auth. 
      $header_cmd = implode(" \\\n ",$headers);

      // Save for later use. 
      $this->user = $user;
      $this->login_type = $site;

      $curl_cmd = "curl -s -X POST \\\n -u $auth \\\n $header_cmd \\\n " . $this->url; 
      $this->last_curl_command = $curl_cmd;

      $output = shell_exec($curl_cmd);
      $token_data = json_decode($output,true);

      if ($token_data == null){
         $this->error = "Curl Command Failed";
         var_dump($output);
         return false;
      }
      if ($token_data["http_code"] != 200){
         $this->error =  "Error getting login token: " . $token_data["message"] . "\n";
         return false;
      }

      $this->token = $token_data["data"]["token"];      
      $this->login_info = $token_data["data"];
      unset($this->login_info["token"]);
      return true;

   }

   function APICall($url,$body = array()){

      $headers = array();
      $headers[] = '-H "AuthType: VMPartner"';
      $headers[] = '-H "Authorization: ' . $this->user . ":" . $this->token . '"';
      $headers[] = '-H "LoginType: ' . $this->login_type  . '"'; // The site to which we are login. 
      $header_str = implode(" ", $headers);

      $final_url = $this->url . $url;

      if (!empty($body)) {
         $curl_cmd = "curl -s -X POST \\\n $header_str \\\n -d '" . json_encode($body) .  "' \\\n" . "'" . $final_url . "'";;
      }
      else {
         $curl_cmd = "curl -s -X POST \\\n $header_str \\\n " . "'" . $final_url . "'";
      }
      
      $this->last_curl_command = $curl_cmd;

      $output = shell_exec($curl_cmd);
      $return_data = json_decode($output,true);

      if ($return_data == null){
         $this->error = "Curl API Call Failed";
         var_dump($output);
         return false;
      }

      if ($return_data["http_code"] != 200){
         $this->error =  "API Call Failed. Reason: " . $return_data["message"] . "\n";
         return false;
      }


      return $return_data;

   }

}

function finishAndDie(APIComm $api){
   echo "Error. Reason: " . $api->getError() . "\n";
   echo "LAST CMD:\n";
   echo $api->getLastCurlCommand();
   echo "\n";
   exit();
}

?>