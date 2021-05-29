<?php

include_once("DataContainerNames.php");

class Fixation{

   const ZERO_TOL_VALUE = 1e-6;

   static function CreateFixationFromDataSet(&$dataset, $start_index, $end_index, $eye, $compute_reading_parameters = false){

      if ($eye == EyeType::LEFT){
         $x_lab = RawDataVectorFields::XL;
         $y_lab = RawDataVectorFields::YL;
         $p_lab = RawDataVectorFields::PUPIL_L;
         $w     = RawDataVectorFields::WORD_L;
         $c     = RawDataVectorFields::CHAR_L;
      }
      else if ($eye == EyeType::RIGHT){
         $x_lab = RawDataVectorFields::XR;
         $y_lab = RawDataVectorFields::YR;
         $p_lab = RawDataVectorFields::PUPIL_R;
         $w     = RawDataVectorFields::WORD_R;
         $c     = RawDataVectorFields::CHAR_R;
      }

      $fix[FixationVectorFields::X]           = 0;
      $fix[FixationVectorFields::Y]           = 0;
      $fix[FixationVectorFields::PUPIL]       = 0;      
      $fix[FixationVectorFields::DURATION]    = 0;
      $fix[FixationVectorFields::START_INDEX] = $start_index;
      $fix[FixationVectorFields::START_TIME]  = $dataset[$start_index][RawDataVectorFields::TIMESTAMP];
      $fix[FixationVectorFields::END_INDEX]   = $end_index;
      $fix[FixationVectorFields::END_TIME]    = $dataset[$end_index][RawDataVectorFields::TIMESTAMP];

      if ($compute_reading_parameters){
         $fix[FixationVectorFields::WORD] = 0;
         $fix[FixationVectorFields::CHAR] = 0;
      }

      $counter = 0;
      $zero_counter = 0;
      for ($i = $start_index; $i <= $end_index; $i++){
         $fix[FixationVectorFields::X]     = $fix[FixationVectorFields::X] + $dataset[$i][$x_lab];
         $fix[FixationVectorFields::Y]     = $fix[FixationVectorFields::Y] + $dataset[$i][$y_lab];
         $fix[FixationVectorFields::PUPIL] = $fix[FixationVectorFields::PUPIL] + $dataset[$i][$p_lab];

         if ($compute_reading_parameters){
            $fix[FixationVectorFields::WORD] = $fix[FixationVectorFields::WORD] + $dataset[$i][$w];
            $fix[FixationVectorFields::CHAR] = $fix[FixationVectorFields::CHAR] + $dataset[$i][$c];   
         }

         $counter++;

         if (($dataset[$i][$p_lab] <= self::ZERO_TOL_VALUE) && ($dataset[$i][$p_lab] >= 0)){
            //echo $dataset[$i][$p_lab];
            $zero_counter++;
         }         

      }

      $fix[FixationVectorFields::X] = $fix[FixationVectorFields::X]/$counter;
      $fix[FixationVectorFields::Y] = $fix[FixationVectorFields::Y]/$counter;
      $fix[FixationVectorFields::PUPIL] = $fix[FixationVectorFields::PUPIL]/$counter;
      if ($compute_reading_parameters){
         $fix[FixationVectorFields::WORD] = $fix[FixationVectorFields::WORD]/$counter;
         $fix[FixationVectorFields::CHAR] = $fix[FixationVectorFields::CHAR]/$counter;   
      }      

      $fix[FixationVectorFields::ZERO_PUPIL] = $zero_counter;

      $fix[FixationVectorFields::DURATION]    = $fix[FixationVectorFields::END_TIME] - $fix[FixationVectorFields::START_TIME];
      $fix[FixationVectorFields::TIME]        = ($fix[FixationVectorFields::END_TIME] + $fix[FixationVectorFields::START_TIME])/2;

      return $fix;

   }

}

?>