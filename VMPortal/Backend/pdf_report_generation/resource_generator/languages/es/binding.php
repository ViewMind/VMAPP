<?php

 $report = [];
 
 $report["uuid"] = "binding";
 
 $report["page_titles"] = [
   ["EVALUACIÓN: TAREA DE COLORES - 2 OBJECTIVOS"],
   ["EVALUACIÓN: TAREA DE COLORES - 3 OBJECTIVOS"],
   ["DESEMPEÑO COGNITIVO Y ÁREAS DEL CEREBRO RELACIONADAS","DESEMPEÑO GENERAL"],
   ["MEMORIA DE TRABAJO VISUAL ","EXPLORACIÓN VISUAL"],
   ["DURACIÓN DE FIJACIONES","NÚMERO DE FIJACIONES"],
   ["GRÁFICO DE FIJACIONES","GRÁFICO DE FIJACIONES COGNITIVAMENTE SALUDABLE"],
 ];

 $report["color_code"]["explanations"] = [
    "red" => "Discapacidad severa\ny/o patológica",
    "green" => "No se encontraron\nproblemas relevantes",
    "yellow" => "Discapacidad leve, no\nnecesariamente patológica"
 ];

 $report["color_code"]["title"] = "Para cada función cognitiva analizada, los resultados son índicados con el siguiente código de colores:";

 $report["visual_search"]["title"]                    = "BÚSQUEDA VISUAL";
 $report["visual_search"]["range_text"]               = "Valor normal por debajo de 3.5";
 $report["gaze_dur"]["title"]                         = "MEMORIA DE TRABAJO VISUAL";
 $report["gaze_dur"]["range_text"]                    = "Valor normal por encima de 800";
 $report["int_mem_perf"]["title"]                     = "DESPEÑO DE MEMORIA INTEGRADORA";
 $report["int_mem_perf"]["range_text"]                = "Valor normal por encima de 68";
 $report["assoc_mem_perf"]["title"]                   = "DESEMPEÑO DE MEMORIA ASOCIATIVA";
 $report["assoc_mem_perf"]["range_text"]              = "Valor normal por encima de 67";
 $report["exec_functions"]["title"]                   = "FUNCIONES EJECUTIVAS";
 $report["exec_functions"]["range_text_3T"]           = "Valor normal por debajo de 16";
 $report["exec_functions"]["range_text_2T"]           = "Valor normal por debajo de 11";
 $report["parahipocampal_region"]["title"]            = "PRESERVACIÓN DE LA REGIÓN PARAHIPOCAMPAL";
 $report["parahipocampal_region"]["range_text_3T"]    = "Valor normal por encima de 63";
 $report["parahipocampal_region"]["range_text_2T"]    = "Valor normal por encima de 67";
 
 $report["affected_areas"]["green"] = "Funciones cognitivas y áreas cerebrales preservadas:";
 $report["affected_areas"]["yellow"] = "Funciones cognitivas y áreas cerebrales con discapcidad leve:";
 $report["affected_areas"]["red"] = "Funciones cognitivas y áreas cerebrales con discapcidad severa y/o patológica:";
 
 $report["affected_areas"]["visual_search"] = "Búsqueda Visual: Ganglia Basal y Colículo Superior";
 $report["affected_areas"]["gaze_dur"] = "Mirada: Corteza Parietal Lateral, Corteza Prefrontal Medial (mPFC), Surco Temporal\nSuperior Anterior (aSTS) ";
 $report["affected_areas"]["int_mem_perf"] = "Funciones de Memoria Integradora:  Corteza entorrinal, corteza perirrinal";
 $report["affected_areas"]["assoc_mem_perf"] = "Desempeño de Memoria Asociativa: Hipocampo";
 $report["affected_areas"]["exec_func"] = "Funciones Ejecutivos: Corteza Prefrontal";
 $report["affected_areas"]["parahipocampal_region"]  = "Región parahipocampal";

 $report["presumptive_diagnosis"]["left_title"] = "Diagnosis Presuntiva";
 $report["presumptive_diagnosis"]["right_title"] = "Índice de Desempeño de A.I.";
 $report["presumptive_diagnosis"]["texts_3T"] = [
  1 => "Alteración Cognitiva y del Cerebro Generalizada.\nRecomendamos repetir el estudio de colores con 2 objectivos.",
  2 => "Alteración de la Memoria Associativa y Córtica.\nRecomendamos repetir el estudio de colores con 2 objectivos.",
  3 => "Enfermedad de Alzheimer probable.\nRecomendamos repetir el estudio de colores con 2 objectivos.",
  4 => "Enfermedad de Alzheimer probable.",
  5 => "Desempeño dentro de valores normales.",
  6 => "Resultados inconclusos. Por favor, verifique la calidad de los datos\nobtenidos. Recomendamos repetir el estudio.",
  7 => "Resultados inconclusos. Recomendamos repetir el estudio.",
];

