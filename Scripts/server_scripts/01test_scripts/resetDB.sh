#!/bin/bash

############## ACTUAL PATCH LOGIC #################

BASE_HOST_NAME=""
DB_RES_HOST="localhost"
DB_ID_HOST="localhost"
DB_PATDATA_HOST="localhost"
PASSWD="givnar"
DB_RES_NAME="viewmind_data";
DB_ID_NAME="viewmind_id";
DB_PATDATA_NAME="viewmind_patdata";  
USER="root"
PORT=3306

# Setting up the password access
export MYSQL_PWD=$PASSWD

echo ">> APPLYING PATCH 05"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME -e "DELETE FROM tDoctors"
mysql -h $DB_ID_HOST -u $USER -P $PORT --database=$DB_ID_NAME -e "DELETE FROM tPatientIDs"
mysql -h $DB_PATDATA_HOST -u $USER -P $PORT --database=$DB_PATDATA_NAME -e "DELETE FROM tPatientData"

echo "Finished"
