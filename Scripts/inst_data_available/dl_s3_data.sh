#!/bin/bash 

LOCAL=$1
OUTPUT_REPO=$2
S3DIR=$3

if [[ $LOCAL == 1 ]]; then
else
   mkdir -f $OUTPUT_REPO/$S3DIR
   aws s3 cp s3://viewmind-raw-eye-data/$S3DIR $OUTPUT_REPO/$S3DIR --recursive
fi
