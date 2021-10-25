<?php
    $qc = array();
    $qc["uuid"] = "qc";
    $qc["title"] = "QUALITY CONTROL ANALYSIS";

    $qc["data_points"]["title"] = "DATA POINTS (<<Y>>)";
    $qc["data_points"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["fixations"]["title"] = "FIXATIONS (<<Y>>)";
    $qc["fixations"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["sample_rate"]["title"] = "SAMPLE RATE (<<Y>>)";
    $qc["sample_rate"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["results"]["title"] = "QUALITY CONTROL RESULTS";
    $qc["results"]["good"] = "Data is suitable for analysis";
    $qc["results"]["bad"] = "Data is not within the expected quality parameters. We recommend repeating the study.";

    $qc["comments"] = "COMMENTS FROM EVALUATOR";

    $qc["study_name"]["Binding BC"] = "Color Combinations";
    $qc["study_name"]["Binding UC"] = "New Colors";
    $qc["study_name"]["Go No-Go"]   = "Go No-Go";
    $qc["study_name"]["NBack MS"]   = "NBack For Multiple Sclerosis";
    $qc["study_name"]["NBack RT"]   = "NBack RT";
    $qc["study_name"]["NBack VS"]   = "NBack RT For Training";
    $qc["study_name"]["Reading"]    = "Reading";

    return $qc;

?>