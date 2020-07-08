#!/bin/bash

## Production IP
DNS="18.220.30.34"
USER="ec2-user"
KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"

# Command to downlaod an S3 folder and then copy it locally
scp -i $KEY -r $USER@$DNS:"/home/ec2-user/EyeServer/OUTPUTS/b62cafe7ac15d7b9c35734d08cfdf1f0a4cabc84c3a90c052c3d65bb47b739dc0af7c55b398bc6c95db60e02b2c4943bb04391803bc355bdb652d2f95ad880d8/2020_07_08_10_29_16/" .





