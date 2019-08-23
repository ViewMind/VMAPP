# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import hashlib as sha3;

uid = "1105849094_0000_P0000"
s=sha3.sha3_512(uid.encode('utf-8')).hexdigest();
print("Str is: " + uid  + " . SHA3-512: " + s);