#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
BUCKET="viewmind-raw-eye-data"

DIR_TO_DL=$1
DIR_MOVE_TO=$2
BASE_COPY_DIR="temp_s3_dir"
DEST="/home/ec2-user/temp_s3_dir"

# echo "Copying $1 to $2";

# Command to downlaod an S3 folder and then copy it locally
ssh -i $KEY $USER@$DNS "mkdir $DEST; aws s3 cp --recursive s3://$BUCKET/$DIR_TO_DL $DEST/$DIR_TO_DL"
scp -i $KEY -r $USER@$DNS:"$DEST/$DIR_TO_DL" $DIR_MOVE_TO
ssh -i $KEY $USER@$DNS "rm -rf $DEST"


