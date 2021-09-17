<?php


   class FixationContainer {

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
      public $start_index;
      public $end_index;
      public $pupil;
      public $blinks;

      private const TOLERANCE = 0.01;

      function __construct(){
         $this->valid = false;

      }

      function computeFixationFromPoints($points, $start_index){
         $this->valid = true;
         $this->x = 0;
         $this->y = 0;
         $n = count($points);

         $this->min_x = $points[0]["x"];
         $this->min_y = $points[0]["y"];
         $this->max_x = $points[0]["x"];
         $this->max_y = $points[0]["y"];

         $this->pupil = 0;
         $this->blinks = 0;

         $this->start_index = $start_index;
         $this->end_index = $start_index + ($n-1);

         foreach ($points as $p){
            
            $this->x = $this->x + $p["x"];
            $this->y = $this->y + $p["y"];
            $this->pupil = $this->pupil + $p["pupil"];

            if ($p["x"] < $this->min_x) $this->min_x = $p["x"];
            else if ($p["x"] > $this->max_x) $this->max_x = $p["x"];
            if ($p["y"] < $this->min_y) $this->min_y = $p["y"];
            else if ($p["y"] > $this->max_y) $this->max_y = $p["y"];

            if ($this->pupil < self::TOLERANCE){
               $this->blinks++;
            }

         }

         $this->x = $this->x/$n;
         $this->y = $this->y/$n;
         $this->pupil = $this->pupil/$n;
         $this->duration = $points[$n-1]["time"] - $points[0]["time"];
         $this->time = ($points[$n-1]["time"] + $points[0]["time"])/2;
         $this->start = $points[0]["time"];
         $this->end   = $points[$n-1]["time"];
         $this->points = $points;

      }

      function toStdJSON(){
         $nfix["duration"]    = $this->duration;
         $nfix["end_index"]   = $this->end_index;
         $nfix["end_time"]    = $this->end;
         $nfix["pupil"]       = $this->pupil;
         $nfix["start_index"] = $this->start_index;
         $nfix["start_time"]  = $this->start;
         $nfix["time"]        = $this->time;
         $nfix["x"]           = $this->x;
         $nfix["y"]           = $this->y;
         $nfix["zero_pupil"]  = $this->blinks;
         return $nfix;
      }

      function compareFixation(FixationContainer $f){
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
      private $index_counter = 0;
      private $current_start_index;

      function __construct($max_dispersion, $sample_frequency, $minimum_fixation_size)
      {
         $this->maxDispersion = $max_dispersion;
         $this->startWindowSize = ceil($minimum_fixation_size*$sample_frequency/1000);
         $this->onlinePointsForFixation = array();
         $this->onlineMMX = new OnlineMM();
         $this->onlineMMY = new OnlineMM();
         $this->index_counter = 0;
         $this->current_start_index = 0;
      }

      function computeFixationsOnline($x,$y,$time,$pupil = 0){

         $p["x"] = $x;
         $p["y"] = $y;
         $p["time"] = $time;    
         $p["pupil"] = $pupil;
         $this->onlinePointsForFixation[] = $p;
         $fixation = new FixationContainer();

         $this->index_counter = 0;

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
            $fixation->computeFixationFromPoints($this->onlinePointsForFixation,$this->current_start_index);
            $this->onlinePointsForFixation = array();
            $this->onlinePointsForFixation[] = $p;
            $this->current_start_index = $this->index_counter;

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

         $fixation = new FixationContainer();

         if (count($this->onlinePointsForFixation) >= $this->startWindowSize) {
            //if (($this->onlineMMX->diff() + $this->onlineMMY->diff()) > $this->maxDispersion) {
               //echo "Computing fix on finalization\n";
               $fixation->computeFixationFromPoints($this->onlinePointsForFixation,$this->current_start_index);
            //}
         }

         $this->onlinePointsForFixation = array();
         $this->current_start_index = 0;
         $this->index_counter = 0;

         return $fixation;
      }





   }

?>