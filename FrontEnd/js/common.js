// This will be the name space for any global constants or variables. 
var GLOBALS = {}; 

// Keys for sessionStorage. 
GLOBALS.SESSION_KEYS = {};
GLOBALS.SESSION_KEYS.EXPIRATION_TIME = "expiration_time" 
GLOBALS.SESSION_KEYS.EXPIRATION_DATE = "expiration_date"
GLOBALS.SESSION_KEYS.API             = "api"
GLOBALS.SESSION_KEYS.USER            = "username"
GLOBALS.SESSION_KEYS.USER_ID         = "user_id"
GLOBALS.SESSION_KEYS.TOKEN           = "token"

// Values for the headers used. 
GLOBALS.HEADERS_VALUES =  {}
GLOBALS.HEADERS_VALUES.PARTNER = "VMPartner"
GLOBALS.HEADERS_VALUES.LOGIN   = "Login"
GLOBALS.HEADERS_VALUES.BASIC   = "Basic"

// The names of the actual headers 
GLOBALS.HEADERS_NAMES = {}
GLOBALS.HEADERS_NAMES.AUTHORIZATION = "Authorization"
GLOBALS.HEADERS_NAMES.AUTH_TYPE = "AuthType"

// The endpoitns to be used. 
GLOBALS.ENDPOINTS = {}
GLOBALS.ENDPOINTS.INSTITUTION = {}
GLOBALS.ENDPOINTS.INSTITUTION.LIST = "institution/list";
GLOBALS.ENDPOINTS.REPORTS = {}
GLOBALS.ENDPOINTS.REPORTS.LIST = "reports/list";

// Gets global values from server.
function getConfigurationFromServer(){

   //var item = sessionStorage.getItem("something");
   //console.log("ITEM is: " + item);

   var xhr = new XMLHttpRequest();
   xhr.open('POST', "php/config.php", true);
   xhr.responseType = "json";
   xhr.onreadystatechange = function() {
      if (xhr.readyState == 4) {
         sessionStorage.setItem(GLOBALS.SESSION_KEYS.API ,this.response.API);
         sessionStorage.setItem(GLOBALS.SESSION_KEYS.EXPIRATION_TIME,this.response.EXPIRATION_TIME);
      }
   };
   xhr.send(null);   
}

// If the global values have not been loaded, they are. 
function checkConfig(){
   if (sessionStorage.getItem(GLOBALS.SESSION_KEYS.API ) === null){
      getConfigurationFromServer();
   }
}

/// Token management is encapsulated within these three function so that it may replaced easily in the future. 
var Token = {
   
   checkLogIn: function(){
      var ret = true;
      
      if (sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN) !== null){
         console.log("Checking date for current token");
         var now = new Date();
         var expiry = new Date(sessionStorage.getItem(GLOBALS.SESSION_KEYS.EXPIRATION_DATE));
         console.log("Comparing NOW: " + now.toString() + " to EXP: " + expiry.toString())
         if (expiry > now) ret = false;
         else ret = true;
      }
      else {
         console.log("TOKEN not set")
         ret = true;
      }
      if (ret) window.location.href = "login.html";
   },

   setToken: function (token){
      var minutes = sessionStorage.getItem(GLOBALS.SESSION_KEYS.EXPIRATION_TIME);
      //var minutes = 1;
      minutes = parseInt(minutes);
      console.log("Adding " + minutes + " minutes");
      var dt = new Date();
      console.log("Current date: " + dt.toString());
      dt.setMinutes( dt.getMinutes() + minutes);
      console.log("Expiration: "  + dt.toString());
      sessionStorage.setItem(GLOBALS.SESSION_KEYS.TOKEN,token);
      sessionStorage.setItem(GLOBALS.SESSION_KEYS.EXPIRATION_DATE,dt.toString())
      //console.log("SET EXPIRY TO: "  + dt.toString());
   },

   getToken: function(){
      return sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN);
   }

}