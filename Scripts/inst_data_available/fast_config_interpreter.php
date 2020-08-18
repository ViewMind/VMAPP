<?php

   class FastConfigInterpreter {

      private $data;
      private $filename;
   
      function __construct($file){
         if (!is_file($file)) $this->data =  array();
         $this->filename = $file;
         $contents = file_get_contents($file);
         $lines = explode("\n",$contents);
         foreach ($lines as $line){
            if ($line == "") continue;            
            $line = str_replace(";","",$line);
            $parts = explode("=",$line);
            $key  = trim($parts[0]);
            $value = trim($parts[1]);
            if (strpos($value,"|") !== false){
               $this->data[$key] = explode("|",$value);
            }
            else $this->data[$key] = $value;
         }
         //var_dump($this->data);
      }
      
      function removeField($field){
         if (array_key_exists($field,$this->data)){
            unset($this->data[$field]);
         }
      }
      
      function getValueForKey($key){
         if (array_key_exists($key,$this->data)){
            return $this->data[$key];
         }
      }
      
      function leaveOnlyKeys($key_list){
         $existing_keys = array_keys($this->data);
         for ($ek = 0; $ek < count($existing_keys); $ek++){
            $key = $existing_keys[$ek];
            if (!in_array($key, $key_list)){
               unset($this->data[$key]);
            }
         }
      }
      
      function replaceKeys($key_map){
         $keys = array_keys($this->data);
         for ($ek = 0; $ek < count($keys); $ek++){
            $key = $keys[$ek];
            if (!array_key_exists($key,$key_map)) continue;            
            $to  = $key_map[$key];
            if (array_key_exists($key,$this->data)){
               $this->data[$to] = $this->data[$key];
               unset($this->data[$key]);
            }
         }
      }
      
      function saveToHDD(){
         $fid = fopen($this->filename,"w");
         foreach($this->data as $key => $value){
            if (is_array($value)){
               fwrite($fid, "$key = " . implode("|",$value) . ";\n");
            }
            else fwrite($fid, "$key = $value;\n");
         }
         fclose($fid);
      }
   
   }
?> 
