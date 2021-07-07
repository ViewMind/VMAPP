
API = {

  APICall: function(endpoint,callback) {
     var xhr = new XMLHttpRequest();
     xhr.open('POST', endpoint, true);
     
     xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTH_TYPE,
                          GLOBALS.HEADERS_VALUES.PARTNER);
 
     var auth_value = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID)  
     + ":" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN)
  
     xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTHORIZATION,auth_value);
     
     xhr.responseType = "json";
     
     xhr.onreadystatechange = function() {
         if (xhr.readyState == 4) {
          window[callback](this.response)
         }
     };
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

  getReport: function (report_id, callback){
    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    GLOBALS.ENDPOINTS.REPORTS.GET + "/" + report_id;
    API.APICall(endpoint,callback);
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