<?php

include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TableRateLimiting.php");

class RateLimiter {

   private $dbcon;
   private $tableRateLimit;
   private $error;

   private const DEFAULT_RATE_LIMIT    = 60;    // Number of requests.
   private const RATE_LIMIT_TIME_FRAME = 20;    // Minutes
   
   function __construct(){

      $this->error = "";

      // Creating the connection.
      $dbcon = new DBCon();
      $this->dbcon = $dbcon->dbOpenConnection(DBCon::DB_MAIN,DBCon::DB_SERVICE_ADMIN);      
      if ($this->dbcon == NULL){
         $this->error = "Error creating db connection: " . $dbcon->getError();
         return;
      }

      // Creating the table instance. 
      $this->tableRateLimit = new TableRateLimiting($this->dbcon);

   }

   function getError(){
      return $this->error;
   }

   function allowRequest($ip){

      if ($this->error != "") return false;

      // First we get the information on the $ip.
      $ip_info = $this->tableRateLimit->getIPInfo($ip);
      if ($ip_info === false){
         $this->error = "Could not get information on $ip due to DB error: " . $this->tableRateLimit->getError();
         return false;
      }
      
      if (count($ip_info) === 0){
         // This means that this IP is the first time that creates a request, so it will be allowed.
         $ans = $this->tableRateLimit->newIP($ip,self::DEFAULT_RATE_LIMIT);
         if ($ans === false){
            $this->error = "Could not insert new $ip in rate limiting. Reason: " . $this->tableRateLimit->getError();
            return false;
         }
         return true;
      }

      // Getting the first row, which is the only record. 
      $ip_info = $ip_info[0];

      // Existing IP. We do the math. 
      $rate_limit = $ip_info[TableRateLimiting::COL_RATE_LIMIT];
      $start      = new DateTime($ip_info[TableRateLimiting::COL_TIME_FRAME_START]);
      $interval   = $start->diff(new DateTime());
      $requests   = $ip_info[TableRateLimiting::COL_REQUESTS];
      $total_minutes = $interval->days*24*60 + $interval->h*60 + $interval->i;
      //echo "Total number of minutes since interval start: $total_minutes\n";
      if ($total_minutes > self::RATE_LIMIT_TIME_FRAME){
         // A new time frame needs to start, so this is a valid request.
         $ans = $this->tableRateLimit->updateIPTimeFrame($ip);
         if ($ans === false){
            $this->error = "Failed updating $ip timeframe. Reason: " . $this->tableRateLimit->getError();
            return false;
         }
         return true;
      }
      else {
         // We need to make sure that the number of requests are not to high.
         $requests++;
         if ($requests > $rate_limit){
            // Since the rate limit is already exceeded there is no need to update.
            $this->error = "Rate limit exceeded. This is the $requests within the time frame of " . self::RATE_LIMIT_TIME_FRAME . " minutes";
            return false;
         }
         
         // Valid request. We must update. 
         $ans = $this->tableRateLimit->updateIPRequestCount($ip,$requests);
         if ($ans === false){
            $this->error = "Failed to update request count for $ip. Reason:  " . $this->tableRateLimit->getError();
            return false;
         }
      }

      return true;

   }

}
// DBCon::setPointerLocation("/home/ariel/repos/viewmind_projects/VMPortal/Backend/00mng/configs");
// $ip = "18.15.240.17";
// $rl = new RateLimiter();

// $ans = $rl->allowRequest($ip);
// if ($ans === false){
//    echo "Request not allowed. Reason: " . $rl->getError() . "\n";
// }
// else{
//    echo "Request allowed\n";
// }
?>