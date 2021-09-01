<?php

/**
 * The script computes the optimum maximum dispersion for each entry file when compared against a reference to find the one the most similar distribution fixation duration. 
 * In order to do this, it does a Maximum Dispersion sweep of each of the files, except the referece and compares the fixation distribution agains the reference 
 * The MD with the smallest disperion is selected and the the fixation distribution and the referce is plotted against each other for each file.
 * Also a plot of the sum of the differences squared for each MD is plotted as well 
 * 
 */

  include_once("raw_data_extraction.php");

  ////////////////////// SETTINGS USED FOR READING STUDY
  $file_list = [
     "reading/eyelink_files/Paolini.asc",
     "reading/gp_files/reading_2021_08_26_07_42.json",
     "reading/hp_files/reading_2021_08_26_07_29.json",
     "reading/smi_files/jonatan_everti_reading_2018_09_18.dat"
  ];
  // For reading EyeLink is used for reference so the the MD is inconsecuential. 
  $reference_index = 0;
  $md_for_reference = 0;
  // Rerence trials. Empty, means all. 
  $reference_trials = [1,2,4,5,6,7,9];
  $working_directory = "reading_eyelink_references";
  $md_of_interest = [105, 197];

//   /////////////////////////// SETTINGS USED FOR BINDING TEST
//   $file_list = [
//      "binding/gp_files/binding_bc_3_l_2_2019_04_02_09_54.dat",
//      "binding/hp_files/binding_2021_08_05_16_47.json",
//      "binding/htc_files/binding_bc_3_l_2_2020_07_14_19_38.dat"
//   ];
//   // For this binding test of the system, we just use the HP file as a reference. 
//   $reference_index = 1;
//   $md_for_reference = 50;
//   // Rerence trials. Empty, means all. 
//   $reference_trials = [];
//   $working_directory = "binding_tests";
//   $md_of_interest = [105, 197];

  ////////////////////////////////// MD SWEEP AND MERIT SETUP

  $md_range = [20,500];
  $md_step = 10;
  

  // Which merit figure is used to compute the best MD. Possibilities are SUMABS and RMS 
  //$merit_figure_to_use = "SUMABS";
  $merit_figure_to_use = "RMS";
  
  // The range in ms for fixation duration histogram. 
  $bin_size = 50;
  $maximum_bin = 14; // Any bin, when computing the merit figure with an index higher than this is ignored. 

  /////////////////////////////////// CREATING ALL THE PATHS.
  $working_directory = "work/$working_directory";
  shell_exec("rm -rf $working_directory");
  $fixation_plots_location = "$working_directory/fixation_plots";
  shell_exec("mkdir -p $fixation_plots_location");
  $fixation_distribution_location = "$working_directory/fixation_distributions";
  shell_exec("mkdir -p $fixation_distribution_location");

  /////////////////////////////////// COMPUTING MD VECTOR
  $md_vector = array();
  for ($md = $md_range[0]; $md <= $md_range[1]; $md = $md + $md_step) {
     $md_vector[] = $md;
  }

  // Joining the MD vector with the MD of interst.
  $md_vector = array_merge($md_vector,$md_of_interest);
  // Making sure it is unique.
  $md_vector = array_unique($md_vector);
  // Sorting it from small to large (This is done in place.)
  sort($md_vector);

  /////////////////////////////////// REFERENCE COMPUTATIONS
  echo "Loading the metadata\n";
  $metadata = json_decode(file_get_contents("metadata.json"),true);

  $ref_file = $file_list[$reference_index];
  echo "REFERENCE FILE: $ref_file\n";
  echo "   Computing reference fixation distributions\n";

  if (!array_key_exists($ref_file,$metadata)){
     echo "ERROR: Could not find reference file $ref_file in loaded metada\n";
     exit();
  }

  $reference = new RawDataProcessor();
  if ( !$reference->setProcessingItem($ref_file, $metadata[$ref_file]) ){
     echo "Failed to set processing of reference data $ref_file: " . $reference->getError() . "\n";
     exit();
  }

  if (!$reference->extractRawData()){
     echo "Failed to extract of reference data $ref_file: " . $reference->getError() . "\n";
     exit();
  }
  $reference->calculateFixations($md_for_reference);

  $reference_histogram = $reference->fixationDistribution($bin_size,$reference_trials);

  echo "   Plotting reference fixations\n";
  $reference->plotDataAndFixations($fixation_plots_location,$reference_trials);

  //var_dump($reference_histogram);
  return;

  /////////////////////////////////// Raw Data and Fixations for all files. 
  $data = array();
  $merit_history = array();
  $md_points = array();

  for ($i = 0; $i < count($file_list); $i++){
  
      // Skip reference file for MD sweep. 
      if ($i == $reference_index) {
          continue;
      }
   
      $file = $file_list[$i];
      echo "FILE: $file\n";
      echo "   Extracting raw data\n";
      
      if (!array_key_exists($file,$metadata)){
       echo "ERROR: Could not find file $file in loaded metada\n";
       exit();
      }
    
      $rdp = new RawDataProcessor();
      if ( !$rdp->setProcessingItem($file, $metadata[$file]) ){
         echo "   ERROR: Failed to set processing data of file $file: " . $rdp->getError() . "\n";
         exit();
      }
   
      if (!$rdp->extractRawData()){
         echo "   ERROR: Failed to set extracting raw data: " . $rdp->getError() . "\n";
         exit();
      }
   
      echo "   Doing Fixation Sweep ...\n";
   
      
      $merit_array = array();

      // The Merit Structucure + the MD is saved here for plotting later. 
      $mds_to_plot = array();

      // Storing the values of the best. 
      $best_merit_array = array();
      $best_merit_array["merit"] = 1e100;
      $best_merit_md = 0;
   
      foreach ($md_vector as $md){

         if (!$rdp->calculateFixations($md)){
            echo "   ERROR: Fixation computations failed: " . $rdp->getError() . "\n";
            exit();
         }
         $histogram = $rdp->fixationDistribution($bin_size,$reference_trials);
         $merit = meritComputation($histogram,$reference_histogram,$merit_figure_to_use,$maximum_bin);
         if ($merit["merit"] < $best_merit_array["merit"]){
            $best_merit_array = $merit;
            $best_merit_md = $md;
         }

         if (in_array($md,$md_of_interest)){
            echo "   MD of Interest $md. Merit is: " . $merit["merit"] . "\n";
            $mds_to_plot[$md] = $merit;
         }
   
         $merit_array[] = $merit["merit"];
   
      }

      $merit_history[$rdp->getName()] = $merit_array;   

      // Adding the best merit array to the mds to plot.
      $mds_to_plot[$best_merit_md] = $best_merit_array;

      echo "   BEST MD: $best_merit_md with a merit of " . $best_merit_array["merit"] . "\n";      

      // This way the we plot the best and the merits of interest. 
      foreach ($mds_to_plot as $md => $merit_array) {
          echo "      Creating Bar Graph For Comparison for $md\n";
          $graph = array();
          $graph["title"] = "Fixation Distribution Comparison for " . $rdp->getName() . " Using MD of $md";
          $graph["series"] = ["values" => $merit_array["values"], "reference" => $merit_array["ref"]];
          $labels = array();
          for ($j = 0; $j < count($merit_array["values"]); $j++) {
              $labels[] = "@ " . $j*$bin_size;  // This means the fixation starts at this value for this bin.
          }
          
          if ($md === $best_merit_md) $best = "BEST-";
          else $best = "";
          
          $graph["xvalues"] = $labels;
          $graph["xlabel"] = "Fixation Durations";
          $graph["output_file"] = $fixation_distribution_location . "/" . $rdp->getName() . "bar_graph_$best$md.png";
          $bar_graph_file = "$working_directory/bar_graph.json";
          $fid = fopen($bar_graph_file, "w");
          fwrite($fid, json_encode($graph, JSON_PRETTY_PRINT));
          fclose($fid);
          shell_exec("python3 plot_bar_graph.py $bar_graph_file");

          echo "      Creating Best MD Fixation Plots for $md\n";
          $rdp->calculateFixations($md); // No need to check for errors as this was one of the parameters used previously.

          $ppath = "$fixation_plots_location/MD_$best$md";
          shell_exec("mkdir -p $ppath");
          $rdp->plotDataAndFixations($ppath, $reference_trials);
      }

  }

  echo "Creating Merit Graph \n";
  
  $graph = array();
  $graph["title"] = "Merit Figure Graph";
  $graph["series"] = $merit_history;
  $graph["xvalues"] = $md_vector;
  $graph["xlabel"] = "Maximum Dispersion";
  $graph["output_file"] = "$working_directory/merit_figure_graph.png";
  $merit_graph_json = "$working_directory/merit_graph.json";
  $fid = fopen($merit_graph_json,"w");
  fwrite($fid,json_encode($graph,JSON_PRETTY_PRINT));
  fclose($fid);

  shell_exec("python3 plot_line_graph.py $merit_graph_json");

  echo "Finished\n";

?>