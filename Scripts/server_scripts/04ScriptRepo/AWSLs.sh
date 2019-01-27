#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
BUCKET="develop-testing"

DIR_TO_DL=$1
DIR_MOVE_TO=$2
BASE_COPY_DIR="temp_s3_dir"
DEST="/home/ec2-user/temp_s3_dir"

# Command to downlaod an S3 folder and then copy it locally
ssh -i $KEY $USER@$DNS "aws s3 ls --recursive s3://$1 >> temp_output"
scp -i $KEY -r $USER@$DNS:"temp_output" $2
ssh -i $KEY $USER@$DNS "rm -f temp_output"



