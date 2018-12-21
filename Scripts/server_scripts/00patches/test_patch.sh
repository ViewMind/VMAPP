#!/bin/bash

#################### CONFIG DATA: Starting bkp and patch to test. Avoids using parameters
DB_BKP="prod_2018_12_15_06_26.sql"
PATCH1="mod03_01_db01.sql"
PATCH2="mod03_01_db02.sql"

############## RESTORING TO CURRENT STATE #################
LOCALPASS="givnar"
LOCALUSER="root"
DBNAME="viewmind_data"
DEST="../bkpProd"

# Setting up the password access
export MYSQL_PWD=$LOCALPASS

# Restoring to the local database.
echo ">> RESTORING TO BKP TO LOCAL DB"
mysql -u $LOCALUSER < $DEST/$DB_BKP 


############## ACTUAL PATCH LOGIC #################

IDTABLE_DUMP="idtable.sql"
PATDATA_DUMP="patdatatable.sql"

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

if [[ $1 == "dev" ]]; then
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

echo ">> APPLYING PATCH $PATCH1"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME < $PATCH1

echo ">> DUMPING ID TABLE"
mysqldump -h $DB_RES_HOST -u $USER -P $PORT $DB_RES_NAME tPatientIDs > $IDTABLE_DUMP

echo ">> DUMPING PATIENT INFO TABLE"
mysqldump -h $DB_RES_HOST -u $USER -P $PORT $DB_RES_NAME tPatientData > $PATDATA_DUMP

echo ">> IMPORTING ID TABLE TO ID DATABASE"
mysql -h $DB_ID_HOST -u $USER -P $PORT $DB_ID_NAME < $IDTABLE_DUMP

echo ">> IMPORTING PATIENT DATA TABLE TO PATIENT DATA DATABASE"
mysql -h $DB_PATDATA_HOST -u $USER -P $PORT $DB_PATDATA_NAME < $PATDATA_DUMP

echo ">> APPLYING PATCH $PATCH2: REMOVING TABLES"
mysql -h $DB_RES_HOST -u $USER -P $PORT --database=$DB_RES_NAME < $PATCH2

echo ">> CLEAN UP"
rm -f $IDTABLE_DUMP
rm -f $PATDATA_DUMP
