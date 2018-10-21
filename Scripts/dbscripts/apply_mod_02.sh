#!/bin/bash 
mysql -u vmdb -p"vmdb" < develop_dump.sql;
mysql -u vmdb -p"vmdb" < mod02.sql;
