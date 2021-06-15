<?php

include ("Fixation.php");
include_once ("TargetHitSearcher.php");

class FixationDataGenerator {

   private $sample_frequency;
   private $max_dispersion_window;
   private $minimum_fixation_length;
   private $latency_escape_radious;
   private $resolution_width;
   private $resolution_height;
   private $minimum_window_size;
   private $screen_center_x;
   private $screen_center_y;
   private $sqr_sac_lat_tol;

   private const MM_MIN       = "min";
   private const MM_MAX       = "max";

   // Saccade normalization constant. 
   private const SNC          = 20;

   private $error;

   function __construct(){
      $this->sample_frequency = -1;
      $this->points_for_fixation = array();
      $this->min_max_x = array();
      $this->min_max_y = array();
   }

   function getError(){
      return $this->error;
   }

   function getScreenCenter(){
      return [$this->screen_center_x,$this->screen_center_y];
   }

   function setProcessingParameters($db_pp){

      // There are two processing parameter types: Those that come from the database which a fixed for a type of equipment.
      // Those that come from the client and have to do with the way stuff is drawn. 

      $expected_data = [ProcessingParameter::SAMPLE_FREQUENCY,       
      ProcessingParameter::MAX_DISPERSION_WINDOW,  
      ProcessingParameter::MINIMUM_FIXATION_LENGTH,
      ProcessingParameter::LATENCY_ESCAPE_RADIOUS,
      ProcessingParameter::RESOLUTION_WIDTH, 
      ProcessingParameter::RESOLUTION_HEIGHT];

      foreach ($expected_data as $proc_param){
         if (!array_key_exists($proc_param,$db_pp)){
            $this->error = "$proc_param was not found in the processing parameters array.";
            return false;
         }
      }

      $this->sample_frequency        = $db_pp[ProcessingParameter::SAMPLE_FREQUENCY];
      $this->max_dispersion_window   = $db_pp[ProcessingParameter::MAX_DISPERSION_WINDOW];
      $this->minimum_fixation_length = $db_pp[ProcessingParameter::MINIMUM_FIXATION_LENGTH];
      $this->latency_escape_radious  = $db_pp[ProcessingParameter::LATENCY_ESCAPE_RADIOUS];
      $this->resolution_width        = $db_pp[ProcessingParameter::RESOLUTION_WIDTH];
      $this->resolution_height       = $db_pp[ProcessingParameter::RESOLUTION_HEIGHT];

      // Computing the minimum window size.
      $sample_rate_in_ms = 1000.0/$this->sample_frequency;
      $this->minimum_window_size = ceil($this->minimum_fixation_length/$sample_rate_in_ms);

      // Screen center and sacadic latency tolerance
      $this->screen_center_x = $this->resolution_width/2;
      $this->screen_center_y = $this->resolution_height/2;
      $this->sqr_sac_lat_tol = $this->latency_escape_radious*$this->latency_escape_radious;      

      return true;
   }

   /**
    * @brief Computing the fixation for a given dat set. 
    */

