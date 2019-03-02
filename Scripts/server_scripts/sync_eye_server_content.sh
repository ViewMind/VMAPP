 #!/bin/bash

if [[ $# != 1 ]]; then
   echo "Only one parameter required"
   exit
fi

KEY="/home/ariela/Dropbox/ViewMindData/awspem/aws_ec2_first_time.pem"
EYESERVER_EYEEXP_FOLDER="/home/ariela/repos/viewmind_projects/EyeServer/bin/eyeexp"
DEST="/home/ec2-user/EyeServer"

# Server type parameters
if [[ $1 == "prod" ]]; then
   DNS="18.220.30.34"
elif [[ $1 == "dev" ]]; then
   DNS="18.191.142.5"
else
   echo "Only options are prod or dev"
fi

scp -i $KEY -r $EYESERVER_EYEEXP_FOLDER ec2-user@$DNS:$DEST
