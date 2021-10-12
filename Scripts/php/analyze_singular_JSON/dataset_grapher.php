<?php

class DataSetGrapher {

   const OPTION_ENABLE_FIX_DRAW       = "enable_fix_draw";
   const OPTION_ENABLE_FIX_DRAW_BOX   = "enable_fix_draw_box";
   const OPTION_ENABLE_DRAW_RAW_DATA  = "enable_draw_raw_data";
   const OPTION_LARGEST_FIXATION      = "largest_fixation";

   // File stuff
   private $json;
   private $output_dir;
   private $vmdata;
   private $qc_graph_dir;

   // Options
   private $enable_fixation_draw;
   private $enable_fixation_square_draw;
   private $enable_raw_data_draw;
   private $min_max_fixduration;

   // Processing paramters
   private $resolution;   
   private $hitboxes;
   private $sample_frequency;

   function __construct($input_file){
      $this->json = $input_file;
      
      // Path for the fixation analysis file and images. 
      $pathinfo = pathinfo($input_file);
      $path  = $pathinfo["dirname"];
      $this->output_dir = "$path/dataset_graphs";
      $this->qc_graph_dir = "$path/qc_graphs";

      // Crating the directory
      shell_exec("mkdir -p " . $this->output_dir);
      shell_exec("mkdir -p " . $this->qc_graph_dir);

      // By default everything is enabled. 
      $this->enable_fixation_draw = true;
      $this->enable_fixation_square_draw = true;
      $this->enable_raw_data_draw = true;

      // Loading the data. 
      $this->vmdata = json_decode(file_get_contents($input_file),true);
      
      // Getting the resolution
      $pp = $this->vmdata["processing_parameters"];
      $w = $pp["resolution_width"];
      $h = $pp["resolution_height"];
      $this->sample_frequency = $pp["sample_frequency"];
      $this->resolution = [$w, $h];
      
      // Get the minimim fixation duration
      $this->min_max_fixduration = [0,1000]; // Setting the max to 1 second = 1000 ms. 
      $this->min_max_fixduration[0] = $pp["minimum_fixation_length"];
      
      // Getting the possible Hitboxes. 
      $this->hitboxes = [];
      $possible_hitboxes = ["go_no_go_hitboxes","nback_hitboxes"];
      
      foreach ($possible_hitboxes as $hitbox_name){
         if (array_key_exists($hitbox_name,$pp)){
            $this->hitboxes = $pp[$hitbox_name];
            break;
         }
      }
   }

   function plotDataSets(){
      foreach ($this->vmdata["studies"] as $study_name => $study_data){

         $eye = $study_data["study_configuration"]["valid_eye"];
         $basename = str_replace(" ","_",$study_name);

         $this->qcGraphs($study_name,$eye,$study_data["quality_control"],$this->vmdata["qc_parameters"]);
      
         foreach ($study_data["trial_list"] as $trial){
            $trial_id = $trial["ID"];
            foreach ($trial["data"] as $dataset_name => $dataset){
      
               $output_name = $this->output_dir . "/$basename" . "_" . $trial_id . "_" . $dataset_name . ".png";
      
               echo "STUDY: $study_name. Trial: $trial_id. Dataset: $dataset_name\n";

               $this->generateDatasetImage($dataset,$eye,$output_name);
      
            }      
         }         
      }      
   }

   function setOption($name,$value){
      if ($name == self::OPTION_ENABLE_DRAW_RAW_DATA){
         $this->enable_raw_data_draw = $value;
      }
      else if ($name == self::OPTION_ENABLE_FIX_DRAW){
         $this->enable_fixation_draw = $value;
      }
      else if ($name == self::OPTION_ENABLE_FIX_DRAW_BOX){
         $this->enable_fixation_square_draw = $value;
      }
      else if ($name == self::OPTION_LARGEST_FIXATION){
         $this->min_max_fixduration[1] = $value;
      }
   }

