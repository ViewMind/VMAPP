<?php

include_once("named_constants.php");
include_once("online_fixations.php");
include_once("aux_classes.php");

class RawDataProcessor {

   // Reading Tags in ASC files. 
   private const START_READING = "EmpiezaLectura";
   private const END_READING = "FinLectura";

   // We always consider the minimum fixation size to be 50 ms. 
   const MINIMUM_FIXATION_SIZE_MS = 50;


   private $raw_data;
   private $fixations;
   private $error;
   private $input_file;
   private $f;
   private $name;
   private $resolution;
   private $input_parser;
   private $fix_computation_method;
   private $json_loaded_fixations;

   /**
    * Configures the processor to extract the raw data and to know which function to use to compute fixations. 
    */
   public function setProcessingItem($file, $item) {
      $this->input_file = $file;
      $required_keys = ["code", "f", "resolution","converter","md_to_fixation_method"];
      foreach ($required_keys as $key){
         if (!array_key_exists($key,$item)){
            $this->error = "Required item key $key is not present in item array";
            return false;
         }
      }

      if (!is_file($file)){
         $this->error = "$file is not a file";
         return false;
      }

      $this->name = $item["code"];
      $this->f = $item["f"];
      $this->resolution = $item["resolution"];
      $this->input_parser = $item["converter"];
      $this->fix_computation_method = $item["md_to_fixation_method"];

      $this->raw_data = array();

      return true;
   }

   /**
    * Extract the raw data for each image (can be more than one per trial) according to the predefined parser. 
    */
   public function extractRawData(){
      if ($this->input_parser == InputFileParser::ASC_PARSER){
         $this->extractRawDataASC();
         return true;
      }
      else if (($this->input_parser == InputFileParser::JSON_BINDING) || 
               ($this->input_parser == InputFileParser::JSON_READING) || 
               ($this->input_parser == InputFileParser::JSON_GONOGO)  || 
               ($this->input_parser == InputFileParser::JSON_NBACKRT)  || 
               ($this->input_parser == InputFileParser::JSON_BINDING_BAD_LABEL) ){
         return $this->jsonRawDataParser();
      }
      else if ($this->input_parser == InputFileParser::LEGACY_BINDING){
         $this->bindingDATExtraction();
         return true;
      }
      else if ($this->input_parser == InputFileParser::LEGACY_READING){
         $this->readingDATExtraction();
         return true;
      }
      else {
         $this->error = "Unknown selected parser: " . $this->input_parser;
         return false;
      }
   }

   /**
    * Computes the fixations according to the predfined algorithm. 
    */

   public function calculateFixations($maximum_dispersion){

      // When EyeLink Method is used, Fixation were extracted along with raw data when on the raw data extraction step. 
      if ($this->fix_computation_method == FixationComputationMethod::EYELINK) return true;

      // Otherwise the Online Fixation Computation is used for the raw data. 
      return $this->onlineMaximumDispersionFixationComputation($maximum_dispersion);

   }

   /**
    * Simply returns the number of data sets. A data set and a trial are the same thing in single image studies (Reading/GoNoGo). Otherwise the belong to one part of the trial (NBack/Binding)
    */
   public function getDataSetCount(){
      return count($this->raw_data);
   }

   public function getName() {
      return $this->name;
   }

   public function getResolution(){
      return $this->resolution;
   }

