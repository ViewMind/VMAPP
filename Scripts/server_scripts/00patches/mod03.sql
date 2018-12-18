-- WARNING: Script requeres root access as it needs to be able to modify to different Databases.

USE viewmind_data;

DROP TABLE IF EXISTS tPatientIDs;

CREATE TABLE tPatientIDs (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid                VARCHAR(255) UNIQUE
);

-- Generating the existing patients its uids.
INSERT INTO tPatientIDs (uid) SELECT DISTINCT p.uid FROM tPatientReqData AS p;

-- Altering the EyeResult table to remove patientid.
ALTER TABLE tEyeResults ADD puid INT;
ALTER TABLE tPatientReqData ADD puid INT;
ALTER TABLE tPatientOptionalData ADD puid INT;

-- Using the keyids from tPatientIDs to add to the puid column in the results table.
UPDATE tEyeResults r JOIN tPatientIDs p ON p.uid = r.patientId SET puid = p.keyid; 

-- Doing the same for the Patient required and optional data.
UPDATE tPatientReqData prd JOIN tPatientIDs p ON p.uid = prd.uid SET puid = p.keyid;
UPDATE tPatientOptionalData pod JOIN tPatientIDs p ON p.uid = pod.patientid SET puid = p.keyid;

-- Deleting the NULL entries. Uncomment to enable
-- DELETE FROM tEyeResults WHERE puid IS NULL;

-- Once the information is saved thoruhg the puid column, the patientid column is removed.
ALTER TABLE tEyeResults DROP COLUMN patientid;
ALTER TABLE tPatientReqData DROP COLUMN uid;
ALTER TABLE tPatientOptionalData DROP COLUMN patientid;


-- //////////////////////////// SWITHCHING DBs /////////////////////////////////
USE viewmind_id;

DROP TABLE IF EXISTS tIDs;

CREATE TABLE tIDs (
    keyid   INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid     VARCHAR(255) UNIQUE
);

-- Copying stuff from the table in one DB into the other.
INSERT INTO tIDs SELECT * FROM viewmind_data.tPatientIDs;

-- Finally dropping the table from the original DB
DROP TABLE viewmind_data.tPatientIDs;
