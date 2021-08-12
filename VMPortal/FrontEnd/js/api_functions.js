
API = {

  APICall: function(endpoint,callback,isBlob) {
     var xhr = new XMLHttpRequest();
     xhr.open('POST', endpoint, true);
     
     xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTH_TYPE,
                          GLOBALS.HEADERS_VALUES.PARTNER);
 
     var auth_value = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID)  
     + ":" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN)
  
     xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTHORIZATION,auth_value);

     var get_blob = false;
         
     if (isBlob !== undefined){
       //console.log("isBlob is not undefined.")
        if (isBlob === true){
            get_blob = true;
            //console.log("Setting get_blob to true")
        }
     }

     if (get_blob){
        xhr.responseType = "blob";
        xhr.onload = function (e) {
          if (this.status == 200){
             var blob = e.currentTarget.response;
             var contentDispo = e.currentTarget.getResponseHeader('Content-Disposition');
             ///// https://stackoverflow.com/a/23054920/
             var fileName = contentDispo.match(/filename[^;=\n]*=((['"]).*?\2|[^;\n]*)/)[1];
             //console.log("Filename is: " + fileName);
             var response = {};
             response.message = "OK";
             response.data = blob;
             response.filename = fileName;
             window[callback](response)
          }
          else{
            var response = {};
            response.message = "Something went wrong while retrieving the PDF";
            window[callback](response)
          }
      }        
     }
     else{
        xhr.responseType = "json";
        xhr.onreadystatechange = function() {
          if (xhr.readyState == 4) {
             window[callback](this.response)
          }
        };
     }
     
     xhr.send(null);
  },

  listInstitutionsForUser: function(callback){
  
     var endopoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.INSTITUTION.LIST + "/" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID);
     API.APICall(endopoint,callback);
  },

  listPatientsForInstitutionForUser: function(institution_id, callback){

     var endopoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.SUBJECT.LIST + "/" +  institution_id;
     
     API.APICall(endopoint,callback);
  },

  listReportsForSubject: function(subject_id, callback){

    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    GLOBALS.ENDPOINTS.REPORTS.LIST + "/" + subject_id;

    API.APICall(endpoint,callback);

  },

  getReport: function (report_id, lang, callback){
    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    GLOBALS.ENDPOINTS.REPORTS.GET + "/" + report_id + "?lang=" + lang;
    //GLOBALS.ENDPOINTS.REPORTS.GET + "/" + report_id;
    API.APICall(endpoint,callback,true);
  },

  modifyAccount: function(parameters,callback){
    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    GLOBALS.ENDPOINTS.PORTAL_USER.MODIFY_OWN + "/" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID) + "?";

    var url_params = [];
    for (p in parameters){
       url_params.push(p + "=" + parameters[p]);
    }
    endpoint = endpoint + url_params.join("&");
    //console.log(endpoint);

    API.APICall(endpoint,callback);

  }


}