<?php

class ValueCategory {

   // const COLOR_RED    = "red";
   // const COLOR_BLUE   = "blue";
   // const COLOR_GREEN  = "green";
   // const COLOR_YELLOW = "yellow";

   /**
    * To construct a value category the construct string must be in the following form
    * valueA valueB.
    * valueA MUST be <= valueB and they must both be numbers
    * If an * precedes either value the value boundary itself is part of the range. 
    * - represents no lower bound while + represents no hight bound.       
    */

   private $label;
   private $category;
   private $error;

   function __construct($construct_string,$label){

      $this->error = "";

      $parts = explode(" ",$construct_string);
      if (count($parts) != 2){
         $this->error = "Badly formatted category string, incorrect part count: |$construct_string|";
         return;
      }

      $this->category = array();
      
      // Parsing the string. If the value starts with * it implies the bound falls within the range. And the only valid non numeric value is either -inf for the first
      // or inf for the second. 
      $low = strtolower($parts[0]);
      $low_first = substr($low,0,1);
      if ($low_first == "*"){
         $low = substr($low,1);
      }

      $high = strtolower($parts[1]);
      $high_first = substr($high,0,1);
      if ($high_first == "*"){
         $high = substr($high,1);
      }

      if ($low != "-") {
         if (!is_numeric($low)) {
             $this->error = "Bad low bound. Not a number: |$low|";
             return;
         }
      }

      if ($high != "+") {
         if (!is_numeric($high)) {
             $this->error = "Bad high bound. Not a number: |$high|";
             return;
         }
      }


      if (($low != "-") && ($high != "+")){
         if ($low > $high){
            $this->error = "Bad range $low > $high";
            return;
         }
      }

      // Saving the category object and the lable. 
      $this->category["min"] = $low;
      $this->category["max"] = $high;
      $this->category["min_inc"] = ($low_first == "*");
      $this->category["max_inc"] = ($high_first == "*");
      $this->label = $label;

   }

   function valueInCategory($value){
      
      if ($this->category["min"] != "-"){
         if ($this->category["min_inc"]){
            if ($value < $this->category["min"]) return false;
         }
         else{
            // If the min boundary is NOT included, then if $value is equal, then the value is NOT in the category.
            if ($value <= $this->category["min"]) return false;
         }
      }

      // Lower bound checks. Now we check for the upper bound. 
      if ($this->category["max"] != "+"){
         if ($this->category["max_inc"]){
            if ($value > $this->category["max"]) return false;
         }
         else{
            // If the min boundary is NOT included, then if $value is equal, then the value is NOT in the category.
            if ($value >= $this->category["max"]) return false;
         }
      }      
      
      
      return true;

   }

   function getError(){
      return $this->error;
   }

   function getLabel(){
      return $this->label;
   }
}

/**
 * It is essentialy a container for an Array of Value Category.
 * The constructor takes an object that is a map of the form 'label' => 'construction_string'.
 */
class SegmentClassifier {

   private $categories;
   private $errors;

   function __construct($categories_and_ranges){

      $this->errors = array();

      foreach ($categories_and_ranges as $label => $range){

         $c = new ValueCategory($range,$label);
         if ($c->getError() != ""){
            $this->errors[] = $c->getError();
         }

         $this->categories[] = $c;

      }
      
   }

   /**
    * Runs the value and returs the label of the matching range.
    * If no match is found it will return an empty string.
    * If there are errors it will also return an empty string. 
    */

   function categorize($value){
      if (!empty($this->errors)) return "";
      foreach ($this->categories as $category){
         if ($category->valueInCategory($value)){
            return $category->getLabel();
         }
      }
      return "";
   }

   function getErrors(){
      return $this->errors;
   }

}


// ///// TEST CODE
// $cats = [
//     1 => "- *2",
//     0 => "2 *3.5",
//    -1 => "3.5 *5",
//    -2 => "5 *7",
//    -3 => "7 +",
// ];

// $segment = new SegmentClassifier($cats);
// if (!empty($segment->getErrors())){
//    var_dump($segment->getErrors());
// }

// echo "Category: " . $segment->categorize(2)  . "\n"


?> 
