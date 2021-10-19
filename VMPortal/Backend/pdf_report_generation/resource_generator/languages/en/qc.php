<?php
    $qc = array();
    $qc["uuid"] = "qc";
    $qc["title"] = "QUALITY CONTROL ANALYSIS";

    $qc["data_points"]["title"] = "DATA POINTS";
    $qc["data_points"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["fixations"]["title"] = "FIXATIONS";
    $qc["fixations"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["sample_rate"]["title"] = "SAMPLE RATE";
    $qc["sample_rate"]["subtitle"] = "Acceptable value above <<X>>";

    $qc["results"]["title"] = "QUALITY CONTROL RESULTS";
    $qc["results"]["good"] = "Data is suitable for analysis";
    $qc["results"]["bad"] = "Data is not within the expected quality parameters. We recommend repeating the study.";

    $qc["comments"] = "COMMENTS FROM EVALUATOR";

    return $qc;

?>