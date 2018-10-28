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
   DB_NAME="viewmind_develop"
   PREFIX="dev"
else
   echo "Only options are prod or dev"
fi

# Key
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
LOCALPASS="vmdb"
LOCALUSER="vmdb"

# MYSQL PARAMETERS
DB_HOST="viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com"
DB_PORT="3306"
DB_USER="root"
DB_PASS="vimiroot"

# The backup
DB_BKP=$PREFIX"_$(date +"%Y_%m_%d_%H_%M").sql"

CREATEBKP="mysqldump -h $DB_HOST -P $DB_PORT -u $DB_USER -p\"$DB_PASS\" --databases $DB_NAME > $DB_BKP"
DELETEBKP="rm $DB_BKP"

# Creating the BKP
echo ">> CREATING THE BKP"
ssh -i $KEY $USER@$DNS "$CREATEBKP"

# Copying it here
echo ">> COPYING THE BKP"
scp -i $KEY $USER@$DNS:$DB_BKP $DEST
   
# Removing the BKP from the server
echo ">> DELETING THE ORIGINAL BKP"
ssh -i $KEY $USER@$DNS "$DELETEBKP"

# Restoring to the local database.
echo ">> RESTORING TO BKP TO LOCAL DB"
mysql -u $LOCALUSER -p"$LOCALPASS" $DB_NAME < $DEST/$DB_BKP