   /**
    * Saves the raw data and fixation plots of the trials specified in the trial list.
    * If the list is empty, then all trials are created. A directory with the compounded name and the input file is created in the 
    * base directory.  
    */
   public function plotDataAndFixations($base_directory, $trial_list = []){

      if (empty($trial_list)) $trial_list = array_keys($this->raw_data);

      $W = $this->resolution[0];
      $H = $this->resolution[1];
      $R = $W*0.01;

      $info = pathinfo($this->input_file);

      $image_dir = "$base_directory/" . str_replace(" ","_",$this-> name) . "/" . $info["basename"];
      shell_exec("rm -rf $image_dir");
      shell_exec("mkdir -p $image_dir");

      if (!is_dir($image_dir)){
         $this->error = "Could not create output image directory: $image_dir";
         return false;
      }

      // Doing the actual fixation plotting.
      foreach ($trial_list as $trial) {
         $image = imagecreatetruecolor($W, $H);
         // Color definitions
         $blue = imagecolorallocate($image, 0, 0, 255);
         $red = imagecolorallocate($image, 255, 0, 0);
         $gray = imagecolorallocate($image, 200, 200, 200);
         $black = imagecolorallocate($image, 0, 0, 0);
         imagefill($image, 0, 0, $gray);
         // We always plot right eye data and fixations
         //var_dump($this->raw_data[$trial]);
         foreach ($this->raw_data[$trial] as $sample) {
             $x = $sample[1];
             $y = $sample[2];
             //echo "Plotting $x, $y\n";
             imageellipse($image, $x, $y, $R, $R, $blue);
         }
         $nfix = 0;         
         foreach ($this->fixations["R"][$trial] as $sample) {
            //[$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];
             $x  = $sample[0];
             $y  = $sample[1];
             $x1 = $sample[5];
             $y1 = $sample[7];
             $x2 = $sample[6];
             $y2 = $sample[8];
             imagefilledellipse($image, $x, $y, $R, $R, $red);
             //echo "$x1 $x2 $y1 $y2\n";
             imagerectangle($image, $x1, $y1, $x2, $y2, $red);
             $nfix++;
         }
         imagestring($image, 5, $W/2, 50, "Number of Fixations: $nfix", $black);
         imagepng($image, "$image_dir/$trial.png");
     }      

     return true;

   }


   /**
    * Computes the fixation distribution. Returns the list of values for the bins, with smallest rangest first to the highest ranges last. 
    * If trial list is not empty, the distribution is computed only over the trials of the list. 
    */

   public function fixationDistribution($bin_size, $trial_list = []){
      // Always analyzing right eye data.
      $bins = array();
      $largest_bin = 0;

      if (empty($trial_list)) $trial_list = array_keys($this->raw_data);

      //var_dump($this->fixations["R"]);
      //return array();

      foreach ($trial_list as $trial) {
         $fixation_list = $this->fixations["R"][$trial];
         foreach ($fixation_list as $fix_row){
            $duration = $fix_row[2]; // Position 2 is the position of the duration of the fixaion in the data row. 
            $bin_number = floor($duration/$bin_size);
            //echo "DUR $duration and BIN SIZE $bin_size give BIN: $bin_number\n";
            if ($bin_number > $largest_bin) {
                $largest_bin = $bin_number;
            }
            $bins[] = $bin_number;
         }
      }

      // Now that we now the largest we create the appropiate array:
      $values = array_fill(0,$largest_bin+1,0);
      foreach ($bins as $b){
         $values[$b]++;
      }

      return $values;

   }

   /**
    * Computes fixations in each image of the raw data using online maximum dispersion. 
    */

   private function onlineMaximumDispersionFixationComputation($maximum_dispersion){

      // Everytime fixations are computed the previous values are resete. 
      $this->fixations = array();
      $this->fixations["L"] = array();
      $this->fixations["R"] = array();      
     
      $onlineFixR = new OnlineMovingWindowAlgorithm($maximum_dispersion,$this->f,self::MINIMUM_FIXATION_SIZE_MS);
      $onlineFixL = new OnlineMovingWindowAlgorithm($maximum_dispersion,$this->f,self::MINIMUM_FIXATION_SIZE_MS);

      if ( !is_array($this->raw_data) ){
         $this->error = "Trying to compute fixations but set processing item was never called\n";
         return false;
      }
      
      if (count($this->raw_data) == 0){
         $this->error = "Trying to compute fixations but no raw data was found\n";
         return false;
      }

      foreach ($this->raw_data as $trial_number => $raw_data_list){

         if (empty($raw_data_list)){
            continue;
         }

         $this->fixations["R"][$trial_number] = array();
         $this->fixations["L"][$trial_number] = array();

         foreach ($raw_data_list as $raw_data) {
            $time = $raw_data[0];
            $xr = $raw_data[1];
            $yr = $raw_data[2];
            $yl = $raw_data[3];
            $xl = $raw_data[4];
            $fix = $onlineFixR->computeFixationsOnline($xr, $yr, $time);
            if ($fix->valid) {
               $this->fixations["R"][$trial_number][] = [$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];         
            }
            $fix = $onlineFixL->computeFixationsOnline($xl, $yl, $time);
            if ($fix->valid) {
               $this->fixations["L"][$trial_number][] = [$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];
            }
         }

         $fix = $onlineFixR->finalizeComputation();         
         if ($fix->valid) $this->fixations["R"][$trial_number][] = [$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];
         $fix = $onlineFixL->finalizeComputation();         
         if ($fix->valid) $this->fixations["L"][$trial_number][] = [$fix->x, $fix->y, $fix->duration, $fix->start, $fix->end, $fix->min_x, $fix->max_x, $fix->min_y, $fix->max_y];
      }

      return true;

   }

