#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

FILE_TO_GET="/home/ec2-user/EyeServer/OUTPUTS/97137928ac89bc6c20ea0323ea0d120a0a3a33decbae197a4fcd928b1b74c7317521b4c5523e4abadf7d847e43e01b976740c7070d948d5b18c65ac4748327ca/2020_05_16_22_56_39/reading_es_2_2020_05_09_10_12.csv";

# Command to downlaod an S3 folder and then copy it locally
scp -i $KEY $USER@$DNS:"$FILE_TO_GET" .




