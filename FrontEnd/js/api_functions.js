

function listInstitutionsForUser(){

   var xhr = new XMLHttpRequest();
   var endopoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
   GLOBALS.ENDPOINTS.INSTITUTION.LIST + "/" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID);
   xhr.open('POST', endopoint, true);
   
   xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTH_TYPE,
                        GLOBALS.HEADERS_VALUES.PARTNER);

   var auth_value = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID)  
   + ":" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN)

   //console.log("Setting the auth value to " + auth_value);

   xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTHORIZATION,auth_value);
   
   xhr.responseType = "json";
   
   xhr.onreadystatechange = function() {
       if (xhr.readyState == 4) {
         console.log(JSON.stringify(this.response));
         //  if ("data" in this.response){
         //     Token.setToken(this.response.data["token"]);
         //     window.location.href = "index.html"
         //     sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER,username);
         //     sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_ID,this.response.data["id"]);
         //  }
         //  else{
         //     // Show message error response.message. 
         //  }
       }
   };
   xhr.send(null);   

}