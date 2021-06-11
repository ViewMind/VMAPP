# Retrieve current IP address

export AWS_PROFILE=viewmind

SG='sg-039b13ab2438395a5'
DESC='ArielArelovich'
IP=`curl -s http://whatismyip.akamai.com/`

OLD_IP=$(aws ec2 describe-security-groups --group-id $SG --query "SecurityGroups[0].IpPermissions[0].IpRanges[?Description=='${DESC}'].CidrIp" --output text)
#echo "$OLD_IP"
aws ec2 revoke-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=22,ToPort=22,IpRanges="[{CidrIp=$OLD_IP,Description='${DESC}'}]"  --output text
aws ec2 authorize-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=22,ToPort=22,IpRanges="[{CidrIp=$IP/32,Description='${DESC}'}]"  --output text
# aws ec2 revoke-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=3306,ToPort=3306,IpRanges="[{CidrIp=$OLD_IP,Description='${DESC}'}]"  --output text
# aws ec2 authorize-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=3306,ToPort=3306,IpRanges="[{CidrIp=$IP/32,Description='${DESC}'}]"  --output text
# aws ec2 revoke-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=3307,ToPort=3307,IpRanges="[{CidrIp=$OLD_IP,Description='${DESC}'}]"  --output text
# aws ec2 authorize-security-group-ingress --group-id $SG --ip-permissions IpProtocol=tcp,FromPort=3307,ToPort=3307,IpRanges="[{CidrIp=$IP/32,Description='${DESC}'}]"  --output text
