<?php

   class LogManager {
   
      private $logfile;
      private $runID;
      private $output_enable;
      private $source;
   
      function __construct($logFile){
         $this->logfile = $logFile;
          // Initializing RUN ID.
         $date = new DateTime();
         $this->runID = "RUN_". $date->format('Y_m_d_H_i_s');         
         $this->output_enable = false; // By Default Echo is disabled as this will run in a server. 
         $this->source = "";
      }   

      function setSource($source){
         $this->source = $source;
      }
      
      function getLogFile(){
         return $this->logfile;
      }

      function suppressOutput(){
         $this->output_enable = false;
      }

      function enableOutput(){
         $this->output_enable = true;
      }
            
      function logError($info){
         $this->logMessage("ERROR",$info);
      }
      
      function logProgress($info){ 
         $this->logMessage("",$info);
      }   
      
      function logWarning($info){
         $this->logMessage("WARNING",$info);
      }
      
      private function logMessage($type,$info){
         $date = new DateTime();
         $ts = $date->format('Y-m-d H:i:s');        

         // The message is complimented with the source, if available. 
         if ($this->source != ""){
            $info = $this->source . " -> ";
         }

         // The actual message is formed.
         $info = "[" . $this->runID  . "][" . $ts . "] " . $info . "\n";

         // Finally we add the type, if pressent.
         if ($type != "") $info = "$type : $info";     

         // Actually doing the logging. 
         if ($this->logfile != ""){
            $handle = fopen($this->logfile,"a");
            if ($handle){
               fwrite($handle,$info);
               fclose($handle);                           
            }
         }   
         if ($this->output_enable) echo $info;      
      }
      
   }
 
?>