   function computeFixationsForDataSet(&$raw_data_list, $which_eye, $is_reading_study = false){

      $fixations = array();

      $start_index = 0;
      $end_index = 0;
      $window_expanded = false;

      $mmx[self::MM_MAX] = 0;
      $mmx[self::MM_MIN] = 0;
      $mmy[self::MM_MAX] = 0;
      $mmy[self::MM_MIN] = 0;

      $data_set_length = count($raw_data_list);

      if ($which_eye == Eye::LEFT){
         $xlabel = DataVectorField::XL;
         $ylabel = DataVectorField::YL;
      }
      else if ($which_eye == Eye::RIGHT){
         $xlabel = DataVectorField::XR;
         $ylabel = DataVectorField::YR;
      }
      else{
         $this->error = "Unknown eye paratemeter $which_eye";
         return array();
      }

      while (true){

        $end_index = $start_index + $this->minimum_window_size - 1;
        $window_expanded = false;

        // If the end index is outside the bounds the remaining data can be discarded as a fixation there
        // would be shorter than the minimum fixation. So the algorithm ends here.
        if ($end_index >= $data_set_length) break; 
        
        $mmx = $this->findDispersionLimits($raw_data_list,$start_index,$end_index,$xlabel);
        $mmy = $this->findDispersionLimits($raw_data_list,$start_index,$end_index,$ylabel);

        while ($this->dispersion($mmx,$mmy) <= $this->max_dispersion_window){

           // Increasing the window size by 1
           $end_index++;
           $window_expanded = true;

           if ($end_index >= $data_set_length){
              // This was the final fixation.
              $fixations[] = Fixation::CreateFixationFromDataSet($raw_data_list,$start_index,$end_index-1,$which_eye,$is_reading_study);
              return $fixations;
           }

           // A new data point is added as part of the fixation
           $mmx[self::MM_MAX] = max($mmx[self::MM_MAX],$raw_data_list[$end_index][$xlabel]);
           $mmx[self::MM_MIN] = min($mmx[self::MM_MIN],$raw_data_list[$end_index][$xlabel]);           
           $mmy[self::MM_MAX] = max($mmy[self::MM_MAX],$raw_data_list[$end_index][$ylabel]);
           $mmy[self::MM_MIN] = min($mmy[self::MM_MIN],$raw_data_list[$end_index][$ylabel]);   
        }

        // If the code, got here, it means the new point is NOT part of the fixation.
        if ($window_expanded){
           // This means that the current window IS a fixation.
           $fixations[] = Fixation::CreateFixationFromDataSet($raw_data_list,$start_index,$end_index-1,$which_eye, $is_reading_study);
           $start_index = $end_index;
        }
        else{
           // The current minumum window is NOT a fixation. We simply move one to the right.
           $start_index++;
        }

      }
      
      return $fixations;
   }

   /**
    * @brief computes all values which are unique for each data set. 
    */
   function computeDataSetValues(&$raw_data_list, &$fix_l, &$fix_r, &$hitbox){

      // All values to compute
      $data_set_values[DataSetComputedValues::DURATION] = 0;
      $data_set_values[DataSetComputedValues::SAC_LAT_L] = 0;
      $data_set_values[DataSetComputedValues::SAC_LAT_R] = 0;
      $data_set_values[DataSetComputedValues::GAZE_L] = 0;
      $data_set_values[DataSetComputedValues::GAZE_R] = 0;
      $data_set_values[DataSetComputedValues::FIX_IN_CENTER_L] = 0;
      $data_set_values[DataSetComputedValues::FIX_IN_CENTER_R] = 0;
      // These will be computed only if the hitbox is non empty. 
      $data_set_values[DataSetComputedValues::RESPONSE_TIME_L] = 0;
      $data_set_values[DataSetComputedValues::RESPONSE_TIME_R] = 0;

      // Duration is the difference between timestamps of the first and last values of the dataset. 
      $last = count($raw_data_list) - 1;
      $data_set_values[DataSetComputedValues::DURATION] = $raw_data_list[$last][DataVectorField::TIMESTAMP] - $raw_data_list[0][DataVectorField::TIMESTAMP];

      // Sacadic latency
      $data_set_values[DataSetComputedValues::SAC_LAT_L] = $this->computeSacadicLatency($raw_data_list,Eye::LEFT);
      $data_set_values[DataSetComputedValues::SAC_LAT_R] = $this->computeSacadicLatency($raw_data_list,Eye::RIGHT);

      // Gazing: The sum of all fixation lengts for each eye. 
      $data_set_values[DataSetComputedValues::GAZE_L] = $this->computeGaze($fix_l);
      $data_set_values[DataSetComputedValues::GAZE_R] = $this->computeGaze($fix_r);

      // The number of fixations that fall in the lat sac radious of the center of the screen. 
      $data_set_values[DataSetComputedValues::FIX_IN_CENTER_L] = $this->computeNumberOfFixationInCenter($fix_l);
      $data_set_values[DataSetComputedValues::FIX_IN_CENTER_R] = $this->computeNumberOfFixationInCenter($fix_r);

      // A hitbox is a 4 Value vector. 
      if (count($hitbox) == 4){
         $index = TargetHitSearcher::findFirstFixationThatHits($hitbox,$fix_l);
         if ($index == -1) $data_set_values[DataSetComputedValues::RESPONSE_TIME_L] = "N/A";
         else $data_set_values[DataSetComputedValues::RESPONSE_TIME_L] = $fix_l[$index][FixationVectorField::TIME] - $raw_data_list[0][DataVectorField::TIMESTAMP];
         $index = TargetHitSearcher::findFirstFixationThatHits($hitbox,$fix_r);
         if ($index == -1) $data_set_values[DataSetComputedValues::RESPONSE_TIME_R] = "N/A";
         else $data_set_values[DataSetComputedValues::RESPONSE_TIME_R] = $fix_r[$index][FixationVectorField::TIME] - $raw_data_list[0][DataVectorField::TIMESTAMP];
      }

      return $data_set_values;

   }
   /**
    * @brief computes all values unique to each fixation. 
    */
   function computeFixationDependantValues(&$fixations,$start_time){
      $last_x = -1;
      $last_y = -1;
      for ($i = 0; $i < count($fixations); $i++){

         $sacade = 0;

         if (($last_x > -1) && ($last_y > -1)){
            $normX = ($last_x - $fixations[$i][FixationVectorField::X])*self::SNC/$this->resolution_width;
            $normY = ($last_y - $fixations[$i][FixationVectorField::Y])*self::SNC/$this->resolution_height;
            $sacade = sqrt($normX*$normX + $normY*$normY);
         }

         $last_x = $fixations[$i][FixationVectorField::X];
         $last_y = $fixations[$i][FixationVectorField::Y];
         
         // The sacade amplitude
         $fixations[$i][FixationVectorField::SAC_AMP] = $sacade;       

         // The timeline. 
         $fixations[$i][FixationVectorField::TIMELINE] = $fixations[$i][FixationVectorField::START_TIME] - $start_time; 
      }
   }

