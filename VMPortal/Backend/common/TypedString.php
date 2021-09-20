<?php

   abstract class TypedString {
   
   static function validate($string) {
      $oClass = new ReflectionClass(static::class);
      $values = array_values($oClass->getConstants());
      if (in_array($string, $values)) return true;
      return false;
   }

   static function listMissingFields($field_list){
      $oClass = new ReflectionClass(static::class);
      $values = array_values($oClass->getConstants());
      $missing = array();
      foreach ($values as $v){
         if (!in_array($v,$field_list)) $missing[] = $v;
      }
      return $missing;
   }
   
   static function getConstValueList(){
      $oClass = new ReflectionClass(static::class);
      return implode(",", array_values($oClass->getConstants()));
   }

   static function getConstList($skip = array()){
      $oClass = new ReflectionClass(static::class);
      $associative_array = $oClass->getConstants();
      if (empty($skip)) return array_values($associative_array);
      $ret = array();
      foreach ($associative_array as $name => $value){
         if (!in_array($name,$skip)) $ret[] = $value;
      }
      return $ret;
   }
   
   }
?>