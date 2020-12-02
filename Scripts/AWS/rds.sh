#!/bin/bash

if [[ $1 == "patdata" ]]; then
   mysql --defaults-extra-file=CNF/vm-patdata.cnf
elif [[ $1 == "dash" ]]; then
   mysql --defaults-extra-file=CNF/vm-dashboard.cnf
elif [[ $1 == "id" ]]; then
   mysql --defaults-extra-file=CNF/vm-id.cnf
elif [[ $1 == "data" ]]; then
   mysql --defaults-extra-file=CNF/vm-data.cnf
else
   echo "Do not know $1"
   exit;
fi