  /**
   * Extraction for ASC format from EyeLink Data. 
   * The format of the ASC (the raw data) seesm to be:
   * Time XL YL ?L XR YR ?R  -> I don't know what the ? columns represent.
   * Fixations are marked by EFIX (This is the end. The Start is Marked by SFIX). 
   * The format is 
   * EFIX [R|L] StartTime EndTime Duration X Y ? 
   */
   private function extractRawDataASC(){

      $lines = explode("\n",file_get_contents($this->input_file));

      $trial_counter = 0;

      $state_in_trial = false;
      $state_in_fixation_l = false;
      $state_in_fixation_r = false;
      
      $mm_l = new PointMinMax();
      $mm_r = new PointMinMax();
     
      foreach ($lines as $line){
   
         if ($state_in_trial){
            if (str_contains($line,self::END_READING)){
               $state_in_trial = false;
               $state_in_fixation_l = false;
               $state_in_fixation_r = false;
               continue;
            }
   
            // Removing the tabs.
            $line = str_replace("\t"," ",$line);
   
            // Making all spaces simple spaces. 
            while (true){
               $before = $line;
               $line = str_replace("  "," ",$line);
               if ($before == $line) break;
            }
   
            if (str_contains($line,"EFIX L")){
               // Fixation end.
               if ($state_in_fixation_l){
                  $parts = explode(" ",$line);
                  //echo count($parts) . " -> parts |$line|\n";
                  if (count($parts) == 8){
                     $start = $parts[2];
                     $end   = $parts[3];
                     $dur   = $parts[4];
                     $x     = $parts[5];
                     $y     = $parts[6];
                     $min_x = $mm_l->x->min;
                     $max_x = $mm_l->x->max;
                     $min_y = $mm_l->y->min;
                     $max_y = $mm_l->y->max;                     
                     $this->fixations["L"][$trial_counter][] = [$x,$y, $dur, $start, $end, $min_x, $max_x, $min_y, $max_y];
                  }
   
               }
               $state_in_fixation_l = false;
            }
            else if (str_contains($line,"SFIX L")){
               // Fixation start. 
               $state_in_fixation_l = true;
               $mm_l->reset();
            }
            else if (str_contains($line,"EFIX R")){
               // Fixation end.
               if ($state_in_fixation_r){
   
                  $parts = explode(" ",$line);
                  if (count($parts) == 8){
                     $start = $parts[2];
                     $end   = $parts[3];
                     $dur   = $parts[4];
                     $x     = $parts[5];
                     $y     = $parts[6];
                     $min_x = $mm_r->x->min;
                     $max_x = $mm_r->x->max;
                     $min_y = $mm_r->y->min;
                     $max_y = $mm_r->y->max;
                     $this->fixations["R"][$trial_counter][] = [$x,$y, $dur, $start, $end, $min_x, $max_x, $min_y, $max_y];
                  }
   
               }
               $state_in_fixation_r = false;
            }
            else if (str_contains($line,"SFIX R")){
               // Fixation start. 
               $state_in_fixation_r = true;
               $mm_r->reset();
            }         
            else if (str_contains($line,"SSACC")) continue;
            else if (str_contains($line,"ESACC")) continue;
   
            $parts = explode(" ",$line);
            if (count($parts) == 7){
               $time   = $parts[0];
               $xr     = $parts[4];
               $yr     = $parts[5];
               $xl     = $parts[1];
               $yl     = $parts[2];
               $this->raw_data[$trial_counter][] = [$time, $xr, $yr, $xl, $yl];
               if ($state_in_fixation_l) $mm_l->point($xl,$yl);
               if ($state_in_fixation_r) $mm_r->point($xr,$yr);
            }
   
         }
         else{
            if (str_contains($line,self::START_READING)){
               $state_in_trial = true;            
               $trial_counter++;
               $this->raw_data[$trial_counter] = array();
               $this->fixations["R"][$trial_counter] = array();
               $this->fixations["L"][$trial_counter] = array();
            }
         }
      }

   }

