<?php

/***
 * The entire purpose of this script is simply to check if simply analyzing the raw data points, we can determine if the subject
 * Did the correct sequence of a given trial and how many times. 
 */

 ////////////////////////////////////// INPUTS //////////////////////////////////////
$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29.json";
//$input_file = "/home/ariel/repos/viewmind_projects/Scripts/php/analyze_singular_JSON/local_work_aranda_nbackrt/nbackrt_2021_10_07_09_29_interpolated.json";
$threshold_n_points = 3;

/////////////////////////////////////////////////////////////////////////////////////
function isPointIn($x,$y, $hitboxes){
   for ($i = 0; $i < count($hitboxes); $i++){
      $hitbox = $hitboxes[$i];
      if ( ($x > $hitbox[0]) && ( $x < ($hitbox[0] + $hitbox[2]) ) ) {
         if ( ($y > $hitbox[1]) && ( $y < ($hitbox[1] + $hitbox[3]) ) ) {
            return $i;
         }
      }
   }
   return -1;
}

/////////////////////////////////////////////////////////////////////////////////////
class OneShotInHitCounter {

   private $hit_counter;
   private $current_id;
   private $minimum_for_hit;
   private $hold;

   function __construct($tolerance){
      $this->hit_counter = 0;
      $this->current_id = -1;
      $this->minimum_for_hit = $tolerance;
      $this->hold = false;
   }

   function hitBox($id){

      //echo "      OneShotInHitCounter::hitBox enterig with $id. Status: CURRENT ID: " . $this->current_id . ". HCOUTER: " . $this->hit_counter . " HOLD: " . $this->hold . "\n";

      if ($id == -1){
         //$this->hit_counter = 0;
         //$this->current_id = -1;
         //$this->hold = false;
         return false;
      }
      
      if ($id == $this->current_id){
         if ($this->hold){
            return false;
         }
         else{
            $this->hit_counter++;
            if ($this->hit_counter >= $this->minimum_for_hit){
               $this->hold = true;
               return true; // This is an active hit on the id box. 
            }
         }
      }

      else{
         $this->hit_counter = 1;
         $this->current_id = $id;
         $this->hold = false;
         return false;
      }
   }

}

/////////////////////////////////////////////////////////////////////////////////////

$vmdata = json_decode(file_get_contents($input_file),true);

// Getting the hitboxes
$hitboxes = $vmdata["processing_parameters"]["nback_hitboxes"];

$trials_completed = array();
$total_trials = 0;

echo "ANALYZING FILE $input_file\n";
echo "USING THRESHOLD of $threshold_n_points\n";

// Iterating over the retrieval phase of each data set. 
foreach ($vmdata["studies"] as $study_name => $study_data){

   $x = "xr"; $y = "yr";
   if ($study_data["study_configuration"]["valid_eye"] == "left"){
      $x = "xl"; $y = "yl";
   }

   $total_trials = count($study_data["trial_list"]);

   foreach ($study_data["trial_list"] as $trial){      
      

      $trial_id = $trial["ID"];
      $trial_type = $trial["trial_type"];      

      echo "TRIAL_ID $trial_id\n";

      // Generating the correct sequence. 
      $order = explode(" ",$trial_type);
      $order = [ $order[2], $order[1], $order[0] ];
      $next_to_hit = 0;

      // Getting the retrieval dataset. 
      $points = $trial["data"]["retrieval_1"]["raw_data"];

      // This is to check when we can consider that we've hit a target box. 
      $checker = new OneShotInHitCounter($threshold_n_points);

      foreach ($points as $p){         

         $hitbox = isPointIn($p[$x],$p[$y],$hitboxes);
         if ($hitbox != -1){
            echo "   HIT: $hitbox\n";
         }
         if ($checker->hitBox($hitbox)){
            // At this point we consider that we have a hit in a hitbox. We check if it is the expected
            echo "   CHIT: $hitbox\n";
            if ($order[$next_to_hit] == $hitbox){
               $next_to_hit++;
               if ($next_to_hit == count($order)) {
                  $trials_completed[] = $trial_id;
                  break;
               }
            }
            else $next_to_hit = 0;
         }
      }

      
   }

}

$n = count($trials_completed);
$p = ($n*100/$total_trials);
echo "Trials Completed $n out of $total_trials ($p %)\n";
var_dump($trials_completed);


?>