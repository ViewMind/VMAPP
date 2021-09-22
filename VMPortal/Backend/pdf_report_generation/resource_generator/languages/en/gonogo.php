<?php
 
 $report = [];
 $report["uuid"] = "gonogo";

 $report["page_titles"] = [
   ["EVALUATION: GO/NO GO TEST"],
   ["BRAIN AREA PERFORMANCE","OVERALL PERFORMANCE"],
   ["VISUAL WORKING MEMORY ","STUDY DURATION"],
   ["SACCADIC BEHAVIOR","FIXATION DURATION"],
   ["FIXATION PLOTTING","FIXATION CONTROL"],
   ["ANNEX"],
 ];

 $report["color_code"]["explanations"] = [
    "red" => "Severe impairment\nand/or pathological",
    "green" => "No relevant problems\nwere encountered",
    "yellow" => "Mild impairment, not\nnecessarily pathological"
 ];

 $report["color_code"]["title"] = "For each cognitive function analyzed, the results are indicated by the following color coding:";

//  [17:15, 8/13/2021] Gerardo Fernandez: Visual_Working_memory_Interference <- Processing_Speed_Interference #prefrontal cortex, caudate nucleus 
// Visual_Working_memory_Facilitated <- Processing_Speed_Facilitated  #prefrontal cortex
// [17:16, 8/13/2021] Gerardo Fernandez: Visual_Working_memory_Interference <- Processing_Speed_Interference
// [17:16, 8/13/2021] Gerardo Fernandez: Visual_Working_memory_Facilitated <- Processing_Speed_Facilitated

 $report["processing_speed"]["title"]                    = "PROCESSING SPEED";
 $report["processing_speed"]["range_text"]               = "Normal value below 1.8";
 $report["processing_speed_interference"]["title"]       = "VISUAL WORKING MEMORY INTERFERENCE";
 $report["processing_speed_interference"]["range_text"]  = "Normal value below 93";
 $report["processing_speed_facilitated"]["title"]        = "VISUAL WORKING MEMORY FACILITATED";
 $report["processing_speed_facilitated"]["range_text"]   = "Normal value below 49";
 $report["reaction_time_facilitated"]["title"]           = "REACTION TIME FACILITATED";
 $report["reaction_time_facilitated"]["range_text"]      = "Normal value below 784";
 $report["reaction_time_interference"]["title"]          = "REACTION TIME INTERFERENCE";
 $report["reaction_time_interference"]["range_text"]     = "Normal value below 829";
 $report["executive_functions"]["title"]                 = "EXECUTIVE FUNCTIONS";
 $report["executive_functions"]["range_text"]            = "Normal value below 320";
 $report["index_error_facilitated"]["title"]             = "INHIBITORY ALTERATIONS FACILITATED";
 $report["index_error_facilitated"]["range_text"]        = "Normal value below 15";
 $report["index_error_interference"]["title"]            = "INHIBITORY ALTERATIONS INTERFERENCE";
 $report["index_error_interference"]["range_text"]       = "Normal value below 31";


 $report["affected_areas"]["green"] = "Preserved Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["yellow"] = "Mildly Impaired Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["red"] = "Severely Impaired and/ or Pathological Cognitive Functions & Brain Areas:";
 ////
 $report["affected_areas"]["processing_speed"] = "Processing Speed: Frontal Lobe";
 $report["affected_areas"]["processing_speed_interference"] = "Prefrontal cortex, caudate nucleus";
 $report["affected_areas"]["processing_speed_facilitated"] = "Prefrontal Cortex";
 $report["affected_areas"]["reaction_time_interference"] = "Medial Prefrontal Cortex (mPFC), Anterior, Posterior";
 $report["affected_areas"]["reaction_time_facilitated"] = "Medial Prefrontal Cortex (mPFC), Subthalamic Nucleus";
 $report["affected_areas"]["executive_functions"] = "Prefrontal Cortex";
 $report["affected_areas"]["index_error_facilitated"]  = "Right Inferior Frontal Gyrus (rIFG)";
 $report["affected_areas"]["index_error_interference"]  = "Caudal Inferior Frontal Gyrus (cIFG)";

 $report["presumptive_diagnosis"]["left_title"] = "Presumptive Diagnosis";
 $report["presumptive_diagnosis"]["right_title"] = "A.I. Performance Index";
 $report["presumptive_diagnosis"]["texts"] = [
  1 => "Generalized Cognitive and Brain Alteration.",
  2 => "Problems for controlling cognitive and motor responses.",
  3 => "Delay when processing information.",
  4 => "Problems for encoding and decoding information",
  5 => "Performance within normal values",
  6 => "Problems for focusing dynamically on a particular activity",
];

 $report["overall_performance"]["text"] = "The following graph displays the general cognitive performance of the patient. For more information\non how to interpret this graph, please refer to the Annex – Overall Performance Graph Interpretation.";
 $report["overall_performance"]["vertex"]["processing_speed"]                 = "Processing Speed";
 $report["overall_performance"]["vertex"]["processing_speed_interference"]    = "Visual Working\nMemory Interference";
 $report["overall_performance"]["vertex"]["processing_speed_facilitated"]     = "Visual Working\nMemory Facilitated";
 $report["overall_performance"]["vertex"]["reaction_time_interference"]       = "Reaction Time\nInterference";
 $report["overall_performance"]["vertex"]["reaction_time_facilitated"]        = "Reaction Time\nFacilitated";
 $report["overall_performance"]["vertex"]["executive_functions"]              = "Executive\nFunctions";
 $report["overall_performance"]["vertex"]["index_error_facilitated"]          = "Inhibitory Alterations\nFacilitated";
 $report["overall_performance"]["vertex"]["index_error_interference"]         = "Inhibitory Alterations\nInterference";

 $report["gaze_duration"]["text"]   = "The Visual Working Memory graph displays the average amount of time [ms] needed by the patient to extract\ninformation from an image during a trial, in both Encoding and Recognition stages. For more\n information on how to interpret this graph, please refer to the ANNEX – Visual Working Meory\nGraph Interpretation.";
 $report["gaze_duration"]["ylabel"] = "Visual Working Memory";
 $report["gaze_duration"]["xlabel"] = ["CONTROL", "PATIENT"];
 $report["gaze_duration"]["legend"] = ["Facilitated Task","Inhibitory Task"];

 $report["study_duration"]["text"]   = "The Study Duration graph displays the average amount of time [ms] needed by the patient complete\nthe whole study, in both Encoding and Recognition stages. For more information on how to interpret\n this graph, please refer to the ANNEX – Study Duration Graph Interpretation";
 $report["study_duration"]["ylabel"] = "Study Duration";
 $report["study_duration"]["xlabel"] = ["CONTROL", "PATIENT"];
 $report["study_duration"]["legend"] = ["Facilitated Task","Inhibitory Task"];

 $report["saccadic_behaviour"]["text"]   = "The Saccadic Behavior graph displays the visual scanning capabilities while looking for targets, in\nboth Encoding and Recognition stages. For more information on how to interpret this graph, please\n refer to the ANNEX – Saccadic Behaviour Graph Interpretation.";
 $report["saccadic_behaviour"]["ylabel"] = "Saccadic Behaviour";
 $report["saccadic_behaviour"]["xlabel"] = ["CONTROL", "PATIENT"];
 $report["saccadic_behaviour"]["legend"] = ["Facilitated Task","Inhibitory Task"];

 $report["fixation_duration"]["text"]   = "The Fixation Duration graph displays the time for extracting relevant information in just once fixation,\nin both Encoding and Recognition stages. For more information on how to interpret this graph, please\nrefer to the ANNEX – Gaze Duration Graph Interpretation.";
 $report["fixation_duration"]["ylabel"] = "Fixation Duration";
 $report["fixation_duration"]["xlabel"] = ["CONTROL", "PATIENT"];
 $report["fixation_duration"]["legend"] = ["Facilitated Task","Inhibitory Task"];

 $report["fixation_plotting"]["text"] = "The following graphs contain the plotting of the fixations done by the patient (left column) and by a\ncognitively healthy control (right column) while completing the different tasks. Tasks with a green\narrow are referred as facilitated task and red arrows as tasks with interference.\n";
 $report["fixation_plotting"]["title_left"] = "PATIENT";
 $report["fixation_plotting"]["title_right"] = "CONTROL";
 $report["fixation_plotting"]["row_1"] = "Trial 10";
 $report["fixation_plotting"]["row_2"] = "Trial 25";
 $report["fixation_plotting"]["row_3"] = "Trial 40";
 $report["fixation_plotting"]["row_4"] = "Trial 55";
 
 return $report;

?>