   /**
    * Loads the JSON Output of the an experiment to get the raw data for each image. 
    * Parsed According To study
    */
   private function jsonRawDataParser(){

      $trial_list = json_decode(file_get_contents($this->input_file),true);
      $this->json_loaded_fixations = array();
      $this->json_loaded_fixations["R"] = array();
      $this->json_loaded_fixations["L"] = array();

      // Resolution is overwritten by the whatever this loaded from the JSON file. 
      $pp_key = "processing_parameters";
      $resW = "resolution_width";
      $resH = "resolution_height";
      if (array_key_exists($pp_key, $trial_list)) {
         $pp = $trial_list[$pp_key];
         if (array_key_exists($resH,$pp) && array_key_exists($resW,$pp)){            
            $this->resolution = [ $pp[$resW], $pp[$resH] ];
         }
         else{
            echo "WARNING EITHER '$resH' or '$resW' is missing from $pp_key in " . $this->input_file . "\n";                     
         }
      }
      else{
         echo "WARNING: PROCESSING PARAMETERS KEY '$pp_key' NOT FOUND IN " . $this->input_file . "\n";         
      }

      if ($this->input_parser == InputFileParser::JSON_READING) {
         $hiararchy_chain = ["studies","Reading","trial_list"];
         $trial_hieararchy_chains = [ ["data","unique","raw_data"] ];
      }
      else if ($this->input_parser == InputFileParser::JSON_BINDING_BAD_LABEL){         
         $hiararchy_chain = ["studies","Binding BC","trial_list"];
         $trial_hieararchy_chains = [ ["data","encoding_1","raw_data"],  ["data","reterieval_1","raw_data"]];
      }
      else if ($this->input_parser == InputFileParser::JSON_BINDING){         
         $hiararchy_chain = ["studies","Binding BC","trial_list"];
         $trial_hieararchy_chains = [ ["data","encoding_1","raw_data"],  ["data","retrieval_1","raw_data"]];
      }
      else if ($this->input_parser == InputFileParser::JSON_GONOGO){         
         $hiararchy_chain = ["studies","Go No-Go","trial_list"];
         $trial_hieararchy_chains = [ ["data","unique","raw_data"] ];
      }
      else if ($this->input_parser == InputFileParser::JSON_NBACKRT){
         $hiararchy_chain = ["studies","NBack RT","trial_list"];
         $trial_hieararchy_chains = [ ["data","encoding_1","raw_data"],  ["data","encoding_2","raw_data"], ["data","encoding_3","raw_data"], ["data","retrieval_1","raw_data"] ];
      }
      else{
         $this->error = "Unknown file parser for data extraction on JSON: " . $this->input_parser;
         return false;
      }

      // First We get The trial List. 
      $trial_counter = 0;
      foreach ($hiararchy_chain as $key){
         if (!array_key_exists($key,$trial_list)){
            $this->error = "Could not find key $key when searching for trial list";
            return false;
         }
         else{
            $trial_list = $trial_list[$key];
         }
      }

      // Then we get the raw data for each part of each trial. 
      $trial_counter = 0;

      //echo "TRIAL LIST of " . count($trial_list) . "\n";

      foreach ($trial_list as $trial){

         foreach ($trial_hieararchy_chains as $thc){

            $raw_data = $trial;
            foreach ($thc as $key){
               if (!array_key_exists($key,$raw_data)){
                  $this->error = "Could not find key $key when searching for raw data on trial " . $trial["ID"];
                  return false;
               }
               else{
                  $raw_data = $raw_data[$key];
               }
            }
      
            // At this point we have the raw data list. 
            $trial_counter++;
            $this->raw_data[$trial_counter] = array();

            foreach ($raw_data as $point){
               $this->raw_data[$trial_counter][] = [ $point["ts"], $point["xr"], $point["yr"], $point["xl"], $point["yl"]] ;
            }

         }

      }

      return true;

   }

