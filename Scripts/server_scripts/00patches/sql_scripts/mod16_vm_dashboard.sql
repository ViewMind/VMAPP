CREATE TABLE tSuppliers (
                            keyid                 INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
                            supplier              VARCHAR(255),
                            country               VARCHAR(255),
                            supplier_id           VARCHAR(255),
                            contact_name          VARCHAR(255),
                            phone_number          INT,
                            email                 VARCHAR(255),
                            enabled               TINYINT
                           );
