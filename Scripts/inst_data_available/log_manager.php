<?php

   class LogManager {
   
      private $logfile;
      private $runID;
   
      function __construct($logFile){
         $this->logfile = $logFile;
          // Initializing RUN ID.
         $date = new DateTime();
         $this->runID = "RUN_". $date->format('Y_m_d_H_i_s');         
      }   
            
      function logError($info){
         $date = new DateTime();
         $ts = $date->format('Y-m-d H:i:s');        
         $info = "ERROR: [" . $this->runID  . "][" . $ts . "] " . $info . "\n";     
         if ($this->logfile != ""){
            $handle = fopen($this->logfile,"a");
            if ($handle){
               fwrite($handle,$info);
               fclose($handle);                           
            }
         }    
         echo $info;
      }
      
      function logProgress($info){ 
         // Getting the time stamp. 
         $date = new DateTime();
         $ts = $date->format('Y-m-d H:i:s');
         $info = "[" . $this->runID  . "][" . $ts . "] " . $info .  "\n";      
         if ($this->logfile != ""){
            $handle = fopen($this->logfile,"a");
            if ($handle){
               fwrite($handle,$info);
               fclose($handle);                           
            }
         }          
         echo $info;
      }   
      
   }
 
?>
