
API = {

   /**
    * 
    * @param {String} endpoint - The string containt the endpoint which is of the form object/operation?parameters
    * @param {String} callback - The string containing the name of the JS function to be called when the transaction completes
    * @param {Boolean} isBlob - Boolean. If true the response will await for a file (i.e a blob) if undefined or false then it's ignored.
    * @param {Object} json_data - If defined this will represent the data sent to the backend. 
    */

  APICall: function(endpoint,callback,isBlob,json_data) {
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
     
     let to_send = null;
     if (json_data !== undefined){
      // Making user only JS objects are sent 
      if (
          (typeof json_data === 'object') &&
          !Array.isArray(json_data) &&
          (json_data !== null)
         )
        {
           to_send = JSON.stringify(json_data);
        }
     }

     //console.log("Sending: " + to_send);

     xhr.send(to_send);
  },

  listInstitutionsForUser: function(callback){
  
     var endopoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.INSTITUTION.LIST + "/" + sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_ID);
     API.APICall(endopoint,callback);
  },

  listPatientsForInstitutionForUser: function(institution_id, callback){

     let permissions = JSON.parse(sessionStorage.getItem(GLOBALS.SESSION_KEYS.PERMISSIIONS));
     let list_endpoint = GLOBALS.ENDPOINTS.SUBJECT.LIST
     if (permissions[GLOBALS.PERMISSIIONS.SEE_ALL_OWN_INST]){
        list_endpoint = GLOBALS.ENDPOINTS.SUBJECT.LIST_INST
     }

     var endopoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     list_endpoint + "/" +  institution_id;
     
     API.APICall(endopoint,callback);
  },

  listReportsForSubject: function(subject_id, callback){

    let permissions = JSON.parse(sessionStorage.getItem(GLOBALS.SESSION_KEYS.PERMISSIIONS));
    let list_endpoint = GLOBALS.ENDPOINTS.REPORTS.LIST
    if (permissions[GLOBALS.PERMISSIIONS.SEE_ALL_OWN_INST]){
       list_endpoint = GLOBALS.ENDPOINTS.REPORTS.LIST_INST
    }

    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    list_endpoint + "/" + subject_id;

    API.APICall(endpoint,callback);

  },

  listMedicalRecordsForInsititution: function (institution, callback){

     var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.MEDRECORDS.LIST + "/" + institution;

     API.APICall(endpoint,callback);

  },

  getMedicalRecord: function (med_rec_id, institution_id, callback){

     var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.MEDRECORDS.GET + "/" + med_rec_id;

     if (med_rec_id == 0){
        endpoint = endpoint + "?institution=" + institution_id;      
     }

     API.APICall(endpoint,callback);

  },

  modifyMedicalRecord: function(subject_id, med_rec, callback){

     var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
     GLOBALS.ENDPOINTS.MEDRECORDS.MODIFY + "/" + subject_id;
   
     API.APICall(endpoint,callback,false,med_rec);

  },

  getReport: function (report_id, lang, discarded ,callback){

   let permissions = JSON.parse(sessionStorage.getItem(GLOBALS.SESSION_KEYS.PERMISSIIONS));
   let report_endpoint = GLOBALS.ENDPOINTS.REPORTS.GET
   if (permissions[GLOBALS.PERMISSIIONS.SEE_ALL_OWN_INST]){
      report_endpoint = GLOBALS.ENDPOINTS.REPORTS.GET_INST
   }    

    var endpoint = sessionStorage.getItem(GLOBALS.SESSION_KEYS.API) + "/" +
    report_endpoint + "/" + report_id + "?lang=" + lang;

    var isBlob = true

    //console.log("Discarded is " + discarded);

    if (discarded != undefined){
       if (discarded){
          console.log("Is discareded")
          endpoint = endpoint + "&discarded=true"
          isBlob = false;
       }
    }

    console.log(endpoint);

    //GLOBALS.ENDPOINTS.REPORTS.GET + "/" + report_id;
    API.APICall(endpoint,callback,isBlob);
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