import sys;
import hashlib as sha3;
tohash = sys.argv[1];
print(sha3.sha3_512(tohash.encode('utf-8')).hexdigest());
