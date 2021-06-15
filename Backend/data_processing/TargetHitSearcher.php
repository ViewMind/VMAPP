<?php

include_once ("DataContainerNames.php");

class TargetHitSearcher {

   const TARGET_HIT_LOGIC_RT = "RT";
   const TARGET_HIT_LOGIC_MS = "MS";

   // This is the maximun and minimum sequence length of targets in a NBack type trial
   const MAX_SEQUENCE_LENGTH = 6;
   const MIN_SEQUENCE_LENGTH = 3;

   const NUMBER_OF_TARGET_BOXES = 6;

   private const RET_TARGET_HIT      = "target_hit";
   private const RET_IS_IN           = "is_in";
   private const RET_SEQ_COMPLETED   = "sequence_completed";
   private const RET_NBACK           = "nback";
   private const RET_ERROR           = "error";

   const COMPUTE_RET_ERROR           = "error";
   const COMPUTE_RET_SEQ_COMPLETE    = "seq_complete";   

   private $target_hit_logic;
   private $hit_boxes;
   private $trial_sequence;
   private $expected_target_index_in_sequence;
   private $all_MS_target_hits;

   // A 4 value vector represents a rectangle in the following manner:
   private const I_X = 0;
   private const I_Y = 1;
   private const I_W = 2;
   private const I_H = 3;

   function __construct(){
      $this->reset();
   }

   function setTargetBoxes($hboxes){
      $this->trial_sequence = array();
      if (count($hboxes) != self::NUMBER_OF_TARGET_BOXES){
         return "Number of expected target boxes is " . self::NUMBER_OF_TARGET_BOXES . " but " . count($hboxes) . " were passed";
      }
      foreach ($hboxes as $hbox){
         if (count($hbox) != 4){
            ob_start();
            var_dump($hbox);
            $print = ob_get_clean();
            return "Bad target box format for " . $print;
         }
         $this->hit_boxes[] = $hbox;
      }
      return "";
   }

   function setTargetLogic($logic){
      if (($logic == self::TARGET_HIT_LOGIC_MS) || ($logic == self::TARGET_HIT_LOGIC_RT) ){
         $this->target_hit_logic = $logic;
         return "";
      }
      return "Unknown target hit logic $logic\n";
   }

   function setNewTrial($trial_sequence){
      $this->trial_sequence = array();
      $temp = explode(" ",$trial_sequence);
      $this->trial_sequence = array();
      foreach($temp as $t){
         //checking that it's a number between 0 and 5
         $t = trim($t);
         if ($t == "") continue;
         $t = intval($t);
         if (($t >= 0) && ($t <= (self::NUMBER_OF_TARGET_BOXES-1))){
            $this->trial_sequence[] = $t;
         }
      }

      if (count($this->trial_sequence) < self::MIN_SEQUENCE_LENGTH){
         return "$trial_sequence is too short, it only has " . count($this->trial_sequence) ;
      }
      if (count($this->trial_sequence) > self::MAX_SEQUENCE_LENGTH){
         return "$trial_sequence is too long, it only has " . count($this->trial_sequence);
      }
      return "";
   }

   function reset(){
      // Ignored in the RT logic. ONly used for the MS logic.
      $this->all_MS_target_hits = array();
      // This will be ignored in the MS logic. Only used for RT logic.
      // TODO For variable number of targets, the intialization needs to be adjusted to NUMBER of targets - 1.
      $this->expected_target_index_in_sequence = 2;
   }

   function dbugState($tab = ""){
      echo "$tab Expected Target In Sequence: " . $this->expected_target_index_in_sequence . "\n";
      echo "$tab Trial Sequence: " . implode("->",$this->trial_sequence) . "\n";
      echo "$tab MSHits: " . count($this->all_MS_target_hits) . "\n";
   }