   private function generateDatasetImage($dataset,$eye,$output_name){
      
      // Creating the image
      $image = imagecreatetruecolor($this->resolution[0], $this->resolution[1]);
   
      // Fixation analysis file. 
      $pathinfo = pathinfo($output_name);
      $fname = $pathinfo["filename"];
      $path  = $pathinfo["dirname"];
      $fix_analysis_file = "$path/$fname" . ".flog";
   
      $fid = fopen($fix_analysis_file,"w");
   
      $px = ""; $py = "";
      $fixations = array();
      if ($eye == "left"){
         $fixations = $dataset["fixations_l"];
         $px = "xl"; $py = "yl";
      }
      else {
         $fixations = $dataset["fixations_r"];
         $px = "xr"; $py = "yr";
      }
   
      $raw_data = $dataset["raw_data"];
   
      // Color definitions
      $blue         = imagecolorallocate($image, 0, 0, 127);
      $red          = imagecolorallocate($image, 170, 0, 0);
      $gray         = imagecolorallocate($image, 210, 210, 210);
      $green        = imagecolorallocate($image, 0, 85, 0);
      $fix_frame    = imagecolorallocate($image, 125, 14, 129);
      $black        = imagecolorallocate($image, 0, 0, 0);
      $font         = 5;
   
      // Background image
      imagefill($image, 0, 0, $gray);
   
      $MIN_R = $this->resolution[0]*0.008;  // Corresponds to minimum fixation duration
      $MAX_R = $MIN_R*20;                // Corresponds to maximum fixation duration. 
      $raw_data_R = $MIN_R/2;         
   
      // Coputing linear coefficients for proportional duration radious. 
      $fdmin = $this->min_max_fixduration[0];
      $fdmax = $this->min_max_fixduration[1];   
      $m  = ($MAX_R - $MIN_R)/($fdmax - $fdmin);
      $K  = $MIN_R - $m*$fdmin;
   
      // First we draw the hitboxes, if present.
      if (is_array($this->hitboxes)){
         foreach ($this->hitboxes as $hit_box){
            $x1 = $hit_box[0];
            $y1 = $hit_box[1];
            $x2 = $hit_box[0] + $hit_box[2];
            $y2 = $hit_box[1] + $hit_box[3];
            imagerectangle($image, $x1, $y1, $x2, $y2, $green);         
         }
      }
      
      // Second we draw the raw data, if present.
      if ($this->enable_raw_data_draw) {
         foreach ($raw_data as $point) {
            imagefilledellipse($image, $point[$px], $point[$py], $raw_data_R, $raw_data_R, $blue);
         }
      }
   
      // Third if enabled, we draw the fixations squares. These represeen the maximum dispersion used. 
      $metadata_text = array();
      $fix_order_counter = 1;
      if ($this->enable_fixation_square_draw){

         foreach ($fixations as $fix){
   
            // We first get the start and end time for the fixation. 
            $start_time = $fix["start_time"];
            $end_time   = $fix["end_time"];
            $fixation_points = $this->getFixationRawDataPoints($raw_data,$start_time,$end_time);
   
            // Saving the metadata text. 
            $N = count($fixation_points);
            if ($N == 0){
               echo "WARNING: EMPTY FIXATION POINTS WHEN SEARCHING FOR DATA POINTS IN FIXATION\n";
               echo "EXITING\n";
               echo "Start Time: $start_time. End Time: $end_time. RAW DATA:\n";
               var_dump($raw_data);
               exit();
            }
            //echo "Fixation points $N\n";
            $text = "$N - " . $fix["duration"] . "ms - " . $fix_order_counter;
            $fix_order_counter++;
            $metadata_text[] = $text;
   
            fwrite($fid,$text . "\n");
   
            // Drawing the fixation bounding box. 
            if ($N > 0) {
               $minx = $fixation_points[0][$px];
               $maxx = $fixation_points[0][$px];
               $miny = $fixation_points[0][$py];
               $maxy = $fixation_points[0][$py];
               $last = -1;
               $first = 0;
               foreach ($fixation_points as $point){
   
                  $current = $point["ts"];
                  if ($last != -1){
                     $text = $current . " " . ($current - $last);
                  }
                  else{
                     $first = $current;
                     $text = $current;
                  }
                  $last = $current;
   
                  fwrite($fid,"$text\n");
   
                  if ($minx > $point[$px]) $minx = $point[$px];
                  else if ($maxx < $point[$px]) $maxx = $point[$px];
                  if ($miny > $point[$py]) $miny = $point[$py];
                  else if ($maxy < $point[$py]) $maxy = $point[$py];
               }
   
               // Saving the last to the log.
               fwrite($fid,"TOTAL: " . ($current - $first) . "\n");
   
               imagerectangle($image, $minx, $miny, $maxx, $maxy, $fix_frame);
            }
            
   
         }

      }
   
      // Finally the actual fixations are drawn. 
      if ($this->enable_fixation_draw){
   
         $index_counter = 0;
   
         foreach ($fixations as $fix) {
            $x = $fix["x"];
            $y = $fix["y"];
            $d = $fix["duration"];
            
            // Make sure the fixation falls withing programmed bounds and compute the proportional radious. 
            if ($d < $fdmin) $d = $fdmin;
            else if ($d > $fdmax) $d = $fdmax;
            $R = $m*$d + $K;
   
            imagefilledellipse($image, $x, $y, $raw_data_R, $raw_data_R, $red); // Center dot of the fixation.
            imageellipse($image, $x, $y, $R, $R, $red);
   
            // Drawing the metadata text if available.
            if (array_key_exists($index_counter,$metadata_text)){
               $text = $metadata_text[$index_counter];
               imagestring($image, $font, $x, $y, $text, $red);
            }
   
            $index_counter++;
   
         }
      }
   
      // Close the fixation analysis file
      fclose($fid);
   
      // Finally we store the image. 
      imagepng($image, $output_name);
   
   }

