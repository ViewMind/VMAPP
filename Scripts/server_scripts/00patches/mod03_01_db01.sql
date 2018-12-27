DROP TABLE IF EXISTS tPatientData;
DROP TABLE IF EXISTS TableC;
DROP TABLE IF EXISTS tPatientIDs;
DROP TABLE IF EXISTS tPlacedProducts;

-- Creating the unified patient data table
CREATE TABLE tPatientData (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid                VARCHAR(255) UNIQUE,
    doctorid           VARCHAR(255),
    firstname          VARCHAR(255),
    lastname           VARCHAR(255),
    idtype             VARCHAR(255),
    sex                CHAR,
    birthcountry       VARCHAR(2),
    birthdate          DATE,
    date_insertion     DATETIME,    
    state              VARCHAR(255),
    city               VARCHAR(255),
    mainactivity       VARCHAR(20),
    highestdegree      VARCHAR(20),
    physicalactivity   VARCHAR(5),
    nofspokenlang      TINYINT,
    agreesharedata     TINYINT,
    familyhistory      VARCHAR(255),
    patienthistory     VARCHAR(255),
    remedies           VARCHAR(255),
    diagnostic         VARCHAR(255),
    puid               INT
);

-- Creating the Patient ID table.
CREATE TABLE tPatientIDs (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid                VARCHAR(255) UNIQUE
);

-- Creating the placed product tables
CREATE TABLE tPlacedProducts (
   keyid               INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
   institution         INT,
   product             VARCHAR(255),
   product_serialnum   VARCHAR(255),
   software_version    VARCHAR(255),
   pc_model            VARCHAR(255),
   etbrand             VARCHAR(255),
   etmodel             VARCHAR(255),
   etserial            VARCHAR(255),
   chinrest_model      VARCHAR(255),
   chinrest_serialnum  VARCHAR(255)
);

-- Altering the EyeResult table to add the puid and remove the patientid
ALTER TABLE tEyeResults ADD puid INT;

-- Creating the table with all the information I want.
CREATE TABLE TableC AS
SELECT a.uid, a.firstname, a.lastname, a.doctorid, a.idtype, a.sex, a.birthcountry, a.birthdate, b.date_insertion, b.state, b.city, b.mainactivity, b.highestdegree, b.physicalactivity, b.nofspokenlang, b.agreesharedata, b.familyhistory, b.patienthistory, b.remedies, b.diagnostic
FROM (SELECT * 
      FROM tPatientReqData a
      WHERE a.keyid = (SELECT MAX(keyid) FROM tPatientReqData a2 WHERE a2.uid = a.uid)) a
JOIN (SELECT * 
      FROM tPatientOptionalData b
      WHERE b.keyid = (SELECT MAX(keyid) FROM tPatientOptionalData b2 WHERE b2.patientid = b.patientid)) b ON b.patientid = a.uid;
      
      
-- Inserting into the new table.
INSERT INTO tPatientData (uid, firstname, lastname, doctorid, idtype, sex, birthcountry, birthdate, date_insertion, state, city, mainactivity, highestdegree, physicalactivity, nofspokenlang, agreesharedata, familyhistory, patienthistory, remedies, diagnostic) 
SELECT uid, firstname, lastname, doctorid, idtype, sex, birthcountry, birthdate, date_insertion, state, city, mainactivity, highestdegree, physicalactivity, nofspokenlang, agreesharedata, familyhistory, patienthistory, remedies, diagnostic  FROM TableC;

-- Deleting the temporary table
DROP TABLE TableC;
DROP TABLE tPatientReqData;
DROP TABLE tPatientOptionalData;

-- Populating the patient ID tables.
INSERT INTO tPatientIDs (uid) SELECT DISTINCT p.uid FROM tPatientData AS p;

-- Using the keyids from tPatientIDs to add to the puid column in the results table and in the patient data table.
UPDATE tEyeResults r JOIN tPatientIDs p ON p.uid = r.patientId SET puid = p.keyid; 
UPDATE tPatientData prd JOIN tPatientIDs p ON p.uid = prd.uid SET puid = p.keyid;

-- Deleting the NULL entries. Uncomment to enable
DELETE FROM tEyeResults WHERE puid IS NULL;

-- Once the information is saved thoruhg the puid column, the patientid column is removed.
ALTER TABLE tEyeResults DROP COLUMN patientid;
ALTER TABLE tPatientData DROP COLUMN uid;

-- Adding stuff to the institution table
ALTER TABLE tInstitution ADD firstname           VARCHAR(255);  
ALTER TABLE tInstitution ADD lastname            VARCHAR(255);
ALTER TABLE tInstitution ADD address             VARCHAR(255);
ALTER TABLE tInstitution ADD phone               VARCHAR(255);
ALTER TABLE tInstitution ADD email               VARCHAR(255);

-- Moving the corresponding data from the institution table to the tPlacedProducts table
INSERT INTO tPlacedProducts (institution, etbrand, etmodel, etserial) SELECT uid, etbrand, etmodel, etserial FROM tInstitution;

-- Removing the redudant columns from the institution
ALTER TABLE tInstitution DROP COLUMN etbrand;
ALTER TABLE tInstitution DROP COLUMN etmodel;
ALTER TABLE tInstitution DROP COLUMN etserial;
