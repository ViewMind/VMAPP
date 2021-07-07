<?php

include_once("DataContainerNames.php");

class Fixation{

   const ZERO_TOL_VALUE = 1e-6;

   static function CreateFixationFromDataSet(&$dataset, $start_index, $end_index, $eye, $compute_reading_parameters = false){

      if ($eye == Eye::LEFT){
         $x_lab = DataVectorField::XL;
         $y_lab = DataVectorField::YL;
         $p_lab = DataVectorField::PUPIL_L;
         $w     = DataVectorField::WORD_L;
         $c     = DataVectorField::CHAR_L;
      }
      else if ($eye == Eye::RIGHT){
         $x_lab = DataVectorField::XR;
         $y_lab = DataVectorField::YR;
         $p_lab = DataVectorField::PUPIL_R;
         $w     = DataVectorField::WORD_R;
         $c     = DataVectorField::CHAR_R;
      }

      $fix[FixationVectorField::X]           = 0;
      $fix[FixationVectorField::Y]           = 0;
      $fix[FixationVectorField::PUPIL]       = 0;      
      $fix[FixationVectorField::DURATION]    = 0;
      $fix[FixationVectorField::START_INDEX] = $start_index;
      $fix[FixationVectorField::START_TIME]  = $dataset[$start_index][DataVectorField::TIMESTAMP];
      $fix[FixationVectorField::END_INDEX]   = $end_index;
      $fix[FixationVectorField::END_TIME]    = $dataset[$end_index][DataVectorField::TIMESTAMP];

      if ($compute_reading_parameters){
         $fix[FixationVectorField::WORD] = 0;
         $fix[FixationVectorField::CHAR] = 0;
      }

      $counter = 0;
      $zero_counter = 0;
      for ($i = $start_index; $i <= $end_index; $i++){
         $fix[FixationVectorField::X]     = $fix[FixationVectorField::X] + $dataset[$i][$x_lab];
         $fix[FixationVectorField::Y]     = $fix[FixationVectorField::Y] + $dataset[$i][$y_lab];
         $fix[FixationVectorField::PUPIL] = $fix[FixationVectorField::PUPIL] + $dataset[$i][$p_lab];

         if ($compute_reading_parameters){
            $fix[FixationVectorField::WORD] = $fix[FixationVectorField::WORD] + $dataset[$i][$w];
            $fix[FixationVectorField::CHAR] = $fix[FixationVectorField::CHAR] + $dataset[$i][$c];   
         }

         $counter++;

         if (($dataset[$i][$p_lab] <= self::ZERO_TOL_VALUE) && ($dataset[$i][$p_lab] >= 0)){
            //echo $dataset[$i][$p_lab];
            $zero_counter++;
         }         

      }

      $fix[FixationVectorField::X] = $fix[FixationVectorField::X]/$counter;
      $fix[FixationVectorField::Y] = $fix[FixationVectorField::Y]/$counter;
      $fix[FixationVectorField::PUPIL] = $fix[FixationVectorField::PUPIL]/$counter;
      if ($compute_reading_parameters){
         $fix[FixationVectorField::WORD] = $fix[FixationVectorField::WORD]/$counter;
         $fix[FixationVectorField::CHAR] = $fix[FixationVectorField::CHAR]/$counter;   
      }      

      $fix[FixationVectorField::ZERO_PUPIL] = $zero_counter;

      $fix[FixationVectorField::DURATION]    = $fix[FixationVectorField::END_TIME] - $fix[FixationVectorField::START_TIME];
      $fix[FixationVectorField::TIME]        = ($fix[FixationVectorField::END_TIME] + $fix[FixationVectorField::START_TIME])/2;

      return $fix;

   }

}

?>