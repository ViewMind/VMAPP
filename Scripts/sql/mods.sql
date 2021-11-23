--------------------------------- 08/07/2021 ---------------------------------
ALTER TABLE evaluations ADD COLUMN study_configuration TEXT;

--------------------------------- 13/07/2021 ---------------------------------
--vm_main
CREATE TABLE updates (
   keyid                  INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   institution_id         INT UNSIGNED,
   institution_instance   INT UNSIGNED,
   unique_id              VARCHAR(50),                         
   creation_date          TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   version_string         VARCHAR(50),
   eyetracker_key         VARCHAR(50),
   valid                  TINYINT DEFAULT 1
)

--vm_secure
ALTER TABLE secrets ADD COLUMN unique_id VARCHAR(50);
UPDATE secrets SET unique_id = CONCAT(institution_id,".",institution_instance);


--------------------------------- 13/07/2021 ---------------------------------
-- vm_secure
ALTER TABLE portal_users ADD COLUMN partner_id VARCHAR(100) AFTER permissions;

--------------------------------- 13/07/2021 ---------------------------------
-- vm_main
ALTER TABLE evaluations ADD COLUMN discard_reason VARCHAR(100) AFTER file_link

--------------------------------- 25/10/2021 ---------------------------------
--vm_secure
CREATE TABLE app_password_recovery (
   keyid                  INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   institution_id         INT UNSIGNED,
   institution_instance   INT UNSIGNED,
   unique_id              VARCHAR(50),
   password_hash          VARCHAR(255)
)

--------------------------------- 27/10/2021 ---------------------------------
--vm_secure
CREATE TABLE medical_records (
   keyid                  INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   first_name             VARCHAR(100),
   last_name              VARCHAR(100),
   unique_id              VARCHAR(100),
   viewmind_id            VARCHAR(40),
   institution_id         INT UNSIGNED,
   subject_link           VARCHAR(40),
   record                 LONGTEXT,
   last_mod_by            VARCHAR(255),
   last_update            TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   valid                  TINYINT DEFAULT 0   
);

--------------------------------- 17/11/2021 ---------------------------------
--vm_main
CREATE TABLE rate_limiting (
   keyid                  INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   ip                     VARCHAR(30),
   rate_limit             INT DEFAULT 0,
   requests               INT DEFAULT 0,
   time_frame_start       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   UNIQUE(ip)
);
