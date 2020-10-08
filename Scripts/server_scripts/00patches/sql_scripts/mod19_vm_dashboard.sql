DROP TABLE IF EXISTS tTrainedUsers;

-- Custom strings table
CREATE TABLE tTrainedUsers (     
                            keyid                 INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            profesion             VARCHAR(255),
                            name                  VARCHAR(255),
                            email                 VARCHAR(255),
                            is_trained            TINYINT,
                            birth_date            DATE,
                            mod_last              TIMESTAMP,
                            mod_user              VARCHAR(255)                            
                           );
