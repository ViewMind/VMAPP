use viewmind_develop;
SELECT '------------------- Patient Required Data -------------------' AS '';
SELECT * FROM viewmind_develop.tDoctors WHERE uid = 'ARvm0000';
SELECT '------------------- Patient Optional Data -------------------' AS '';
SELECT * FROM tPatientReqData WHERE doctorid = 'ARvm0000';
SELECT '------------------- Doctor Data -------------------' AS '';
SELECT * FROM tPatientOptionalData WHERE patientid = 'ARvm0001';
SELECT '------------------- Results Table  -------------------' AS '';
SELECT * FROM tEyeResults WHERE doctorid = 'ARvm0000';




