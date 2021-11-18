# #!/bin/bash
# 
# # // Values for the headers used. 
# # GLOBALS.HEADERS_VALUES =  {}
# # GLOBALS.HEADERS_VALUES.PARTNER = "VMPartner"
# # GLOBALS.HEADERS_VALUES.LOGIN   = "Login"
# # GLOBALS.HEADERS_VALUES.BASIC   = "Basic"
# 
# # // The names of the actual headers 
# # GLOBALS.HEADERS_NAMES = {}
# # GLOBALS.HEADERS_NAMES.AUTHORIZATION = "Authorization"
# # GLOBALS.HEADERS_NAMES.AUTH_TYPE = "AuthType"
# 
# 
# # Simple test just to try to authorize. We only care about seen the headers. 
# API="https://devapi.viewmind.net"
# USERNAME="ariel.arelovich@viewmind.ai"
# PASSWORD="mati1234"
# HEADER_AUTH_TYPE="AuthType"
# HEADER_AUTH_TYPE_VALUE="Login"
# 
# echo "Doing the request"
# curl -X POST --user $USERNAME:$PASSWORD --header "$HEADER_AUTH_TYPE: $HEADER_AUTH_TYPE_VALUE" $API
# 
# echo ""
# echo "Done"

for i in {1..100}
do
   echo "========================= ATTEMP $i ==============================="
   curl -i -s -k -X $'POST' \
       -H $'Host: devapi.viewmind.net' -H $'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0' -H $'Accept: */*' -H $'Accept-Language: es-AR,es;q=0.8,en-US;q=0.5,en;q=0.3' -H $'Accept-Encoding: gzip, deflate' -H $'Authtype: Login' -H $'Authorization: Basic anBhYmxvQHlacHBvLm5adDoxMjM=' -H $'Origin: https://devportal.viewmind.net' -H $'Referer: https://devportal.viewmind.net/' -H $'Sec-Fetch-Dest: empty' -H $'Sec-Fetch-Mode: cors' -H $'Sec-Fetch-Site: same-site' -H $'Content-Length: 0' -H $'Te: trailers' \
       $'https://devapi.viewmind.net/'
   echo ""
done

