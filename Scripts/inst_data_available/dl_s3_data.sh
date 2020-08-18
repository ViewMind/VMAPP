#!/bin/bash 

LOCAL=$1
OUTPUT_REPO=$2
S3DIR=$3

if [[ $LOCAL == 1 ]]; then
   REMOTE_TEMPDIR="/home/ec2-user/viewmind_projects/Scripts/inst_data_available/LOCALTESTS"
   KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
   DNS="18.220.30.34"
   USER="ec2-user"
   
   DL_IN_REMOTE="aws s3 cp s3://viewmind-raw-eye-data/$S3DIR $REMOTE_TEMPDIR/$S3DIR --recursive"   
   ssh -i $KEY $USER@$DNS "$DL_IN_REMOTE"   
   
   # Copying it locally. 
   echo "OUTPUT_REPO is $OUTPUT_REPO"
   scp -i $KEY -r $USER@$DNS:$REMOTE_TEMPDIR/$S3DIR $OUTPUT_REPO
   
else
   aws s3 cp s3://viewmind-raw-eye-data/$S3DIR $OUTPUT_REPO/$S3DIR --recursive
fi
