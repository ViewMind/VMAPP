 #!/bin/bash

if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

# Server type parameters
if [[ $1 == "prod" ]]; then
   DBNAME_BASE="viewmind_data"
   DBNAME_ID="viewmind_id"
   DBNAME_PATDATA="viewmind_patdata"
   BUCKET="viewmind-raw-eye-data"
elif [[ $1 == "dev" ]]; then
   DBNAME_BASE="viewmind_develop"
   DBNAME_ID="viewmind_id_dev"
   DBNAME_PATDATA="viewmind_patdata_dev"
   BUCKET="develop-testing"
else
   echo "Only options are prod or dev"
fi

clear

# MYSQL PARAMETERS
DB_BASE="viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
DB_ID="viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com" 
DB_PATDATA="viewmind-patdata.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com" 
DB_PORT="3306"
DB_USER="root"
DB_PASS="vimiroot"

PATSHA3="5c0a9b846380effff34b3fc8ce5f2dc4c0e697f68290da3a87842da6c7efd25b166f17e07e90369aaa39345445bd098f2777de5054cf4d63801c54bc0f2e7a9e"

# Checking that the doctor was created

export MYSQL_PWD=$DB_PASS

echo ">> Checking if test doctor exists"
mysql -h $DB_BASE -P $DB_PORT -u $DB_USER --database=$DBNAME_BASE -e "SELECT * FROM viewmind_develop.tDoctors WHERE uid = 'ARvm0000'"

echo ">> Checking if test patient was created correctly "
mysql -h $DB_PATDATA -P $DB_PORT -u $DB_USER --database=$DBNAME_PATDATA -e "SELECT keyid, doctorid, firstname, lastname, idtype, sex , birthcountry, birthdate , date_insertion, state, city  FROM tPatientData  WHERE doctorid = 'ARvm0000'"
mysql -h $DB_ID -P $DB_PORT -u $DB_USER --database=$DBNAME_ID -e "SELECT * FROM tPatientIDs  WHERE uid = '$PATSHA3'"

echo ">> Checking for test results "
mysql -h $DB_BASE -P $DB_PORT -u $DB_USER --database=$DBNAME_BASE -e "SELECT keyid, study_id, study_date FROM tEyeResults  WHERE doctorid = 'ARvm0000'"

echo ">> Searrhing for test patient folder"
aws s3 ls s3://$BUCKET | grep $PATSHA3
