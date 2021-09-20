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