   function isHit($x, $y, $data_set_type){

      $ans[self::RET_IS_IN] = 0;
      $ans[self::RET_SEQ_COMPLETED] = 0;
      $ans[self::RET_TARGET_HIT] = "N/A";
      $ans[self::RET_NBACK] = "N/A";
      $ans[self::RET_ERROR] = "";

      // Finding which target, if any, was hit by the fixation.
      for ($i = 0; $i < count($this->hit_boxes); $i++){
         $result = $this->isInHitBox($i,$x,$y);
         if ($result[self::RET_ERROR] != ""){
            $ans[self::RET_ERROR] = $result[self::RET_ERROR];
            return $ans;
         }
         if ($result[self::RET_IS_IN]){
            $ans[self::RET_TARGET_HIT] = $i; 
         }
      }      

      // The calling the specific logic for each study. 
      if ($this->target_hit_logic == self::TARGET_HIT_LOGIC_MS){
         return $this->isHitMS($data_set_type,$ans);
      }
      else{
         return $this->isHitRT($data_set_type,$ans);
      }
   }

   private function isHitRT($data_set_type,$ans){

      switch ($data_set_type){
         case DataSetType::ENCODING_1:
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[0]) $ans[self::RET_IS_IN] = 1;
            break;
         case DataSetType::ENCODING_2:
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[1]) $ans[self::RET_IS_IN] = 1;
            break;
         case DataSetType::ENCODING_3:
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[2]) $ans[self::RET_IS_IN] = 1;
            break;
         case DataSetType::RETRIEVAL_1:
            // We need to detect if the fixation sequence, here is correct.
            // The expected target is the allwasy the last in the sequence.            
            if ($this->expected_target_index_in_sequence > -1){
               $ans[self::RET_IS_IN] = 0;
               if ($this->trial_sequence[$this->expected_target_index_in_sequence] == $ans[self::RET_TARGET_HIT]){
                  $ans[self::RET_NBACK] = $this->expected_target_index_in_sequence+1;
                  $this->expected_target_index_in_sequence--;
                  $ans[self::RET_IS_IN] = 1;
                  if ($this->expected_target_index_in_sequence == -1){
                     $ans[self::RET_SEQ_COMPLETED] = 1;
                  }
               }
            }
            break;
      }

      return $ans;

   }

   private function isHitMS($data_set_type,$ans){

      switch ($data_set_type){
         case DataSetType::ENCODING_1:
            // These are the images that show the red dots.
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[0]) $ans[self::RET_IS_IN] = 1;
            $this->expected_target_index_in_sequence = -1;
            break;
         case DataSetType::ENCODING_2:
            // These are the images that show the red dots.
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[1]) $ans[self::RET_IS_IN] = 1;
            $this->expected_target_index_in_sequence = -1;
            break;
         case DataSetType::ENCODING_3:
            // These are the images that show the red dots.
            if ($ans[self::RET_TARGET_HIT] == $this->trial_sequence[2]) $ans[self::RET_IS_IN] = 1;
            $this->expected_target_index_in_sequence = -1;
            break;
         case DataSetType::RETRIEVAL_1:
            // The target should hit the last target
            $this->expected_target_index_in_sequence = 2;
            break;
         case DataSetType::RETRIEVAL_2:
            // This should hit the second target.
            $this->expected_target_index_in_sequence = 1;
            break;
         case DataSetType::RETRIEVAL_3:
            // This should hit the first target.
            $this->expected_target_index_in_sequence = 0;
            break;
      }

      // If we are in the recognition  part of the trial.
      if ($this->expected_target_index_in_sequence > -1){
         // We check that the target hit corresponds to the correct target hit.
         if ($this->trial_sequence[$this->expected_target_index_in_sequence] == $ans[self::RET_TARGET_HIT]){
            $ans[self::RET_NBACK] = $this->expected_target_index_in_sequence+1;
            $ans[self::RET_IS_IN] = 1;
            $this->all_MS_target_hits[] = true;
         }
      }

      if ($data_set_type == DataSetType::RETRIEVAL_3){
         if (count($this->all_MS_target_hits)){
            // This means all targets were hit and in ther right sequence so we can consider the sequence complete.
            $ans[self::RET_SEQ_COMPLETED] = 1;
         }
      }

      return $ans;

   }

   private function isInHitBox($hit_box_index, $x, $y){

      $ans[self::RET_ERROR] = "";
      $ans[self::RET_IS_IN] = false;

      if (($hit_box_index < 0) || ($hit_box_index >= count($this->hit_boxes))){
         $ans[self::RET_ERROR] = "The hit box index $hit_box_index is out of bounds";
         return $ans;
      }

      $hitbox = $this->hit_boxes[$hit_box_index];
      $ans[self::RET_IS_IN] = TargetHitSearcher::isHitInBox($hitbox,$x,$y);

      return $ans;
   }

   static function isHitInBox($hitbox, $x, $y){
      if (($x >= $hitbox[self::I_X]) && ($x <= ($hitbox[self::I_X] + $hitbox[self::I_W]))){
         if (($y >= $hitbox[self::I_Y]) && ($y <= ($hitbox[self::I_Y] + $hitbox[self::I_H]))){
            return true;
         }
      }
      return false;
   }

   static function findFirstFixationThatHits(&$hitbox, &$fixation_list){
      for ($i = 0; $i < count($fixation_list); $i++){
         if (TargetHitSearcher::isHitInBox($hitbox,$fixation_list[$i][FixationVectorField::X],$fixation_list[$i][FixationVectorField::Y])){
            return $i;
         }
      }
      return -1;
   }

   static function computeFixationBasedNBackValues(&$trial, $hit_boxes , $trial_sequence, $study){

      $ans[self::COMPUTE_RET_ERROR]    = "";
      $fix_list_names = [DataSetField::FIXATIONS_L, DataSetField::FIXATIONS_R];
      $ans[DataSetField::FIXATIONS_L][self::COMPUTE_RET_SEQ_COMPLETE] = -1;
      $ans[DataSetField::FIXATIONS_R][self::COMPUTE_RET_SEQ_COMPLETE] = -1;
   
      $ths = new TargetHitSearcher();
      $data_set_order = array();
   
      if ($study == Study::NBACKMS){
         $data_set_order = [DataSetType::ENCODING_1,DataSetType::ENCODING_2,DataSetType::ENCODING_3,DataSetType::RETRIEVAL_1,DataSetType::RETRIEVAL_2,DataSetType::RETRIEVAL_3];
         $ths->setTargetLogic(TargetHitSearcher::TARGET_HIT_LOGIC_MS);
      }
      else if ($study == Study::NBACKRT){
         $data_set_order = [DataSetType::ENCODING_1,DataSetType::ENCODING_2,DataSetType::ENCODING_3,DataSetType::RETRIEVAL_1];
         $ths->setTargetLogic(TargetHitSearcher::TARGET_HIT_LOGIC_RT);
      }
      else{
         $ans[self::COMPUTE_RET_ERROR] =  "Unrecoginzed study for fixation based NBack values: $study";
         return $ans;
      }
   
      $error = $ths->setTargetBoxes($hit_boxes);
      if ($error != ""){
         $ans[self::COMPUTE_RET_ERROR] = "Error setting the target boxes: " .  $error;
         return $ans;
      }
   
      $error = $ths->setNewTrial($trial_sequence);
      if ($error != ""){
         $ans[self::COMPUTE_RET_ERROR] = "Error setting the new trial: " .  $error;
         return $ans;
      }
   
      foreach ($fix_list_names as $fix_list) {
         foreach ($data_set_order as $data_set) {
             $fixation_list = $trial[TrialField::DATA][$data_set][$fix_list];
             //echo "   DATASET: $data_set\n";
             for ($i = 0; $i < count($fixation_list); $i++) {
                 //echo "      FIX: (" . $fixation_list[$i][FixationVectorField::X] . ","  . $fixation_list[$i][FixationVectorField::Y] . ")\n";
                 //$ths->dbugState("      ");
                 $result = $ths->isHit($fixation_list[$i][FixationVectorField::X], $fixation_list[$i][FixationVectorField::Y], $data_set);
                 //echo "      RESULT: TARGET HIT: " . $result[self::RET_TARGET_HIT] . ". IS IN: " . $result[self::RET_IS_IN] . "\n";
                 //$ths->dbugState("      ");
                 if ($result[self::RET_ERROR] != "") {
                     $ans[self::COMPUTE_RET_ERROR] = "Error computing ISHIT values for fixation $i for $data_set and $fix_list: " . $ans[self::RET_ERROR];
                     return $ans;
                 }

                 $fixation_list[$i][FixationVectorField::NBACK]       = $result[self::RET_NBACK];
                 $fixation_list[$i][FixationVectorField::IS_IN]       = $result[self::RET_IS_IN];       // This basically is a flag to detect wheter the fixation was in the target of interest, where interst is based on the context
                 $fixation_list[$i][FixationVectorField::TARGET_HIT]  = $result[self::RET_TARGET_HIT];  // This is the ID of the target hit if one was hit.

               if ($result[self::RET_SEQ_COMPLETED] == 1) {
                     $ans[$fix_list][self::COMPUTE_RET_SEQ_COMPLETE] = $i; // Saving the data set is not necessary as it's always the last one.
               }
             }
             // Storing the new values. 
             $fixation_list = $trial[TrialField::DATA][$data_set][$fix_list] = $fixation_list;
         }
      }
   
      return $ans;
   }   

   static function nonAdjustedTargetBoxes($adjusted_target_boxes){
      $non_adjusted_target_boxes = array();
      for ($i = 0; $i < count($adjusted_target_boxes); $i++){
         $tbox = $adjusted_target_boxes[$i];
         if (($i != 5) && ($i != 2)){
            $tbox[self::I_H] = $tbox[self::I_H]/2;
            $tbox[self::I_Y] = $tbox[self::I_Y] + $tbox[self::I_H]/2;
         }
         $non_adjusted_target_boxes[] = $tbox;
      }
      return $non_adjusted_target_boxes;
   }

   static function computeFixationHitsGoNoGo(&$hitboxes, &$fixations, $correct_target_box, $centerX){
      for ($i = 0; $i < count($fixations); $i++){
         $f = $fixations[$i];
         $is_in = TargetHitSearcher::hitLogicForGoNoGo($hitboxes,$f[FixationVectorField::X],$f[FixationVectorField::Y],$correct_target_box,$centerX);
         $f[FixationVectorField::IS_IN] = $is_in;
         $fixations[$i] = $f;
      }
   }

   private static function hitLogicForGoNoGo(&$hitboxes, $x, $y, $correct_target_box, $centerX){
      // Checks need to be before calling this function. 

      //var_dump($correct_target_box);

      if ($correct_target_box == GoNoGoTargetBoxes::LEFT) $non_correct_box = GoNoGoTargetBoxes::RIGHT; // If the left is the correct one, the right one is the wrong one
      else $non_correct_box = GoNoGoTargetBoxes::LEFT;                                                 // Otherwise the left is the wrong one. 

      if (TargetHitSearcher::isHitInBox($hitboxes[GoNoGoTargetBoxes::ARROW],$x,$y)) return 0;
      
      if (TargetHitSearcher::isHitInBox($hitboxes[$correct_target_box],$x,$y)) return 1;

      if (TargetHitSearcher::isHitInBox($hitboxes[$non_correct_box],$x,$y)) return -1;

      if ($correct_target_box == GoNoGoTargetBoxes::LEFT){
         // The correct target Box is the Left One. So isIN will be zero if x is between the arrow and the left target box
         // which means it's x value should be LOWER than the half of the screen or centerX
         if ($x < $centerX) return 0;
         else return -1;
      }
      else { // $correct_target_box should be RIGHT.
         // The correct target Box is the Right One. So isIN will be zero if x is between the arrow and the right target box
         // which means it's x value should be LARGER than the half of the screen or centerX
         if ($x > $centerX) return 0;
         else return -1;
      }      
      
   }

}

?>
