<?php
    $qc = array();
    $qc["uuid"] = "qc";
    $qc["title"] = "ANÁLISIS DE CONTROL DE CALIDAD";

    $qc["data_points"]["title"] = "PUNTOS DE DATOS (<<Y>>)";
    $qc["data_points"]["subtitle"] = "Valor aceptable por encima de <<X>>";

    $qc["fixations"]["title"] = "FIJACIONES (<<Y>>)";
    $qc["fixations"]["subtitle"] = "Valor aceptable por encima de <<X>>";

    $qc["sample_rate"]["title"] = "FREQUENCIA DE SAMPLEO (<<Y>>)";
    $qc["sample_rate"]["subtitle"] = "Valor aceptable por encima de <<X>>";

    $qc["results"]["title"] = "RESULTADOS DE CONTROL DE CALIDAD";
    $qc["results"]["good"] = "Los datos son válidos para el análisis.";
    $qc["results"]["bad"] = "Los datos no se encuentran dentro de los márgenes de calidad esperados. Recomendamos repetir el estudio";

    $qc["comments"] = "COMENTARIOS DEL EVALUADOR";

    $qc["study_name"]["Binding BC"] = "Combinación de Colores";
    $qc["study_name"]["Binding UC"] = "Nuevos Colores";
    $qc["study_name"]["Go No-Go"]   = "Go No-Go";
    $qc["study_name"]["NBack MS"]   = "NBack Tipo MS";
    $qc["study_name"]["NBack RT"]   = "NBack Tipo RT";
    $qc["study_name"]["NBack VS"]   = "NBack Tipo RT Para Entrenamiento";
    $qc["study_name"]["Reading"]    = "Lectura";

    return $qc;

?>