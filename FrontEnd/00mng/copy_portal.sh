#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Need to run as root"
  exit
fi

DEST=/home/web/vmportal
SRC=../
APACHE_USER="www-data"

declare -a EXCLUDE=("00mng" "LOGS")
declare -a FILES_TO_DELETE=()

LIST=($(ls $SRC))

total=${#LIST[@]}

echo "Copying $total items"

for (( i=0; i<$total; i++ )); do
   item=${LIST[$i]}
   
   if [[ " ${EXCLUDE[@]} " =~ " ${item} " ]]; then
       continue;
   fi   

   #cp -r $SRC/$item $DEST/$item
   cp -r $SRC/$item $DEST/
   
done

echo "Deleting files from list"

total=${#FILES_TO_DELETE[@]}

for (( i=0; i<$total; i++ )); do
   item=${FILES_TO_DELETE[$i]}
   item=$DEST/$item
   rm -f $item
done

echo "Switching groups to apache user"
chgrp -R $APACHE_USER $DEST

echo "Adding write permissions to apache user"
chmod -R g+w $DEST






