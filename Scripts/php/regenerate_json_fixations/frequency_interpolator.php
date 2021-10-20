<?php

class FrequencyInterpolator {

   private $vmdata;
   private $target_interval;
   private $output_file;

   function __construct($input_file){
      $this->vmdata = json_decode(file_get_contents($input_file),true);

      // Getting the target sampling frequency
      $sampling_frequency = $this->vmdata["processing_parameters"]["sample_frequency"];

      // Computing the target interval in ms.
      $this->target_interval = floor(1000/$sampling_frequency);

      // Generating the output file name
      $pathinfo = pathinfo($input_file);
      $dir = $pathinfo["dirname"];
      $basename = $pathinfo["filename"];
      $this->output_file = "$dir/$basename" . "_interpolated.json";

   }

   function interpolate(){
      
      // Iterating over each study, then over the trial list of the study then over each dataset within each trial. 

      foreach ($this->vmdata["studies"] as $study_name => $study_data){

         $new_trial_list = array();

         foreach ($study_data["trial_list"] as $trial){         

            foreach ($trial["data"] as $dataset_name => $dataset){
               $raw_data = $this->linearInterpolateRawData($dataset["raw_data"]);
               $dataset["raw_data"] = $raw_data;
               $trial["data"][$dataset_name] = $dataset;
            }

            $new_trial_list[] = $trial;
            
         }

         $study_data["trial_list"] = $new_trial_list;
         $this->vmdata["studies"][$study_name] = $study_data;

      }

      // Storing the results in the output file
      $fid = fopen($this->output_file,"w");
      fwrite($fid,json_encode($this->vmdata,JSON_PRETTY_PRINT));
      fclose($fid);

      return $this->output_file;

   }

   /**
    * This will take a raw data vector and interpolate between two succesive points in the data in the following manner:
    *    The target interval is D (in ms). This is the ideal difference betwee two successive time stamps in data points. 
    *    The number of points to add between two successive datapoints A and B (B comes after A) is N = floor((B.ts-A.ts)/D) - 1. If N >= 1, the interpolation between A and B is carried out. 
    *    Interpolation is linear so the resulting points belong the formula Interpolated Point = A*(1-k) + B*k where the multiplication is carried out for each x, y coordinate for each eye. 
    *    So once the N is defined, we need to compute the alpha for each of the N values. But the DeltaK can be easily computed as DeltaK = D/(B.ts - A.ts)
    *    Hence using DeltaK and the Interpolated point formula each interpolation point can be easily coputed. 
    */
   private function linearInterpolateRawData($raw_data){

      // Intializing the first value. 
      $A = $raw_data[0];

      // For simplicity in the math we use a new name for the target difference in time stamps.
      $D = $this->target_interval;

      // The new array to return.
      $ndp = array();
      $ndp[] = $A;

      for ($i = 1; $i < count($raw_data); $i++){

         $B = $raw_data[$i];

         // Computing the necessary number of interpolated points.
         $STS = $A["ts"]; // STS = Start Time Stamp. 
         $TD = $B["ts"] - $STS;
         $N = floor($TD/$D) - 1;
                  
         if ($N >= 1){
            
            // Interpolation is carried out.
            
            $DeltaK = $D/$TD;  // Computing the delta. 

            // Computing the linear interpolation of each component for the right eye and left eye. 
            for ($j = 1; $j <= $N; $j++){
               $K = $DeltaK*$j;
               $new_point = array();
               
               // We round x and y position as they are pixel positions in the screen. 
               $new_point["pl"] = $A["pl"]*(1 - $K) + $B["pl"]*$K;
               $new_point["pr"] = $A["pr"]*(1 - $K) + $B["pr"]*$K;               
               $new_point["ts"] = $STS + $j*$D;
               $new_point["xl"] = round($A["xl"]*(1 - $K) + $B["xl"]*$K);
               $new_point["xr"] = round($A["xr"]*(1 - $K) + $B["xr"]*$K);
               $new_point["yl"] = round($A["yl"]*(1 - $K) + $B["yl"]*$K);
               $new_point["yr"] = round($A["yr"]*(1 - $K) + $B["yr"]*$K);               

               $new_point["i"]  = true; // Making sure the point is marked as interpolated. 

               $ndp[] = $new_point;
               
            }

         }

         // Adding the last data point and setting the current as previous
         $A = $B;
         $ndp[] = $B;

      }

      return $ndp;

   }


}

?>