#!/bin/bash

# // Values for the headers used. 
# GLOBALS.HEADERS_VALUES =  {}
# GLOBALS.HEADERS_VALUES.PARTNER = "VMPartner"
# GLOBALS.HEADERS_VALUES.LOGIN   = "Login"
# GLOBALS.HEADERS_VALUES.BASIC   = "Basic"

# // The names of the actual headers 
# GLOBALS.HEADERS_NAMES = {}
# GLOBALS.HEADERS_NAMES.AUTHORIZATION = "Authorization"
# GLOBALS.HEADERS_NAMES.AUTH_TYPE = "AuthType"


# Simple test just to try to authorize. We only care about seen the headers. 
API="https://devapi.viewmind.net"
USERNAME="ariel.arelovich@viewmind.ai"
PASSWORD="mati1234"
HEADER_AUTH_TYPE="AuthType"
HEADER_AUTH_TYPE_VALUE="Login"

echo "Doing the request"
curl -X POST --user $USERNAME:$PASSWORD --header "$HEADER_AUTH_TYPE: $HEADER_AUTH_TYPE_VALUE" $API

echo ""
echo "Done"