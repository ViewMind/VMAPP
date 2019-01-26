#!/bin/bash


if [[ $1 == "prod" ]]; then
   BASE_HOST_NAME=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
   PASSWD="vimiroot"
else
   BASE_HOST_NAME=""
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

DBNAME="viewmind_dashboard_users"
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
