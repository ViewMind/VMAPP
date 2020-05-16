DROP TABLE IF EXISTS tSuppliers;
DROP TABLE IF EXISTS tPurchases;
DROP TABLE IF EXISTS tStrings;
DROP TABLE IF EXISTS tStockMP;
DROP TABLE IF EXISTS tProduction;
DROP TABLE IF EXISTS tTechnicalAssistance;
DROP TABLE IF EXISTS tComplaints;
DROP TABLE IF EXISTS tInstitutions;
DROP TABLE IF EXISTS tEvaluations;

-- Custom strings table
CREATE TABLE tStrings (     
                            keyid                 INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            table_column          VARCHAR(255),
                            string                VARCHAR(255)
                           );

-- The supliers table. 
CREATE TABLE tSuppliers (
                            keyid                 INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            supplier              VARCHAR(255),
                            country               VARCHAR(255),
                            supplier_id           VARCHAR(255),
                            contact_name          VARCHAR(255),
                            phone_number          VARCHAR(255),
                            email                 VARCHAR(255),
                            enabled               TINYINT,
                            mod_last              TIMESTAMP,
                            mod_user              VARCHAR(255)
                           );
                           
-- The purchase table
CREATE TABLE tPurchases (
                            keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            product_type                 INT,
                            quantity                     INT,
                            brand                        VARCHAR(255),
                            lot                          VARCHAR(255),
                            model                        VARCHAR(255),
                            serial_number                VARCHAR(255),
                            supplier                     VARCHAR(255),
                            entrance_date                DATE,
                            invoice_number               VARCHAR(255),                            
                            admin_accepted               TINYINT,
                            admin_rejection_reason       INT,
                            technical_accepted           TINYINT,     
                            technical_rejection_reason   INT,          
                            controled_by                 VARCHAR(255),             
                            control_date                 DATE,
                            purchase_id                  INT,
                            mod_last                     TIMESTAMP,
                            mod_user                     VARCHAR(255)
                           );
                           
-- The Stock MP Table
CREATE TABLE tStockMP (
                       keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                       purchase_id                  INT,
                       inuse_product                VARCHAR(255),
                       disabled                     TINYINT
);

-- The Production Table
CREATE TABLE tProduction (
                            keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            product_sn                   VARCHAR(255),
                            linked_institution           INT,
                            production_date              DATE,
                            product                      INT,
                            software_version             VARCHAR(255),
                            et_software_version          INT,
                            pc_configuration             TINYINT,
                            pc_configuration_rejection   INT,
                            et_install                   TINYINT,
                            et_install_rejection         INT,
                            eyeexp_install               TINYINT,
                            eyeexp_install_rejection     INT,
                            manual_version               INT,
                            final_control                TINYINT,
                            final_control_rejection      INT,                           
                            labelling                    TINYINT,
                            packaging                    TINYINT,
                            final_release                TINYINT,
                            observations                 VARCHAR(255),             
                            mod_last                     TIMESTAMP,
                            mod_user                     VARCHAR(255)                         
);

-- The technical assistance table
CREATE TABLE tTechnicalAssistance (
                            keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            product_sn                   VARCHAR(255),
                            arrival_date                 DATE,
                            received_by                  VARCHAR(255),
                            institution                  INT,
                            problem_found                INT,
                            cause                        INT,
                            solution                     VARCHAR(255),
                            resolution_date              DATE,
                            user_in_charge               VARCHAR(255),
                            assist_uid                   VARCHAR(255),
                            extra_details                VARCHAR(255),
                            mod_last                     TIMESTAMP,
                            mod_user                     VARCHAR(255)
);

-- The complaints table
CREATE TABLE tComplaints (
                            keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            arrival_date                 DATE,
                            institution                  INT,
                            product_sn                   VARCHAR(255),
                            complaint                    INT,
                            complaint_description        VARCHAR(255),
                            inmmediate_action            INT,
                            action_date                  DATE,
                            received_by                  VARCHAR(255),
                            internal_analysis            VARCHAR(255),
                            action_description           VARCHAR(255),
                            user_in_charge               VARCHAR(255),
                            complaint_ID                 VARCHAR(255),
                            recall_ID                    VARCHAR(255),
                            notification_ID              VARCHAR(255),
                            mod_last                     TIMESTAMP,
                            mod_user                     VARCHAR(255)
);

-- The instituions table
CREATE TABLE tInstitutions (
                            keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            uid                          INT,
                            name                         VARCHAR(255),
                            firstname                    VARCHAR(255),     
                            lastname                     VARCHAR(255),     
                            address                      VARCHAR(255),     
                            phone                        VARCHAR(255),     
                            email                        VARCHAR(255),     
                            enabled                      TINYINT,
                            mod_last                     TIMESTAMP,
                            mod_user                     VARCHAR(255)
);

-- The Evaluation Table
CREATE TABLE tEvaluations (
                           keyid                        INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                           institution                  INT,
                           evaluations                  INT,
                           UNIQUE KEY (institution, evaluations)
);

INSERT INTO `tInstitutions` VALUES (1,1145868706,'Axis - Bahia Blanca','David Alejandro','Orozco','San Martin 752, BahiÂ­a Blanca, Buenos Aires','+5492914023913','ddaaorozco@gmail.com',1,'2020-05-10 18:14:47','ariela'),(2,1242673082,'Strathclyde','Mario','Parra','50 George St ,Graham Hills Building, Universyty of Strathclyde, Glagow, Scotland, UK. ZipCode G1 1QE','+447854483968','mario.parra-rodriguez@strath.ac.uk',1,'2020-05-10 18:14:47','ariela'),(3,740292426,'Sanatorio de los Arcos','Maria Alejandra','Amengual','Av. Juan B Justo 909, Sanatorio de los Arcos, Ciudad Autonoma de Buenos Aires, Argentina','47784613','MariaAlejandra.Amengual@swissmedical.com.ar',0,'2020-05-10 18:14:47','ariela'),(4,1369462188,'ViewMind','Matias','','','','',1,'2020-05-10 18:14:47','ariela'),(5,0,'Test Institution','Ariel','Arelovich','','','aarelovich@gmail.com',1,'2020-05-10 18:14:47','ariela'),(6,1771608688,'Fundacion Fleni','Salvador','Guinjoan','Montaneses 2325, Ciudad Autonoma de Buenos Aires','+541157773200 (INT 2514)','sguinjoan@fleni.org.ar',0,'2020-05-10 18:14:47','ariela'),(7,1105849094,'Fundacion Universidad de Antioquia','Luis guillermo','Mendez Barrera','Sede de Investigacion Universitaria SIU Calle 62 No. 52 - 59','+5742196664','luis.mendez48@gmail.com',1,'2020-05-10 18:14:47','ariela'),(8,989302458,'Novartis','Jelena','Curcic','NIBR Building, Novartis, Basel, Switzaerland','+ 41 79 500 36 48','jelena.curcic@novartis.com',1,'2020-05-10 18:14:47','ariela'),(9,1083582946,'Universidad Complutense de Madrid','Sara Fernandez','Guinea','','','',1,'2020-05-10 18:14:47','ariela');

-- Adding the history columns to the User table. 
ALTER TABLE tUsers ADD mod_last TIMESTAMP;
ALTER TABLE tUsers ADD mod_user VARCHAR(255);
DROP INDEX username ON tUsers;
UPDATE tUsers SET mod_last = NOW(), mod_user='ariela'; 