   private function getFixationRawDataPoints($raw_data,$start,$end){
      $status = 0;
      $fpoints = array();
      foreach ($raw_data as $point){
         if ($status == 0){
            if ($point["ts"] == $start){
               $fpoints[] = $point;
               $status++;   
            }
         }
         else if ($status == 1){
            $fpoints[] = $point;
            if ($point["ts"] == $end){            
               $status++;   
               break;
            }
         }
      }
   
      // This doubles a coherence check as BOTH the start and end timestamp need to be found
      // as the start and end of consecutive fixations. 
      if ($status == 2) return $fpoints;
      return array();
   
   } 
   
   private function qcGraphs($study_name,$eye,$qc,$qc_params){

      $qcp    = $qc_params[$study_name];
      $fplus  = $qc_params["allowed_f_plus_variation"];
      $fminus = $qc_params["allowed_f_minus_variation"];

      $basename = str_replace(" ","_",$study_name);
      $json_output =  $this->qc_graph_dir . "/$basename.json"; // Since this is communication file between PHP and Python only one file name is needed for all files
      $graph_name_f  =  $this->qc_graph_dir . "/$basename-f.png";
      $graph_name_dp  =  $this->qc_graph_dir . "/$basename-dp.png";
      $graph_name_fix  =  $this->qc_graph_dir . "/$basename-fix.png";

      // Plotting the frequency variation.
      $fdata_y = $qc["avg_freq_per_trial"];

      $trial_list = array();
      $fdata_plus_y = array();
      $fdata_minus_y = array();
      for ($i = 0; $i < count($fdata_y); $i++){
         $trial_list[] = $i;
         $fdata_plus_y[] = $this->sample_frequency*(1 + $fplus/100);
         $fdata_minus_y[] = $this->sample_frequency*(1 - $fminus/100);
      }

      $plot_data["xvalues"] = $trial_list;
      $plot_data["series"] = [
         "Max F" => $fdata_plus_y,
         "Min F" => $fdata_minus_y,
         "F" => $fdata_y
      ];
      $plot_data["xlabel"] = "trial_number";
      $plot_data["ylabel"] = "F in Hz";
      $plot_data["title"]  = "Average Sample Frequency in each trial";
      $plot_data["output_file"] = $graph_name_f;
      
      $fid = fopen($json_output,"w");
      fwrite($fid,json_encode($plot_data));
      fclose($fid);

      shell_exec("python3 plot_line_graph.py $json_output");

      // Plotting the data points and glitches per trial in the same graph per trial. 
      $dp_y = $qc["points_per_trial"];
      $gl_y = $qc["glitches_per_trial"];
      $plot_data = array();
    
      $plot_data["xvalues"] = $trial_list;
      $plot_data["series"] = [
         "DataPoints" => $dp_y,
         "FGlitches" => $gl_y
      ];
      $plot_data["xlabel"] = "trial_number";
      $plot_data["ylabel"] = "Number";
      $plot_data["title"]  = "Number of Data Points and Frequency Glitches Per trial";
      $plot_data["output_file"] = $graph_name_dp;

      $fid = fopen($json_output,"w");
      fwrite($fid,json_encode($plot_data));
      fclose($fid);

      shell_exec("python3 plot_bar_graph.py $json_output");

      // Plotting the fixations per trial.
      $plot_data = array();
    
      $plot_data["xvalues"] = $trial_list;
      $plot_data["series"] = [
         "Fixations" => $qc["fixations_per_trial"]
      ];
      $plot_data["xlabel"] = "trial_number";
      $plot_data["ylabel"] = "Number";
      $plot_data["title"]  = "Fixations Per trial";
      $plot_data["output_file"] = $graph_name_fix;

      $fid = fopen($json_output,"w");
      fwrite($fid,json_encode($plot_data));
      fclose($fid);

      shell_exec("python3 plot_bar_graph.py $json_output");
        
   }   

}


?>