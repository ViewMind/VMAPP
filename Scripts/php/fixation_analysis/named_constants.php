<?php
abstract class ETCode
{
    const GP  = "GazePoint";
    const SMI = "SMI";
    const HP  = "HP Reberv";
    const HTC = "HTC Vive Eye Pro";
    const EYELINK = "EyeLink";
}

abstract class FixationComputationMethod {
   const MD = "MaximumDispersion";
   const EYELINK = "EyeLink";
}

abstract class SamplingFrequency {
   const GP = 150;
   const HTC = 120;
   const EYELINK = 1000;
   const SMI = 120;
   const SMI250 = 250;
   const HP = 120;
}

abstract class InputFileParser {
   const LEGACY_BINDING = "LegacyBinding";
   const LEGACY_READING = "LegacyReading";
   const ASC_PARSER     = "ASCParser";
   const JSON_READING   = "JSONReading";
   const JSON_BINDING   = "JSONBinding";
}

abstract class Resolutions {
   const SMI_HI_DEF = [1600,900];
   const GP_FULL_HD = [1920,1080];
   const GP_HD = [1366,768];
   const HP_HALF = [1158,1188];
   const HP_FULL = [2316,2376];
   const HTC = [1532,1704];
   const EYELINK = [1024,768];
}

?>