SELECT '------------------- Patient Required Data -------------------' AS '';
SELECT * FROM tDoctors WHERE uid = 'ARvm0000';
SELECT '------------------- Patient Optional Data -------------------' AS '';
SELECT * FROM tPatientReqData WHERE doctorid = 'ARvm0000';
SELECT '------------------- Doctor Data -------------------' AS '';
SELECT * FROM tPatientOptionalData WHERE patientid = 'ARvm0001';
SELECT '------------------- Results Table  -------------------' AS '';
SELECT * FROM tEyeResults WHERE doctorid = 'ARvm0000';
SELECT '------------------- OlD STUFF CHECK FOR DOCTOR  -------------------' AS '';
SELECT * FROM tDoctors WHERE uid LIKE '%vm00%'