   /**
    * Extracts data from legacy DAT/DATF Binding Files
    */
   private function bindingDATExtraction(){

      $HEADER = "#IMAGE";

      $lines = explode("\n",file_get_contents($this->input_file));

      // Filtering the report.
      $headercounter = 0;
      for ($i = 0; $i < count($lines); $i++){
         if (str_contains($lines[$i],$HEADER)) $headercounter++;
         if ($headercounter == 2) break;
      }
      
      $resolution_line = $i+1;
      
      // Storing resolution, overwritting preset value. 
      $res = $lines[$resolution_line];
      $res = explode(" ",$res);
      $this->resolution =  [$res[0], $res[1]];
      
      //echo "RESOLUTION $W x $H\n";
      
      $start = $resolution_line + 1;
      $trial_counter = 0; // The very first line will increment it to 1. 
   
     
      for ($i = $start; $i < count($lines); $i++) {
      
         $line = trim($lines[$i]);
   
         $parts = explode(" ",$line);
   
         //echo "Part count " . count($parts) . "\n";
   
         if (count($parts) == 2){
            // This is a trial name and 0/1 for encoding/retrieval.            
            $trial_counter++;
            $this->raw_data[$trial_counter] = array();
            continue;               
         }
         else if (count($parts) == 8){
            var_dump($parts);
         }
         else if (count($parts) == 7){
            // This is actual data. 
            $time   = $parts[0];
            $xr     = $parts[1];
            $yr     = $parts[2];
            $xl     = $parts[3];
            $yl     = $parts[4];
            $this->raw_data[$trial_counter][] = [$time, $xr, $yr, $xl, $yl];
         }
   
      }
   }


   /**
    * Extracts data from legacy DAT/DATF Reading Files.
    */
   private function readingDATExtraction(){

      $HEADER = "#READING";
      
      $lines = explode("\n",file_get_contents($this->input_file));
  
      // Filtering the report.
      $headercounter = 0;
      for ($i = 0; $i < count($lines); $i++){
         if (str_contains($lines[$i],$HEADER)) $headercounter++;
         if ($headercounter == 2) break;
      }
      
      $resolution_line = $i+1;
      
      // Skipping the resolution
      $res = $lines[$resolution_line];
      $res = explode(" ",$res);
      $this->resolution =  [$res[0], $res[1]];
           
      $start = $resolution_line + 1;
      $trial_id = "";
      $trial_counter = 1;
   
      for ($i = $start; $i < count($lines); $i++) {
      
         $parts = explode(" ",$lines[$i]);
         if (count($parts) != 13) continue;
      
         $trial = $parts[0];
         $time   = $parts[1];
         $xr     = $parts[2];
         $yr     = $parts[3];
         $xl     = $parts[4];
         $yl     = $parts[5];
   
         if (($trial != $trial_id) && ($trial_id != "")){
            $trial_counter++;
            $this->raw_data[$trial_counter] = array();
         }
         $trial_id = $trial;
         
         $this->raw_data[$trial_counter][] = [$time, $xr, $yr, $xl, $yl];         
      
      }
   
   }
   

   /**
    * Get the error message if any. 
    */
   public function getError(){
      return $this->error;
   }



}

?>