$report["presumptive_diagnosis"]["texts_2T"] = [
  1 => "Alteración Cognitiva y del Cerebro Generalizada.",
  2 => "Alteración de la Memoria Associativa y Córtica.\nRecomendamos repetir el estudio de colores con 2 objectivos y agregar el estudio Go No-Go.",
  3 => "Enfermedad de Alzheimer probable.\nRecomendamos realizar el estudio Go No-Go.",
  4 => "Enfermedad de Alzheimer probable.",
  5 => "Desempeño dentro de valores normales.",
  6 => "Resultados inconclusos. Por favor, verifique la calidad de los datos\nobtenidos. Recomendamos repetir el estudio.",
];
 
 $report["overall_performance"]["text"] = "El siguiente gráfico muestra el desempeño cognitivo general del paciente.";
 $report["overall_performance"]["vertex"]["visual_search"]         = "Búsqueda Visual";
 $report["overall_performance"]["vertex"]["gaze_dur"]              = "Memoria de Trabajo\nVisual";
 $report["overall_performance"]["vertex"]["int_mem_perf"]          = "Memoria\nde Integración";
 $report["overall_performance"]["vertex"]["assoc_mem_perf"]        = "Memoria\nAsociativa";
 $report["overall_performance"]["vertex"]["exec_functions"]        = "Funciones\nEjecutivas";
 $report["overall_performance"]["vertex"]["parahipocampal_region"] = "Región\nParahipocampal";

 $report["gaze_duration"]["text"] = "El Gráfico de Memoria de Trabajo Visual muestra el tiempo promedio [ms] requerido por un paciente\npara extraer información de una imágen durante una prueba, tanto en la fase\nde Codificación como en la de Reconocimiento";
 $report["gaze_duration"]["ylabel"] = "Memoria de Trabajo Visual";
 $report["gaze_duration"]["graph_title"] = ["PACIENTE","CONTROL COGNITIVAMENTE SALUDABLE"];
 $report["gaze_duration"]["xlabel"] = ["CODIFICACIÓN", "RECONOCIMIENTO"];
 $report["gaze_duration"]["legend"] = ["Memoria de Integración","Memoria Asociativa"];
 
 $report["visual_scanning"]["text"] = "El Gráfico de Exploración Visual, muestra las capacidades de exploración visual del paciente mientras\nbusca a los objetivos tanto en la fase de Codificación como en la de Reconocimiento";
 $report["visual_scanning"]["ylabel"] = "Exploración Visual";
 $report["visual_scanning"]["graph_title"] = ["PACIENTE","CONTROL COGNITIVAMENTE SALUDABLE"];
 $report["visual_scanning"]["xlabel"] = ["CODIFICACIÓN", "RECONOCIMIENTO"];
 $report["visual_scanning"]["legend"] = ["Memoria de Integración","Memoria Asociativa"];

 $report["fixation_duration"]["text"] = "El Gráfico de Duración de Fijaciones muestra la duración promedio de las fijaciones realizadas\npor el paciente tanto en la fase de Codificación como en la de Reconocimiento";
 $report["fixation_duration"]["ylabel"] = "Duración de las Fijaciones";
 $report["fixation_duration"]["graph_title"] = ["PACIENTE","CONTROL COGNITIVAMENTE SALUDABLE"];
 $report["fixation_duration"]["xlabel"] = ["CODIFICACIÓN", "RECONOCIMIENTO"];
 $report["fixation_duration"]["legend"] = ["Memoria de Integración","Memoria Asociativa"];

 $report["number_of_fixations"]["text"] = "El grafico del Número de Fijaciones muestra la cantidad de fijaciones realizadas por el paciente\ntanto en la fase de Codificación como en la de Reconocimiento";
 $report["number_of_fixations"]["ylabel"] = "Número de Fijaciones";
 $report["number_of_fixations"]["graph_title"] = ["PACIENTE","CONTROL COGNITIVAMENTE SALUDABLE"];
 $report["number_of_fixations"]["xlabel"] = ["CODIFICACIÓN", "RECONOCIMIENTO"];
 $report["number_of_fixations"]["legend"] = ["Memoria de Integración","Memoria Asociativa"];

 $report["fixation_plotting"]["text"] = "El gráfico muestra las fijaciones realizadas por el paciente, durante las pruebas 10, 20y 31 en\nCodificación (izquierda) y Reconocimiento (derecha), de la Tarea de Combinación de Colores";
 $report["fixation_plotting"]["title_left"] = "CODIFICACIÓN";
 $report["fixation_plotting"]["title_right"] = "RECONOCIMIENTO";
 $report["fixation_plotting"]["row_1"] = "Prueba 10";
 $report["fixation_plotting"]["row_2"] = "Prueba 20";
 $report["fixation_plotting"]["row_3"] = "Prueba 32";
 $report["fixation_plotting"]["control"]["text"] = "El gráfico muestra las fijaciones realizadas por una persona sana en las fases de Codificación (Izquierda)\ny Reconocimiento (Derecha), en la Tarea de Combinación de Colores";
 $report["fixation_plotting"]["control"]["label"] = "Ejemplo";
 
 return $report;

?>