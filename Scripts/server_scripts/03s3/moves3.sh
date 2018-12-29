#!/bin/bash

function padTo10() {
   L=${#1}
   toPad=$(expr 10 - $L)
   ans=$1
   for ((c=0; c<$toPad; c++)); do
      ans=0"$ans"
   done
   echo $ans
}

clear
if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

export MYSQL_PWD="vimiroot"

if [[ $1 == "dev" ]]; then
   BUCKET="develop-testing"
   DB_ID_NAME="viewmind_id_dev"
elif [[ $1 == "prod" ]]; then
   BUCKET="viewmind-raw-eye-data"
   DB_ID_NAME="viewmind_id"
fi

# Listing the doctors.
drids=($(aws s3 ls s3://$BUCKET))

for i in "${drids[@]}"; do
   if [[ $i != "PRE" ]]; then
      echo "Patients for DR: $i"
      patids=($(aws s3 ls s3://$BUCKET/$i))
      for j in "${patids[@]}"; do
         if [[ $j != "PRE" ]]; then
            patid="${j::-1}"
            sha=$(python3 sha3_512.py $patid);
            #queryRes=($(mysql -h viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com -u root -P 3306 --database=$DB_ID_NAME -e "SELECT keyid FROM tPatientIDs WHERE uid = '$patid'"))
            #puid=${queryRes[1]}
            #puid=$(padTo10 $puid)
            echo "   PATID: $patid. SHA3 Result: $sha"
            aws s3 cp --recursive s3://$BUCKET/$i$j s3://$BUCKET/$sha
         fi
      done
   fi
done

