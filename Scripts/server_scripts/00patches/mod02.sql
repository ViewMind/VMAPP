DROP TABLE IF EXISTS tInstitution;

CREATE TABLE tInstitution (
    keyid              INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
    uid                INT UNIQUE,
    name               VARCHAR(255),
    hashpass           VARCHAR(255),
    evaluations        INT,
    etbrand            VARCHAR(255),
    etmodel            VARCHAR(255),
    etserial           VARCHAR(255)
);

ALTER TABLE tDoctors MODIFY COLUMN medicalinstitution INT;
UPDATE tDoctors SET medicalinstitution = 0;
