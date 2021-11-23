<?php
 
 $report = [];
 $report["uuid"] = "nbackrt";

 $report["page_titles"] = [
   ["EVALUATION: NBACK RT"],
   ["COGNITIVE PERFORMANCE AND RELATED BRAIN AREAS","OVERALL PERFORMANCE"],
   ["FIXATION PLOTTING", "FIXATION EXAMPLE"],
 ];

 $report["color_code"]["explanations"] = [
    "red" => "Severe and/or\npathological impairment",
    "green" => "No relevant problems\nwere encountered",
    "yellow" => "Mild impairment, not\nnecessarily pathological",
    "blue"   => "Above average\nperformance"
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

 $report["affected_areas"]["green"] = "Preserved Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["yellow"] = "Mildly Impaired Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["red"] = "Severely Impaired and/ or Pathological Cognitive Functions & Brain Areas:";

 $report["affected_areas"]["visual_exploration_efficiency"] = "Visual Exploratory Efficiency: Basal Ganglia (BG) & Superior Colliculus (SC)";
 $report["affected_areas"]["visual_working_memory"] = "Visual Working Memory: Medial Prefrontal Cortex (mPFC)";
 $report["affected_areas"]["correct_responses"] = "Correct Responses: Prefrontal Cortex, Anterior Cingulate Cortex (ACC)";
 $report["affected_areas"]["working_memory"] = "Working Memory: Frontal and Parietal Regions";
 $report["affected_areas"]["inhibitory_processes"] = "Inhibitory Processes: Caudal Inferior Frontal Gyrus (cIFG)";
 $report["affected_areas"]["decoding_efficiency"]  = "Decoding Efficiency: Lateral Geniculate Nucleus (lGN)";
 $report["affected_areas"]["enconding_efficiency"]  = "Encoding Efficiency:  Lateral Posterior (LP) Nucleus";

 $report["overall_performance"]["text"] = "Overall Performance";
 $report["overall_performance"]["vertex"]["visual_exploration_efficiency"]       = "Visual Exploration\nEfficiency";
 $report["overall_performance"]["vertex"]["visual_working_memory"]               = "Visual Working\nMemory";
 $report["overall_performance"]["vertex"]["correct_responses"]                   = "Correct\nResponses";
 $report["overall_performance"]["vertex"]["working_memory"]                      = "Working\nMemory";
 $report["overall_performance"]["vertex"]["inhibitory_processes"]                = "Inhibitory\nProcesses";
 $report["overall_performance"]["vertex"]["decoding_efficiency"]                 = "Decoding\nEfficiency";
 $report["overall_performance"]["vertex"]["enconding_efficiency"]                = "Encoding\nEfficiency";

 $report["fixation_plotting"]["text"] = "";
 $report["fixation_plotting"]["title_left"] = "ENCODING";
 $report["fixation_plotting"]["title_right"] = "RECOGNITION";
 $report["fixation_plotting"]["row_1"] = "Trial 14";
 $report["fixation_plotting"]["row_2"] = "Trial 55";
 $report["fixation_plotting"]["row_3"] = "Trial 92";
 $report["fixation_plotting"]["control"]["text"] = "Fixation Plotting Example";
 $report["fixation_plotting"]["control"]["label"] = "Control";
  
 return $report;

?>