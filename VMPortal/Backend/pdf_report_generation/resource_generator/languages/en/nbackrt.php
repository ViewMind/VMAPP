<?php
 
 $report = [];
 $report["uuid"] = "nbackrt";

 $report["page_titles"] = [
   ["EVALUATION: NBACK RT"],
   ["COGNITIVE PERFORMANCE AND RELATED BRAIN AREAS","OVERALL PERFORMANCE"],
   ["FIXATION PLOTTING", "FIXATION EXAMPLE"],
 ];

 $report["color_code"]["explanations"] = [
    "red" => "Severe impairment\nand/or pathological",
    "green" => "No relevant problems\nwere encountered",
    "yellow" => "Mild impairment, not\nnecessarily pathological",
    "blue"   => "Above avegerage\nPerformance"
 ];

 $report["color_code"]["title"] = "For each cognitive function analyzed, the results are indicated by the following color coding:";

 $report["visual_exploration_efficiency"]["title"]                               = "VISUAL EXPLORATORY EFFICIENCY";
 $report["visual_exploration_efficiency"]["range_text"]                          = "Normal value above 4";
 $report["visual_working_memory"]["title"]                                       = "VISUAL WORKING MEMORY";
 $report["visual_working_memory"]["range_text"]                                  = "Normal value below 2400";
 $report["correct_responses"]["title"]                                           = "CORRECT RESPONSES";
 $report["correct_responses"]["range_text"]                                      = "Normal value above 60";
 $report["working_memory"]["title"]                                              = "WORKING MEMORY";
 $report["working_memory"]["range_text"]                                         = "Normal value above 58";
 $report["inhibitory_processes"]["title"]                                        = "INHIBITORY PROCESSES";
 $report["inhibitory_processes"]["range_text"]                                   = "Normal value below 18";
 $report["decoding_efficiency"]["title"]                                         = "DECODING EFFICIENCY";
 $report["decoding_efficiency"]["range_text"]                                    = "Normal value below 650";
 $report["enconding_efficiency"]["title"]                                        = "ENCODING EFFICIENCY";
 $report["enconding_efficiency"]["range_text"]                                   = "Normal value below 260";

 $report["overall_performance"]["text"] = "Overall Performance";
 $report["overall_performance"]["vertex"]["visual_exploration_efficiency"]       = "Visual Exploration\nEfficiency";
 $report["overall_performance"]["vertex"]["visual_working_memory"]               = "Visual Working\nMemory";
 $report["overall_performance"]["vertex"]["correct_responses"]                   = "Correct\nResponses";
 $report["overall_performance"]["vertex"]["working_memory"]                      = "Working\nMemory";
 $report["overall_performance"]["vertex"]["inhibitory_processes"]                = "Inhibitory\nProcesses";
 $report["overall_performance"]["vertex"]["decoding_efficiency"]                 = "Decoding\nEfficiency";
 $report["overall_performance"]["vertex"]["enconding_efficiency"]                = "Encoding\nEfficiency";

 $report["fixation_plotting"]["text"] = "Fixation Plotting Text";
 $report["fixation_plotting"]["title_left"] = "ENCODING";
 $report["fixation_plotting"]["title_right"] = "RECOGNITION";
 $report["fixation_plotting"]["row_1"] = "Trial 12";
 $report["fixation_plotting"]["row_2"] = "Trial 50";
 $report["fixation_plotting"]["row_3"] = "Trial 90";
 $report["fixation_plotting"]["control"]["text"] = "Fixation Plotting Example";
 $report["fixation_plotting"]["control"]["label"] = "Example";
  
 return $report;

?>
