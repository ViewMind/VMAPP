USE viewmind_data;

DROP TABLE IF EXISTS tPatients;
DROP TABLE IF EXISTS tPatientReqData;
DROP TABLE IF EXISTS tPatientOptionalData;
DROP TABLE IF EXISTS tDoctors;
DROP TABLE IF EXISTS tEyeResults;

CREATE TABLE tPatientReqData (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid                VARCHAR(255) UNIQUE,
    doctorid           VARCHAR(255),
    firstname          VARCHAR(255),
    lastname           VARCHAR(255),
    idtype             VARCHAR(255),
    sex                CHAR,
    birthcountry       VARCHAR(2),
    birthdate          DATE
);

CREATE TABLE tPatientOptionalData (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,     
    patientid          VARCHAR(255),    
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
    diagnostic         VARCHAR(255)    
);


CREATE TABLE tDoctors (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    uid                VARCHAR(255) UNIQUE,
    firstname          VARCHAR(255),
    lastname           VARCHAR(255),
    idtype             VARCHAR(255),
    countryid          VARCHAR(2),
    state              VARCHAR(255),
    city               VARCHAR(255),
    medicalinstitution VARCHAR(255),
    telephone          VARCHAR(30),
    email              VARCHAR(255),
    address            VARCHAR(255)
);

CREATE TABLE tEyeResults (
   keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
   patientid          VARCHAR(255),
   doctorid           VARCHAR(255),
   study_id           VARCHAR(255),
   study_date         DATETIME,
   rdTotalFixL        INT,
   rdFStepFixL        INT,
   rdMStepFixL        INT,
   rdSStepFixL        INT,
   rdTotalFixR        INT,
   rdFStepFixR        INT,
   rdMStepFixR        INT,
   rdSStepFixR        INT,
   bcAvgPupilR        FLOAT,
   ucAvgPupilR        FLOAT,
   bcAvgPupilL        FLOAT,
   ucAvgPupilL        FLOAT,  
   bcGazeResR         FLOAT,
   ucGazeResR         FLOAT,
   bcGazeResL         FLOAT,
   ucGazeResL         FLOAT,
   bcCorrectAns       TINYINT,
   ucCorrentAns       TINYINT,
   bcWrongAns         TINYINT,
   ucWrongAns         TINYINT,
   bcTestCorrectAns   TINYINT,
   ucTestCorrectAns   TINYINT,   
   bcTestWrongAns     TINYINT,
   ucTestWrongAns     TINYINT,      
   fdHitR             INT,
   fdHitL             INT,
   fdHitTotal         INT   
);
