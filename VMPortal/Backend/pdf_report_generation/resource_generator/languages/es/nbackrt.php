<?php
 
 $report = [];
 $report["uuid"] = "nbackrt";

 $report["page_titles"] = [
   ["EVALUACIÓN: NBACK RT"],
   ["DESEMPEÑO COGNITIVO Y ÁREAS DEL CEREBRO RELACIONADAS","DESEMPEÑO GENERAL"],
   ["GRÁFICO DE FIJACIONES","EJEMPLO DE FIJACIONES"]
 ];

 $report["color_code"]["explanations"] = [
  "red" => "Discapacidad severa\ny/o patológica",
  "green" => "No se encontraron\nproblemas relevantes",
  "yellow" => "Discapacidad leve, no\nnecesariamente patológica",
  "blue"   => "Desempeño por encima\ndel promedio"
];

$report["color_code"]["title"] = "Para cada función cognitiva analizada, los resultados son índicados con el siguiente código de colores:";

 $report["visual_exploration_efficiency"]["title"]                               = "EFICIENCIA EXPLORATORIA VISUAL";
 $report["visual_exploration_efficiency"]["range_text"]                          = "Valores normales por encimad de  4";
 $report["visual_working_memory"]["title"]                                       = "MEMORIA DE TRABAJO VISUAL";
 $report["visual_working_memory"]["range_text"]                                  = "Valores normales por debajo de 2400";
 $report["correct_responses"]["title"]                                           = "RESPUESTAS CORRECTAS";
 $report["correct_responses"]["range_text"]                                      = "Valores normales por encimad de  60";
 $report["working_memory"]["title"]                                              = "MEMORIA DE TRABAJO";
 $report["working_memory"]["range_text"]                                         = "Valores normales por encimad de  58";
 $report["inhibitory_processes"]["title"]                                        = "PROCESOS INHIBITORIOS";
 $report["inhibitory_processes"]["range_text"]                                   = "Valores normales por debajo de 18";
 $report["decoding_efficiency"]["title"]                                         = "EFICIENCIA DE DECODIFICACIÓN";
 $report["decoding_efficiency"]["range_text"]                                    = "Valores normales por debajo de 650";
 $report["enconding_efficiency"]["title"]                                        = "EFICIENCIA DE CODFICACIÓN";
 $report["enconding_efficiency"]["range_text"]                                   = "Valores normales por debajo de 260";

 $report["affected_areas"]["green"] = "Funciones cognitivas y áreas cerebrales preservadas:";
 $report["affected_areas"]["yellow"] = "Funciones cognitivas y áreas cerebrales con discapcidad leve:";
 $report["affected_areas"]["red"] = "Funciones cognitivas y áreas cerebrales con discapcidad severa y/o patológica:";

 $report["affected_areas"]["visual_exploration_efficiency"] = "Eficiencia Exploratoria Visual: Ganglios basales (BG) y colículo superior (SC)";
 $report["affected_areas"]["visual_working_memory"] = "Memoria de Trabajo Visual: Corteza prefrontal medial (mPFC)";
 $report["affected_areas"]["correct_responses"] = "Respuestas Correctas: Corteza prefrontal, corteza cingulada anterior (ACC)";
 $report["affected_areas"]["working_memory"] = "Memoria de Trabajo: Regiones frontales y parietales";
 $report["affected_areas"]["inhibitory_processes"] = "Procesos Inhibitorios: Circunvolución frontal inferior caudal (cIFG)";
 $report["affected_areas"]["decoding_efficiency"]  = "Eficiencia de Decodificación: Núcleo geniculado lateral (lGN)";
 $report["affected_areas"]["enconding_efficiency"]  = "Eficiencia de Encodificación: Núcleo lateral posterior (LP)";

 $report["overall_performance"]["text"] = "Desempeño General";
 $report["overall_performance"]["vertex"]["visual_exploration_efficiency"]       = "Eficiencia de Exploración\nVisual";
 $report["overall_performance"]["vertex"]["visual_working_memory"]               = "Memoria de Trabajo\nVisual";
 $report["overall_performance"]["vertex"]["correct_responses"]                   = "Respuestas\nCorrectas";
 $report["overall_performance"]["vertex"]["working_memory"]                      = "Memoria de\nTrabajo";
 $report["overall_performance"]["vertex"]["inhibitory_processes"]                = "Procesos\nInhibitorios";
 $report["overall_performance"]["vertex"]["decoding_efficiency"]                 = "Eficiencia de\nDecodificación";
 $report["overall_performance"]["vertex"]["enconding_efficiency"]                = "Eficiencia de\nEncodificación";

 $report["fixation_plotting"]["text"] = "";
 $report["fixation_plotting"]["title_left"] = "CODIFICACIÓN";
 $report["fixation_plotting"]["title_right"] = "RECONOCIMIENTO";
 $report["fixation_plotting"]["row_1"] = "Prueba 14";
 $report["fixation_plotting"]["row_2"] = "Prueba 55";
 $report["fixation_plotting"]["row_3"] = "Prueba 92";
 $report["fixation_plotting"]["control"]["text"] = "Ejemplo de Fijaciones";
 $report["fixation_plotting"]["control"]["label"] = "Control";
  
 return $report;

?>