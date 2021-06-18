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
   qc_parameters            TEXT
);


CREATE TABLE evaluations (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   portal_user              INT UNSIGNED,
   study_type               VARCHAR(255),
   study_date               DATE,
   processing_date          DATE DEFAULT CURRENT_TIMESTAMP,
   institution_id           INT UNSIGNED,
   institution_instance     INT UNSIGNED,
   subject_id               VARCHAR(255),
   evaluator_email          VARCHAR(255),
   evaluator_name           VARCHAR(255),
   evaluator_lastname       VARCHAR(255),
   protocol                 VARCHAR(255),
   qc_parameters            LONGTEXT,
   qc_graphs                LONGTEXT,   
   results                  LONGTEXT  
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
   permissions              LONGTEXT,
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
   institution_id            INT UNSIGNED,
   instance_number           INT UNSIGNED,
   unique_id                 VARCHAR(40),
   gender                    VARCHAR(10),
   lastest                   TINYINT UNSIGNED DEFAULT 0
);

CREATE TABLE portal_users (
   keyid                    INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
   name                     VARCHAR(255),
   lastname                 VARCHAR(255),
   email                    VARCHAR(255),
   passwd                   VARCHAR(255),
   company                  VARCHAR(255),
   creation_token           VARCHAR(512),
   creation_date            TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   token                    VARCHAR(512),
   token_expiration         TIMESTAMP,
   permissions              LONGTEXT,
   user_role                TINYINT,
   enabled                  TINYINT DEFAULT 1,
   unique(email)
);

