DROP DATABASE IF EXISTS vm_main;

CREATE DATABASE vm_main;
USE vm_main;

CREATE TABLE institution (
   keyid                     INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   institution_name          VARCHAR(255),
   email                     VARCHAR(255),
   address                   VARCHAR(255),
   state_or_province         VARCHAR(255),
   country                   VARCHAR(255),
   postal_code               VARCHAR(255),
   enabled                   TINYINT DEFAULT 1,
   phone_number              VARCHAR(20)
);


CREATE TABLE portal_users (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   name                     VARCHAR(255),
   lastname                 VARCHAR(255),
   email                    VARCHAR(255),
   passwd                   VARCHAR(255),
   creation_token           TEXT,
   creation_date            TIMESTAMP DEFAULT CURRENT_DATE,
   user_role                TINYINT DEFAULT 0,
   enabled                  TINYINT DEFAULT 1,
   unique(email)
);

CREATE TABLE institution_users (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   portal_user              INT UNSIGNED,
   institution_id           INT UNSIGNED,
   CONSTRAINT user_institution UNIQUE (portal_user,institution_id)
);

CREATE TABLE product_processing_parameters (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   product                  VARCHAR(255),
   processing_parameters    TEXT,
   frequency_parameters     TEXT
);


CREATE TABLE evaluations (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   portal_user              INT UNSIGNED,
   study_id                 VARCHAR(255),
   study_date               DATE,
   processing_date          DATE DEFAULT CURRENT_DATE,
   subject_id               INT UNSIGNED,
   protocol                 VARCHAR(255),
   frequency_error_flag     TINYINT DEFAULT 0,
   results                  LONGTEXT
);   

CREATE TABLE frequency_analysis_data (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   frequency_parameters     LONGTEXT,
   no_points_set_count      INT UNSIGNED,
   gliched_set_count        INT UNSIGNED,
   frequency_error_flag     TINYINT DEFAULT 0,
   eye_fixation_counts      LONGTEXT,
   filename                 VARCHAR(255),
   evaluation_id            INT UNSIGNED
);

DROP DATABASE IF EXISTS vm_secure;
CREATE DATABASE vm_secure;
USE vm_secure;

CREATE TABLE placed_products (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   product_id               VARCHAR(255),
   product_description      VARCHAR(255),
   institution_id           INT UNSIGNED,    
   institution_instance     INT UNSIGNED,   
   enabled                  TINYINT DEFAULT 1,
   status                   TINYINT DEFAULT 0,
   enable_token             VARCHAR(255)
);

CREATE TABLE secrets (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   secret_key               VARCHAR(512),
   institution_id           INT UNSIGNED,    
   institution_instance     INT UNSIGNED,
   secret                   VARCHAR(512),
   enabled                  TINYINT DEFAULT 1
);

CREATE TABLE subjects (
   keyid                     INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   name                      VARCHAR(255),
   last_name                 VARCHAR(255),
   birth_country_code        VARCHAR(2),
   birth_date                DATE,
   age                       TINYINT UNSIGNED,
   years_formation           TINYINT UNSIGNED,
   internal_id               VARCHAR(255),
   local_id                  VARCHAR(255),
   portal_user               INT UNSIGNED,
   institution_id            INT UNSIGNED
);

CREATE TABLE API_users (
   keyid                     INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   name                      VARCHAR(255),
   last_name                 VARCHAR(255),   
   email                     VARCHAR(255),
   company                   VARCHAR(255),
   created                   TIMESTAMP DEFAULT CURRENT_DATE,
   username                  VARCHAR(255),
   pass_word                 VARCHAR(255),
   token                     VARCHAR(512),
   token_expiration          TIMESTAMP,
   permissions               LONGTEXT
);
