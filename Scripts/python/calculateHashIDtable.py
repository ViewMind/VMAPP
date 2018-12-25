#import sha3;
#data = "Hola Mundo Cruel"
##s=sha3.sha3_512(data.encode('utf-8')).hexdigest();
#print(s)
#
#tocompare = "e08ec7d57911ef831ccd0cdb9e1148faf3c53560a3695366af78254f91e5e017fc1627561b040497867cc5c3b7d7140a90106ae013ff50469197160582c2e2c4"
#
#print(len(tocompare))
#
#print("Same: " + str((tocompare == s)) )

#### CONFIGURATION VARIABLES
LOCAL=0
DEV  =1;

######################## IMPLEMENTATION ########################

import _mysql;

# Local Parameters.
if (LOCAL):
   host     = "localhost";
   user     = "root";
   password = "givnar";
else:
   # Remote parameters:
   host     = "viewmind-id.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";
   user     = "root";
   password = "vimiroot";

if (DEV):
   database = "viewmind_id_dev";
else:
   database = "viewmind_id";

port     = 3306;

db=_mysql.connect(host=host,user=user,passwd=password,port=port,db=database);
db.query("SELECT uid FROM tPatientIDs");
rows = db.store_result();

while 1:
    res = rows.fetch_row();
    if (res): 
       print(res);
    else:
       break;