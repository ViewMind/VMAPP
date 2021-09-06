<?php


   class Fixation {

      public $x;
      public $y;
      public $duration;
      public $start;
      public $end;
      public $time;
      public $valid;
      public $points;
      public $min_x;
      public $max_x;
      public $min_y;
      public $max_y;


      private const TOLERANCE = 0.01;

      function __construct(){
         $this->valid = false;

      }

      function computeFixationFromPoints($points){
         $this->valid = true;
         $this->x = 0;
         $this->y = 0;
         $n = count($points);

         $this->min_x = $points[0]["x"];
         $this->min_y = $points[0]["y"];
         $this->max_x = $points[0]["x"];
         $this->max_y = $points[0]["y"];


         foreach ($points as $p){
            $this->x = $this->x + $p["x"];
            $this->y = $this->y + $p["y"];
            
            if ($p["x"] < $this->min_x) $this->min_x = $p["x"];
            else if ($p["x"] > $this->max_x) $this->max_x = $p["x"];
            if ($p["y"] < $this->min_y) $this->min_y = $p["y"];
            else if ($p["y"] > $this->max_y) $this->max_y = $p["y"];

         }

         $this->x = $this->x/$n;
         $this->y = $this->y/$n;
         $this->duration = $points[$n-1]["time"] - $points[0]["time"];
         $this->time = ($points[$n-1]["time"] + $points[0]["time"])/2;
         $this->start = $points[0]["time"];
         $this->end   = $points[$n-1]["time"];
         $this->points = $points;

      }

      function compareFixation(Fixation $f){
         $error = "";
         $duration = $f->duration;
         $x = $f->x;
         $y = $f->y; 
         //echo "$x, $y, $duration\n";
         //var_dump($f);
         if (abs($duration - $this->duration) > self::TOLERANCE){
            $error =  "WRONG DURATION: " . $this->duration . " REF: $duration. ";
         }
         if (abs($x - $this->x) > self::TOLERANCE){
            $error =  $error . "WRONG X: " . $this->x . " REF: $x. ";
         }
         if (abs($y - $this->y) > self::TOLERANCE){
            $error =  $error . "WRONG Y: " . $this->y . " REF: $y .";
         }
         //return "STOP";
         return $error;
      }

   }

   class OnlineMM {
      
      public $min;
      public $max;

      function __construct(){
         $this->min = 0;
         $this->max = 0;
      }

      function diff(){
         return $this->max - $this->min;
      }

      function setVal($v){
         $this->min = $v;
         $this->max = $v;
      }

      function setNewMaxMin($v){
         if ($v < $this->min) $this->min = $v;
         if ($v > $this->max) $this->max = $v;
      }

      function print($name){
         return "min_$name: " . $this->min . " max_$name: " . $this->max;
      }

   }

   class OnlineMovingWindowAlgorithm {


      private $startWindowSize;
      private $onlinePointsForFixation;
      private $maxDispersion;
      private $onlineMMX;
      private $onlineMMY;

      function __construct($max_dispersion, $sample_frequency, $minimum_fixation_size)
      {
         $this->maxDispersion = $max_dispersion;
         $this->startWindowSize = ceil($minimum_fixation_size*$sample_frequency/1000);
         $this->onlinePointsForFixation = array();
         $this->onlineMMX = new OnlineMM();
         $this->onlineMMY = new OnlineMM();

      }

      function computeFixationsOnline($x,$y,$time){

         $p["x"] = $x;
         $p["y"] = $y;
         $p["time"] = $time;         
         $this->onlinePointsForFixation[] = $p;
         $fixation = new Fixation();

         //$this->onlineMMX->diff() + $this->onlineMMY->diff()

         if (count($this->onlinePointsForFixation)  < $this->startWindowSize) {
            //echo "x: $x y: $y " . $this->onlineMMX->print("x") . " " . $this->onlineMMY->print("y") . " x";
            return $fixation;
         }

         if (count($this->onlinePointsForFixation)  == $this->startWindowSize) {

            $this->onlineFindDispLimits();

            if (($this->onlineMMX->diff() + $this->onlineMMY->diff()) > $this->maxDispersion) {
               array_shift($this->onlinePointsForFixation);               
            }

            return $fixation;

         }

         $this->onlineMMX->setNewMaxMin($x);
         $this->onlineMMY->setNewMaxMin($y);

         if (($this->onlineMMX->diff() + $this->onlineMMY->diff()) > $this->maxDispersion) {

            array_pop($this->onlinePointsForFixation);
            $fixation->computeFixationFromPoints($this->onlinePointsForFixation);
            $this->onlinePointsForFixation = array();
            $this->onlinePointsForFixation[] = $p;

         }

         return $fixation;

      }

      function onlineFindDispLimits(){

         $first = $this->onlinePointsForFixation[0];
         $this->onlineMMY->setVal($first["y"]);
         $this->onlineMMX->setVal($first["x"]);

         for ($i = 0; $i < count($this->onlinePointsForFixation); $i++){
            $p = $this->onlinePointsForFixation[$i];
            $this->onlineMMY->setNewMaxMin($p["y"]);
            $this->onlineMMX->setNewMaxMin($p["x"]);
   
         }


      }

      function traceString($trial_counter = ""){
         $n  = count($this->onlinePointsForFixation);
         if ($n == 0) return;

         $p = $this->onlinePointsForFixation[$n-1];
         $toprint["TRIAL"] = $trial_counter;
         $toprint["TIME"] = $p["time"];;
         $toprint["X"] = $p["x"];
         $toprint["Y"] = $p["y"];
         $toprint["LIST SIZE"] = $n;
         $toprint["MIN X"] = $this->onlineMMX->min;
         $toprint["MAX X"] = $this->onlineMMX->max;
         $toprint["MIN Y"] = $this->onlineMMY->min;
         $toprint["MAX Y"] = $this->onlineMMY->max;
         $toprint["DISP X"] = $this->onlineMMX->diff();
         $toprint["DISP Y"] = $this->onlineMMY->diff();
         $toprint["DISP"] = $this->onlineMMX->diff() + $this->onlineMMY->diff();
         $toprint["MAX DISP"] = $this->maxDispersion;

         $rep = "";
         foreach ($toprint as $name => $value){
            $rep = $rep . "$name: $value. ";
         }

         return $rep;

      }

      function finalizeComputation(){

         $fixation = new Fixation();

         if (count($this->onlinePointsForFixation) >= $this->startWindowSize) {
            //if (($this->onlineMMX->diff() + $this->onlineMMY->diff()) > $this->maxDispersion) {
               //echo "Computing fix on finalization\n";
               $fixation->computeFixationFromPoints($this->onlinePointsForFixation);
            //}
         }

         $this->onlinePointsForFixation = array();

         return $fixation;
      }





   }

?>