   private function computeGaze(&$raw_data_list){
      $gaze = 0;
      foreach ($raw_data_list as $f){
         $gaze = $gaze + $f[FixationVectorField::DURATION];         
      }
      return $gaze;
   }

   private function computeSacadicLatency(&$raw_data_list, $eye){
      $sac_lat = 0;
      if ($eye == Eye::LEFT){
         $x = DataVectorField::XL;
         $y = DataVectorField::YL;
      }
      else{
         $x = DataVectorField::XR;
         $y = DataVectorField::YR;
      }

      $start_time = $raw_data_list[0][DataVectorField::TIMESTAMP];

      for ($i = 0; $i < count($raw_data_list); $i++){
         
         $diffx = $raw_data_list[$i][$x] - $this->screen_center_x;
         $diffx = $diffx*$diffx;

         $diffy = $raw_data_list[$i][$y] - $this->screen_center_y;
         $diffy = $diffy*$diffy;

         if (($diffx+$diffy) > $this->sqr_sac_lat_tol){
            $sac_lat = $raw_data_list[$i][DataVectorField::TIMESTAMP] - $start_time;
            break;
         }
      }

      return $sac_lat;
   }

   private function computeNumberOfFixationInCenter(&$fix){
      $counter = 0;

      $x = FixationVectorField::X;
      $y = FixationVectorField::Y;

      for ($i = 0; $i < count($fix); $i++){
         
         $diffx = $fix[$i][$x] - $this->screen_center_x;
         $diffx = $diffx*$diffx;

         $diffy = $fix[$i][$y] - $this->screen_center_y;
         $diffy = $diffy*$diffy;

         if (($diffx+$diffy) <= $this->sqr_sac_lat_tol){
            $counter++;
         }
      }

      return $counter;            
   }

   private function dispersion($mmx, $mmy){
      $dispersion = $mmx[self::MM_MAX] - $mmx[self::MM_MIN];
      $dispersion = $dispersion + ($mmy[self::MM_MAX] - $mmy[self::MM_MIN]);
      return $dispersion;
   }

   private function findDispersionLimits(&$raw_data_list, $start_index, $end_index, $value_name){
       $mm[self::MM_MAX] = $raw_data_list[$start_index][$value_name];
       $mm[self::MM_MIN] = $raw_data_list[$start_index][$value_name];

       for ($i = $start_index+1; $i <= $end_index; $i++) {
           $mm[self::MM_MAX] = max($mm[self::MM_MAX], $raw_data_list[$i][$value_name]);
           $mm[self::MM_MIN] = min($mm[self::MM_MIN], $raw_data_list[$i][$value_name]);
       }

       return $mm;
   }
}

?> 
