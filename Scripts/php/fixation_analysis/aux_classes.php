<?php

class MinMax {

   public $min;
   public $max;

   function reset(){
      $this->min = 10000;
      $this->max = 0;
   }
   
   function value($v){
      if ($this->min > $v) $this->min = $v;
      else if ($this->max < $v) $this->max = $v;
   }

}

class PointMinMax {

   public MinMax $x;
   public MinMax $y;

   function __construct(){
      $this->x = new MinMax();
      $this->y = new MinMax();      
   }

   function reset(){
      $this->x->reset();
      $this->y->reset();
   }

   function point($x,$y){
      $this->x->value($x);
      $this->y->value($y);
   }

}

/***
 * Computes the merit figures for two vectors one representing a fixation distribution and the otherone a reference fixation distribution
 * It ensures that the vectors are the same size, first and the then does the sum square fo the difference. 
 * Retunrs the nomralized vectors and the the merit figure. 
 */

function meritComputation($values, $references, $merit_figure_to_use,$maximum_bin){
   $ret["merit"] = 0;
   $ret["ref"] = array();
   $ret["values"] = array();

   $N = count($values);
   $M = count($references);
   $S = 0;

   if ($N > $M){
      for ($i = 0; $i < ($N-$M); $i++){
         $references[] = 0;
      }
      $S = $N;
   }
   else if ($M > $N){
      for ($i = 0; $i < ($M-$N); $i++){
         $values[] = 0;
      }
      $S = $M;
   }
   // If they are both the same size, then that size of any of them will do. 
   else $S = $M;

   // Since the bin is a fixation duration range, doing this is equivalent to not contemplating fixations above a certain threshold. 
   if ($S > $maximum_bin) $S = $maximum_bin;

   for ($i = 0; $i < $S; $i++){
      if ($merit_figure_to_use == "RMS") {
         $ret["merit"]= $ret["merit"] + pow($references[$i] - $values[$i],2);
      }
      else if ($merit_figure_to_use == "SUMABS"){
         $ret["merit"]= $ret["merit"] + abs($references[$i] - $values[$i]);
      }
   }
   if ($merit_figure_to_use == "RMS") $ret["merit"] =  sqrt($ret["merit"]/$S);

   $ret["ref"] = $references;
   $ret["values"] = $values;

   return $ret;
}
?>