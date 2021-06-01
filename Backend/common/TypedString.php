<?php
   abstract class TypedString {
   
   static function validate($string) {
      $oClass = new ReflectionClass(static::class);
      $values = array_values($oClass->getConstants());
      if (in_array($string, $values)) return true;
      return false;
   }
   
   static function getConstValueList(){
      $oClass = new ReflectionClass(static::class);
      return implode(",", array_values($oClass->getConstants()));
   }
   
   }
?>