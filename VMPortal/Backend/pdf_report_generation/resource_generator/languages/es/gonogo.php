<?php
 
 $report = [];
 $report["uuid"] = "gonogo";

 $report["page_titles"] = [
   ["EVALUACIÓN: PRUEBA GO/NO GO"],
   ["DESEMPEÑO COGNITIVO Y ÁREAS DEL CEREBRO RELACIONADAS","DESEMPEÑO GENERAL"],
   ["MEMORIA DE TRABAJO VISUAL "],
   ["COMPORTAMIENTO SACÁDICO","DURACIÓN DE FIJACIONES"],
   ["GRÁFICO DE FIJACIONES","CONTROL DE FIJACIONES"]
 ];

 $report["color_code"]["explanations"] = [
  "red" => "Discapacidad severa\ny/o patológica",
  "green" => "No se encontraron\nproblemas relevantes",
  "yellow" => "Discapacidad leve, no\nnecesariamente patológica"
];

 $report["color_code"]["title"] = "Para cada función cognitiva analizada, los resultados son índicados con el siguiente código de colores:";

 $report["processing_speed"]["title"]                    = "VELOCIDAD DE PROCESAMIENTO";
 $report["processing_speed"]["range_text"]               = "Valor normal por debajo de 1.8";
 $report["processing_speed_facilitated"]["title"]        = "MEMORIA DE TRABAJO VISUAL - FACILITADA";
 $report["processing_speed_facilitated"]["range_text"]   = "Valor normal por debajo de 49";
 $report["processing_speed_interference"]["title"]       = "MEMORIA DE TRABAJO VISUAL - INTERFERENCIA";
 $report["processing_speed_interference"]["range_text"]  = "Valor normal por debajo de 93";
 $report["reaction_time_facilitated"]["title"]           = "TIEMPO DE REACCIÓN - FACILITADO";
 $report["reaction_time_facilitated"]["range_text"]      = "Valor normal por debajo de 784";
 $report["reaction_time_interference"]["title"]          = "TIEMPO DE REACCIÓN - INTERFERENCIA";
 $report["reaction_time_interference"]["range_text"]     = "Valor normal por debajo de 829";
 $report["executive_functions"]["title"]                 = "FUNCIONES EJECUTIVAS";
 $report["executive_functions"]["range_text"]            = "Valor normal por debajo de 320";
 $report["index_error_facilitated"]["title"]             = "CAPACIDADES INHIBITORIAS - FACILITADAS";
 $report["index_error_facilitated"]["range_text"]        = "Valor normal por debajo de 15";
 $report["index_error_interference"]["title"]            = "CAPACIDADES INHIBITORIAS - INTERFERENCIA";
 $report["index_error_interference"]["range_text"]       = "Valor normal por debajo de 31";


 $report["affected_areas"]["green"] = "Funciones cognitivas y áreas cerebrales preservadas:";
 $report["affected_areas"]["yellow"] = "Funciones cognitivas y áreas cerebrales con discapcidad leve:";
 $report["affected_areas"]["red"] = "Funciones cognitivas y áreas cerebrales con discapcidad severa y/o patológica:";
  ////
 $report["affected_areas"]["processing_speed"] = "Velocidad de Procesamiento: Lóbulo Frontal";
 $report["affected_areas"]["processing_speed_interference"] = "Memoria de Trabajo Visual - Interferencia: Corteza prefrontal, núcleo caudado";
 $report["affected_areas"]["processing_speed_facilitated"] = "Memoria de Trabajo Visual - Facilitada: Corteza prefrontal";
 $report["affected_areas"]["reaction_time_interference"] = "Tiempro de Reacción  - Interferencia: Corteza prefrontal medial (mPFC), anterior, posterior";
 $report["affected_areas"]["reaction_time_facilitated"] = "Tiempro de Reacción  - Facilitado: Corteza prefrontal medial (mPFC), núcleo subtalámico";
 $report["affected_areas"]["executive_functions"] = "Funciones Ejecutivas: Corteza Prefrontal";
 $report["affected_areas"]["index_error_facilitated"]  = "Capacidades Inhibitorias Facilitadas: Circunvolución frontal inferior derecha (rIFG)";
 $report["affected_areas"]["index_error_interference"]  = "Capacidades Inhibitorias - Interferencia: Circunvolución frontal inferior caudal (cIFG)";

 $report["presumptive_diagnosis"]["left_title"] = "Diagnóstico Presuntivo";
 $report["presumptive_diagnosis"]["right_title"] = "Índice de Desempeño";
 $report["presumptive_diagnosis"]["texts"] = [
  1 => "Alteraciones Generales Cognitivas y Cerebrales.",
  2 => "Problemas controlando respuestas motoras y cognitivas.",
  3 => "Retraso al procesar información.",
  4 => "Problemas codificando y decodificando información",
  5 => "Desempeño dentro de valores nominales.",
  6 => "Problemas al concentrarse dinámicamente en una actividad particular.",
];

 $report["overall_performance"]["text"] = "El siguiente gráfico muestra el desempeño cognitivo general del paciente.";
 $report["overall_performance"]["vertex"]["processing_speed"]                 = "Velocidad de\nProcesamiento";
 $report["overall_performance"]["vertex"]["processing_speed_interference"]    = "Memoria de Trabajo\nVisual - Interferencia";
 $report["overall_performance"]["vertex"]["processing_speed_facilitated"]     = "Memoria de Trabajo\nVisual Facilitada";
 $report["overall_performance"]["vertex"]["reaction_time_interference"]       = "Tiempo de Reacción\nInterferencia";
 $report["overall_performance"]["vertex"]["reaction_time_facilitated"]        = "Tiempo de Reacción\nFacilitado";
 $report["overall_performance"]["vertex"]["executive_functions"]              = "Funciones\nEjecutivas";
 $report["overall_performance"]["vertex"]["index_error_facilitated"]          = "Alteraciones Inhibitorias\nFacilitadas";
 $report["overall_performance"]["vertex"]["index_error_interference"]         = "Alteraciones Inhibitorias\nInterferidas";

 $report["gaze_duration"]["text"]   = "El Gráfico de Memoria de Trabajo Visual muestra el tiempo promedio [ms] requerido por el paciente para\nextraeer información de la imagen durante una prueba";
 $report["gaze_duration"]["ylabel"] = "Memoria de Trabajo Visual";
 $report["gaze_duration"]["xlabel"] = ["CONTROL", "PACIENTE"];
 $report["gaze_duration"]["legend"] = ["Tarea Facilitada","Tarea con Interferencia"];

 $report["study_duration"]["text"]   = "The Study Duration graph displays the average amount of time [ms] needed by the patient complete\nthe whole study, in both Encoding and Recognition stages.";
 $report["study_duration"]["ylabel"] = "Duración del Estudio";
 $report["study_duration"]["xlabel"] = ["CONTROL", "PACIENTE"];
 $report["study_duration"]["legend"] = ["Tarea Facilitada","Tarea con Interferencia"];

 $report["saccadic_behaviour"]["text"]   = "El Gráfico de Comportamiento Sacádico muestra las capacidades de exploración visual mientras se\nbuscan objectivos.";
 $report["saccadic_behaviour"]["ylabel"] = "Comportamiénto Sacádico";
 $report["saccadic_behaviour"]["xlabel"] = ["CONTROL", "PACIENTE"];
 $report["saccadic_behaviour"]["legend"] = ["Tarea Facilitada","Tarea con Interferencia"];

 $report["fixation_duration"]["text"]   = "El Gráfico de Duración de Fijaciones muestra el tiempo requerido para extraer información relevante\ndurante una fijación";
 $report["fixation_duration"]["ylabel"] = "Duración de Fijaciones";
 $report["fixation_duration"]["xlabel"] = ["CONTROL", "PACIENTE"];
 $report["fixation_duration"]["legend"] = ["Tarea Facilitada","Tarea con Interferencia"];

 $report["fixation_plotting"]["text"] = "Los siguientes gráficos muestra las fijaciones realizadas por el paciente (izquierda) y por un control cognitivamente\nsaludable (derecha) durante algunas pruebas. Las tareas con flecha verde son las facilitadas miestras que aquellas\ncon flecha roja son con interferencia.";
 $report["fixation_plotting"]["title_left"] = "PACIENTE";
 $report["fixation_plotting"]["title_right"] = "CONTROL";
 $report["fixation_plotting"]["row_1"] = "Prueba 10";
 $report["fixation_plotting"]["row_2"] = "Prueba 25";
 $report["fixation_plotting"]["row_3"] = "Prueba 40";
 $report["fixation_plotting"]["row_4"] = "Prueba 55";
 
 return $report;

?>