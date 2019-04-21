#!/bin/bash

############## ACTUAL PATCH LOGIC #################

if [[ $1 == "dev" ]] || [[ $1 == "prod" ]]; then
   BASE_HOST_NAME=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
   DB_RES_HOST="viewminddb$BASE_HOST_NAME"
   DB_ID_HOST="viewmind-id$BASE_HOST_NAME"
   DB_PATDATA_HOST="viewmind-patdata$BASE_HOST_NAME"
   PASSWD="vimiroot"
else
   BASE_HOST_NAME=""
   DB_RES_HOST="localhost"
   DB_ID_HOST="localhost"
   DB_PATDATA_HOST="localhost"
   PASSWD="givnar"
fi

if [[ $1 == "dev" ]] || [[ $1 == "ldev"  ]]; then
   DB_RES_NAME="viewmind_develop";
   DB_ID_NAME="viewmind_id_dev";
   DB_PATDATA_NAME="viewmind_patdata_dev";
   
else
   DB_RES_NAME="viewmind_data";
   DB_ID_NAME="viewmind_id";
   DB_PATDATA_NAME="viewmind_patdata";  
fi

USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 09"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "ALTER TABLE tEyeResults ADD COLUMN ferror TINYINT DEFAULT 0"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "DROP TABLE IF EXISTS tFData"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "CREATE TABLE tFData (
                                                                        keyid               INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
                                                                        freq_tol_params     VARCHAR(255),
                                                                        no_data_set_count   INT,
                                                                        glicthed_set_count  INT,
                                                                        ferror              TINYINT,
                                                                        eye_fixation_counts VARCHAR(6000),
                                                                        filename            VARCHAR(255),
                                                                        teye_result         INT
                                                                    );"
echo "Finished"
