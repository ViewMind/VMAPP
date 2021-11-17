<?php

include_once ("TableBaseClass.php");

class TableRateLimiting extends TableBaseClass {
   
   const TABLE_NAME               = "rate_limiting";
   const IN_DB                    = DBCon::DB_MAIN;

   const COL_IP                   = "ip";
   const COL_RATE_LIMIT           = "rate_limit";
   const COL_REQUESTS             = "requests";
   const COL_TIME_FRAME_START     = "time_frame_start";

   function __construct(PDO $con){
      parent::__construct($con);
   }

   function getIPInfo($ip){
      $select = new SelectOperation();
      if (!$select->addConditionToANDList(SelectColumnComparison::EQUAL,self::COL_IP,$ip)){
         $this->error = static::class . "::" . __FUNCTION__ . " Failed form SELECT. Reason: " . $select->getError();
         return false;
      }
      return $this->simpleSelect(array(),$select);      
   }

   function newIP ($ip, $rate_limit){
      $params[self::COL_IP] = $ip;
      $params[self::COL_RATE_LIMIT] = $rate_limit;
      $params[self::COL_REQUESTS] = 1;
      return $this->insertionOperation($params,"Inserting $ip into Rate Limiting Table");
   }

   function updateIPTimeFrame($ip){

      // Since there is no base function that allows the update of a column to CURRENT_TIMESTAMP., this one is custom

      $sql = "UPDATE " . self::TABLE_NAME . " SET " 
      . self::COL_REQUESTS . " = 1, "  
      . self::COL_TIME_FRAME_START . " = CURRENT_TIMESTAMP() " 
      . " WHERE " . self::COL_IP . " = '$ip'";

      try {
         $time = microtime(true);
         $stmt = $this->con->prepare($sql);
         $stmt->execute([]);
         $this->time_for_last_query = (microtime(true) - $time)*1000;
         $this->last_query = $sql;
         $this->last_inserted[] = $this->con->lastInsertId();
      }
      catch (PDOException $e){
         $this->error = "Update failure on IP TimeFrame: " . $e->getMessage() . ". SQL: $sql";
         return false;
      }      

      return array();
   }

   function updateIPRequestCount($ip,$requests){
      $params[self::COL_REQUESTS] = $requests;
      return $this->updateOperation($params,"Updating requests for $ip to $requests",self::COL_IP,$ip);
   }

}

?>