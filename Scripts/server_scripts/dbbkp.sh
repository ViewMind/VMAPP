 #!/bin/bash

if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

# Server type parameters
if [[ $1 == "prod" ]]; then
   DNS="18.220.30.34"
   USER="ec2-user"
   DEST="./bkpProd"
   DB_NAME="viewmind_data"
   PREFIX="prod"
elif [[ $1 == "dev" ]]; then
   DNS="18.191.142.5"
   USER="ec2-user"
   DEST="./bkpDev"
   declare -a DB_NAMES=("viewmind_develop" "viewmind_id_dev" "viewmind_patdata_dev")
   declare -a HOST_NAMES=("viewminddb" "viewmind-id" "viewmind-patdata")   
   declare -a FILE_NAMES=("res" "id" "patdata")
   PREFIX="dev"
else
   echo "Only options are prod or dev"
fi

clear

# MYSQL PARAMETERS
DB_BASE_HOST=".cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
DB_PORT="3306"
DB_USER="root"
DB_PASS="vimiroot"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

# The base backup name
DB_BKP=$PREFIX"_$(date +"%Y_%m_%d_%H_%M").sql"
LOCALPASS="vmdb"
LOCALUSER="vmdb"

total=${#DB_NAMES[@]}
for (( i=0; i<$total; i++ )); do
   
   DB_NAME=${DB_NAMES[$i]}   
   FNAME=${FILE_NAMES[$i]}   
   FNAME=$FNAME"_"$DB_BKP
   DB_HOST=${HOST_NAMES[$i]}   
   DB_HOST=$DB_HOST$DB_BASE_HOST
   
   CREATEBKP="mysqldump -h $DB_HOST -P $DB_PORT -u $DB_USER -p\"$DB_PASS\" --databases $DB_NAME > $FNAME"
   DELETEBKP="rm $FNAME"
   
   echo ">> CREATING THE BKP For $DB_NAME named: $FNAME"
   ssh -i $KEY $USER@$DNS "$CREATEBKP"
   # Copying it here
   echo ">> COPYING THE BKP: $FNAME to $DEST"
   scp -i $KEY $USER@$DNS:$FNAME $DEST
   echo ">> DELETING THE ORIGINAL BKP"
   ssh -i $KEY $USER@$DNS "$DELETEBKP"
   echo ">>>>>>>>>>>>>>>>>>>>>>>>>>> DONE!"
   
   # Restoring to the local database.
   echo ">> RESTORING TO BKP: $FNAME TO LOCAL DB: $DB_NAME"
   mysql -u $LOCALUSER -p"$LOCALPASS" $DB_NAME < $DEST/$FNAME  
   
done

