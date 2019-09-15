#!/bin/bash

############## ACTUAL PATCH LOGIC #################

if [[ $1 == "prod" ]]; then
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

DB_RES_NAME="viewmind_data";
DB_ID_NAME="viewmind_id";
DB_PATDATA_NAME="viewmind_patdata";  

USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 13"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "CREATE TABLE tMedicalRecords (
                                                                        keyid                 INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
                                                                        date                  DATE,
                                                                        puid                  INT,
                                                                        record_index          INT,
                                                                        presumptive_diagnosis TEXT,
                                                                        formative_years       INT,
                                                                        medication            LONGTEXT,
                                                                        RNM                   LONGTEXT,
                                                                        evaluations           LONGTEXT
                                                                      );"

mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN idtype;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN state;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN city;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN mainactivity;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN highestdegree;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN physicalactivity;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN nofspokenlang;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN agreesharedata;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN familyhistory;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN patienthistory;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN remedies;"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "ALTER TABLE tPatientData DROP COLUMN diagnostic;"

mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "ALTER TABLE tEyeResults ADD client_study_date DATETIME;"

                                                                      

echo "Finished"


