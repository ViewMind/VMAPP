<?php

 $report = [];
 
 $report["uuid"] = "binding";
 
 $report["page_titles"] = [
   ["EVALUATION: COLOR TASK - 2 TARGETS"],
   ["EVALUATION: COLOR TASK - 3 TARGETS"],
   ["COGNITIVE PERFORMANCE AND RELATED BRAIN AREAS","OVERALL PERFORMANCE"],
   ["GAZE DURATION","VISUAL SCANNING"],
   ["FIXATION DURATION","NUMBER OF FIXATIONS"],
   ["FIXATION PLOTTING","COGNITIVELY HEALTHY CONTROL FIXATIONS PLOTTING"],
 ];

 $report["color_code"]["explanations"] = [
    "red" => "Severe impairment\nand/or pathological",
    "green" => "No relevant problems\nwere encountered",
    "yellow" => "Mild impairment, not\nnecessarily pathological"
 ];

 $report["color_code"]["title"] = "For each cognitive function analyzed, the results are indicated by the following color coding:";

 $report["visual_search"]["title"]                    = "VISUAL SEARCH ";
 $report["visual_search"]["range_text"]               = "Normal value below 3.5";
 $report["gaze_dur"]["title"]                         = "GAZE DURATION";
 $report["gaze_dur"]["range_text"]                    = "Normal value above 800";
 $report["int_mem_perf"]["title"]                     = "INTEGRATIVE MEMORY PERFORMANCE";
 $report["int_mem_perf"]["range_text"]                = "Normal value above 68";
 $report["assoc_mem_perf"]["title"]                   = "ASOCIATIVE MEMORY PERFORMANCE";
 $report["assoc_mem_perf"]["range_text"]              = "Normal value above 67";
 $report["exec_functions"]["title"]                   = "EXECUTIVE FUNCTIONS";
 $report["exec_functions"]["range_text_3T"]           = "Normal value below 16";
 $report["exec_functions"]["range_text_2T"]           = "Normal value below 11";
 $report["parahipocampal_region"]["title"]            = "PARAHIPOCAMPAL REGION PRESERVATION";
 $report["parahipocampal_region"]["range_text_3T"]    = "Normal value above 63";
 $report["parahipocampal_region"]["range_text_2T"]    = "Normal value above 67";


 $report["affected_areas"]["green"] = "Preserved Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["yellow"] = "Mildly Impaired Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["red"] = "Severely Impaired and/ or Pathological Cognitive Functions & Brain Areas:";
 $report["affected_areas"]["visual_search"] = "Visual Search: Basal Ganglia, Superior Colicullus";
 $report["affected_areas"]["gaze_dur"] = "Gaze: Lateral Parietal Cortex, Medial Prefrontal Cortex (mPFC), Anterior Superior\nTemporal Sulcus (aSTS) ";
 $report["affected_areas"]["int_mem_perf"] = "Integrative Memory Functions: Enthorinal Cortex, Perirhinal Cortex";
 $report["affected_areas"]["assoc_mem_perf"] = "Associative Memory Performance: Hipocampus";
 $report["affected_areas"]["exec_func"] = "Executive Functions: Prefrontal Cortex";
 $report["affected_areas"]["parahipocampal_region"]  = "Parahipocampal Region";

 $report["presumptive_diagnosis"]["left_title"] = "Presumptive Diagnosis";
 $report["presumptive_diagnosis"]["right_title"] = "A.I. Performance Index";
 $report["presumptive_diagnosis"]["texts"] = [
   1 => "Probable Alzheimer's Disease. We recommend repeating\nthe Colors Test with 2 targets.",
   2 => "Generalized Cognitive and Brain Alteration. We recommend\nrepeating the Colors Test with 2 targets.",
   3 => "Probable Alzheimer's Disease",
   4 => "Inconclusive",
   5 => "Inconclusive Results. Please Check the quality of the\ndata gathered. We recommend to repeat the study.",
   6 => "Performance within normal values",
   7 => "Non Conclusive. We recommend repeating the study",
 ];
 
 $report["overall_performance"]["text"] = "The following graph displays the general cognitive performance of the patient. For more information\non how to interpret this graph, please refer to the Annex – Overall Performance Graph Interpretation.";
 $report["overall_performance"]["vertex"]["visual_search"]         = "Visual Search";
 $report["overall_performance"]["vertex"]["gaze_dur"]              = "Gaze Duration";
 $report["overall_performance"]["vertex"]["int_mem_perf"]          = "Integrative\nMemory";
 $report["overall_performance"]["vertex"]["assoc_mem_perf"]        = "Associative\nMemory";
 $report["overall_performance"]["vertex"]["exec_functions"]        = "Executive\nFunctions";
 $report["overall_performance"]["vertex"]["parahipocampal_region"] = "Parahipocampal\nregion";

 $report["gaze_duration"]["text"] = "The Gaze Duration graph displays the average amount of time [ms] needed by the patient to extract\ninformation from an image during a trial, in both Encoding and Recognition stages. For more\ninformation on how to interpret this graph, please refer to the ANNEX – Gaze Duration Graph\nInterpretation.";
 $report["gaze_duration"]["ylabel"] = "Gaze Duration";
 $report["gaze_duration"]["graph_title"] = ["PATIENT","COGNITIVELY HEALTHY CONTROL"];
 $report["gaze_duration"]["xlabel"] = ["ENCODING", "RECOGNITION"];
 $report["gaze_duration"]["legend"] = ["Integrative Memory","Associative Memory"];
 
 $report["visual_scanning"]["text"] = "The Visual Scanning graph displays the patient’s visual scanning capabilities while looking for targets,\nin both Encoding and Recognition stages. For more information on how to interpret this graph,\nplease refer to the ANNEX – Visual Scanning Interpretation.";
 $report["visual_scanning"]["ylabel"] = "Visual Scanning";
 $report["visual_scanning"]["graph_title"] = ["PATIENT","COGNITIVELY HEALTHY CONTROL"];
 $report["visual_scanning"]["xlabel"] = ["ENCODING", "RECOGNITION"];
 $report["visual_scanning"]["legend"] = ["Integrative Memory","Associative Memory"];

 $report["fixation_duration"]["text"] = "The Fixation Duration graph displays the average amount of time [ms] of the fixations performed by\nthe patient, in both Encoding and Recognition stages. For more information on how to interpret this\ngraph, please refer to the ANNEX – Fixation Duration Interpretation.";
 $report["fixation_duration"]["ylabel"] = "Fixation Duration";
 $report["fixation_duration"]["graph_title"] = ["PATIENT","COGNITIVELY HEALTHY CONTROL"];
 $report["fixation_duration"]["xlabel"] = ["ENCODING", "RECOGNITION"];
 $report["fixation_duration"]["legend"] = ["Integrative Memory","Associative Memory"];

 $report["number_of_fixations"]["text"] = "The Number of Fixations graph displays the number of fixations performed by the patient, in both\nEncoding and Recognition stages. For more information on how to interpret this graph, please refer\nto the ANNEX – Number of Fixations Interpretation.";
 $report["number_of_fixations"]["ylabel"] = "Numbe of Fixations";
 $report["number_of_fixations"]["graph_title"] = ["PATIENT","COGNITIVELY HEALTHY CONTROL"];
 $report["number_of_fixations"]["xlabel"] = ["ENCODING", "RECOGNITION"];
 $report["number_of_fixations"]["legend"] = ["Integrative Memory","Associative Memory"];

 $report["fixation_plotting"]["text"] = "The Fixations Plotting Chart displays the fixations performed by the patient, during the the trials 10, 20\nand 31 of the Encoding (left) and Recognition (Right) stages, in the Color Combination Test.";
 $report["fixation_plotting"]["title_left"] = "ENCODING";
 $report["fixation_plotting"]["title_right"] = "RECOGNITION";
 $report["fixation_plotting"]["row_1"] = "Trial 10";
 $report["fixation_plotting"]["row_2"] = "Trial 20";
 $report["fixation_plotting"]["row_3"] = "Trial 32";
 $report["fixation_plotting"]["control"]["text"] = "The Cognitively Healthy Control Fixations Plotting chart displays the fixations performed by a\nCognitively Healthy person, during the Encoding (left) and Recognition (Right) stages, in the Color\nCombination Test.";
 $report["fixation_plotting"]["control"]["label"] = "Example";
 
 return $report;

?> 
