#### CONFIGURATION VARIABLES
LOCAL       = 1;
DEV         = 1;

######################## IMPLEMENTATION ########################

import _mysql;

# Local Parameters.
if (LOCAL):
   host     = "localhost";
   user     = "root";
   password = "givnar";
else:
   # Remote parameters:
   host     = "viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com";
   user     = "root";
   password = "vimiroot";

if (DEV):
   database = "viewmind_develop";
else:
   database = "viewmind_data";

port     = 3306;

db=_mysql.connect(host=host,user=user,passwd=password,port=port,db=database);

# Creating a new temporary column
db.query("SELECT keyid FROM tPlacedProducts");
rows = db.store_result();

while 1:
    res = rows.fetch_row();
    if (res): 
       keyid = str(res[0][0]);       
       uid = keyid;
       while (len(uid) < 10):
           uid = "0" + uid;
       uid = "VM" + uid;
       #print("UID is: " + uid  + " with Keyid: " + keyid + ". SHA3-512: " + s);
       db.query("UPDATE tPlacedProducts SET product_serialnum = '" + uid + "' WHERE keyid = " + keyid);
    else:
       break;
       
print("Finished");