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

   static function getConstList(){
      $oClass = new ReflectionClass(static::class);
      return array_values($oClass->getConstants());
   }
   
   }
?>