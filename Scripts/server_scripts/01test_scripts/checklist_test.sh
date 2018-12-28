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
   PREFIX="prod"
elif [[ $1 == "dev" ]]; then
   DBNAME_BASE="viewmind_develop"
   DBNAME_ID="viewmind_id_dev"
   DBNAME_PATDATA="viewmind_patdata_dev"
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

PATSHA3="f2327a48c464ec671e83e68251b4f892fa9cbb6e65ab023f97c8b80fa9e7d2b9a889d3fb7e1230dd98e92b1b3162c5bb4a03a03ca6e1dfcc301fad8d8abfb770"

# Checking that the doctor was created

echo ">> Checking if test doctor exists"
mysql -h $DB_BASE -P $DB_PORT -u $DB_USER -p $DB_PASS --database=$DBNAME_BASE -e "SELECT * FROM viewmind_develop.tDoctors WHERE uid = 'ARvm0000'"

