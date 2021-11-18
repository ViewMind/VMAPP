// This will be the name space for any global constants or variables. 
var GLOBALS = {}; 

// Keys for sessionStorage. 
GLOBALS.SESSION_KEYS = {};
GLOBALS.SESSION_KEYS.EXPIRATION_TIME   = "expiration_time" 
GLOBALS.SESSION_KEYS.EXPIRATION_DATE   = "expiration_date"
GLOBALS.SESSION_KEYS.API               = "api"
GLOBALS.SESSION_KEYS.USER              = "username"
GLOBALS.SESSION_KEYS.USER_ID           = "user_id"
GLOBALS.SESSION_KEYS.USER_FNAME        = "user_first_name"
GLOBALS.SESSION_KEYS.USER_LNAME        = "user_last_name"
GLOBALS.SESSION_KEYS.TOKEN             = "token"
GLOBALS.SESSION_KEYS.USER_INSTITUTIONS = "user_institutions"
GLOBALS.SESSION_KEYS.PERMISSIIONS      = "permissions";

// Permissions keys
GLOBALS.PERMISSIIONS = {};
GLOBALS.PERMISSIIONS.SEE_ALL_OWN_INST  = "see_all_own_institution";
GLOBALS.PERMISSIIONS.SEE_MED_RECS      = "load_medical_records";
GLOBALS.PERMISSIIONS.SEE_ADMIN_PANEL   = "admin_dashboard";

// Simple comunicating between pages. 
GLOBALS.PAGE_COMM = {}
GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID   = "selected_institution";
GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME = "selected_institution_name";
GLOBALS.PAGE_COMM.SELECTED_SUBJECT          = "selected_subject";
GLOBALS.PAGE_COMM.SELECTED_REPORT           = "selected_report";
GLOBALS.PAGE_COMM.SELECTED_PDF              = "selected_PDF";
GLOBALS.PAGE_COMM.SELECTED_MEDREC           = "selected_medical_record"

// For accessing the inst construct.
GLOBALS.INST_LIST = {};
GLOBALS.INST_LIST.NAME = "name";
GLOBALS.INST_LIST.ID   = "id";

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
GLOBALS.ENDPOINTS.REPORTS.GET  = "reports/get";
GLOBALS.ENDPOINTS.REPORTS.LIST_INST = "reports/list_all_own_institution";
GLOBALS.ENDPOINTS.REPORTS.GET_INST  = "reports/get_own_institution";
GLOBALS.ENDPOINTS.REPORTS.ALL_EVALS  = "reports/admin_evaluation_list";
GLOBALS.ENDPOINTS.SUBJECT = {}
GLOBALS.ENDPOINTS.SUBJECT.LIST = "subjects/list";
GLOBALS.ENDPOINTS.SUBJECT.LIST_INST = "subjects/list_all_own_institution";
GLOBALS.ENDPOINTS.PORTAL_USER = {};
GLOBALS.ENDPOINTS.PORTAL_USER.MODIFY_OWN = "portal_users/modify_own"
GLOBALS.ENDPOINTS.PORTAL_USER.LOGOUT = "portal_users/logout"
GLOBALS.ENDPOINTS.MEDRECORDS = {}
GLOBALS.ENDPOINTS.MEDRECORDS.MODIFY = "medical_record/modify"
GLOBALS.ENDPOINTS.MEDRECORDS.LIST = "medical_record/list"
GLOBALS.ENDPOINTS.MEDRECORDS.GET = "medical_record/get"
GLOBALS.URLPARAMS = {}
GLOBALS.URLPARAMS.SUBJECT = {}
GLOBALS.URLPARAMS.SUBJECT.PORTAL_USER = "pp";

// The HTML Elements that will be commonly accessed by all the JS scripts. 
GLOBALS.HTML = {}
GLOBALS.HTML.WAIT_DIALOG = {}
GLOBALS.HTML.WAIT_DIALOG.TEXT = "waitDialogText"
GLOBALS.HTML.WAIT_DIALOG.MODAL = "waitDialog"
GLOBALS.HTML.ERROR_DIALOG = {}
GLOBALS.HTML.ERROR_DIALOG.MODAL = "errorDialog";
GLOBALS.HTML.ERROR_DIALOG.TITLE = "errorTitle";
GLOBALS.HTML.ERROR_DIALOG.TEXT  = "errorMainBody";
GLOBALS.HTML.USERNAME = "display_user_name";
GLOBALS.HTML.NAV_INST_LIST = "side_bar_institution_list";
GLOBALS.HTML.MEDREC_INST_LIST = "side_bar_med_record_institution_list"
GLOBALS.HTML.MAIN = {}
GLOBALS.HTML.MAIN.TITLE    = "main_title";
GLOBALS.HTML.MAIN.CONTENTS = "main_contents";
GLOBALS.HTML.MAIN.TABLE    = "main_table";
GLOBALS.HTML.MAIN.TRAIL    = "main_trail";
GLOBALS.HTML.MEDREC_SIDE_MENU = "side_bar_medrecs";
GLOBALS.HTML.ADMIN_PANEL   = "admin_dashboard_panel";

// The Routing Constants.
GLOBALS.ROUTING = {}
GLOBALS.ROUTING.PARAMS = {}
GLOBALS.ROUTING.PARAMS.GOTO  = "goto" 
GLOBALS.ROUTING.PARAMS.ID    = "id" 
GLOBALS.ROUTING.PAGES = {};
GLOBALS.ROUTING.PAGES.INSTITUTION  = "institution";
GLOBALS.ROUTING.PAGES.SUBJECTS     = "subjects";
GLOBALS.ROUTING.PAGES.REPORTS      = "reports";
GLOBALS.ROUTING.PAGES.MEDRECS      = "medrecs";
GLOBALS.ROUTING.PAGES.USER_ACCOUNT = "myaccount";
GLOBALS.ROUTING.PAGES.MEDRECEDIT   = "medrecedit";
GLOBALS.ROUTING.PAGES.ADMINEVAL    = "admineval";

// Directory Map For Report Resources.
GLOBALS.REPRESMAP = {}
GLOBALS.REPRESMAP["Go No-Go"] = "GoNoGo";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
function checkConfig(force){
   if (force === undefined) force = false;
   if ((sessionStorage.getItem(GLOBALS.SESSION_KEYS.API ) === null) || (force)){
      getConfigurationFromServer();
   }
}

/// Token management is encapsulated within these three function so that it may replaced easily in the future. 
var Token = {
   
   checkLogIn: function(){
      var ret = true;
      
      if (sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN) !== null){
         //console.log("Checking date for current token");
         var now = new Date();
         var expiry = new Date(sessionStorage.getItem(GLOBALS.SESSION_KEYS.EXPIRATION_DATE));
         //console.log("Comparing NOW: " + now.toString() + " to EXP: " + expiry.toString())
         if (expiry > now) ret = false;
         else ret = true;
      }
      else {
         //console.log("TOKEN not set")
         ret = true;
      }
      if (ret) window.location.href = "login.html";
   },

   setToken: function (token){
      var minutes = sessionStorage.getItem(GLOBALS.SESSION_KEYS.EXPIRATION_TIME);
      //var minutes = 1;
      minutes = parseInt(minutes);
      //console.log("Adding " + minutes + " minutes");
      var dt = new Date();
      //console.log("Current date: " + dt.toString());
      dt.setMinutes( dt.getMinutes() + minutes);
      //console.log("Expiration: "  + dt.toString());
      sessionStorage.setItem(GLOBALS.SESSION_KEYS.TOKEN,token);
      sessionStorage.setItem(GLOBALS.SESSION_KEYS.EXPIRATION_DATE,dt.toString())
      //console.log("SET EXPIRY TO: "  + dt.toString());
   },

   getToken: function(){
      return sessionStorage.getItem(GLOBALS.SESSION_KEYS.TOKEN);
   }

}

/// Wait Dialog In case We neeed it.
var WaitDialog = {

   modal: undefined,

   init: function(){
      var myModal = document.getElementById(GLOBALS.HTML.WAIT_DIALOG.MODAL);
      this.modal = new bootstrap.Modal(myModal);
   },

   open: function(title){
      document.getElementById(GLOBALS.HTML.WAIT_DIALOG.TEXT).innerHTML = title;
      this.modal.show(); 
   },

   close: function() {      
      //var myModal = document.getElementById(GLOBALS.HTML.WAIT_DIALOG.MODAL);
      //var modal = new bootstrap.Modal(myModal);
      this.modal.hide();
   }

}

/// Error dialog. Will show when an error occurs with the simplified server message. 
var ErrorDialog  = {

   modal: undefined,

   init: function(){
      var myModal = document.getElementById(GLOBALS.HTML.ERROR_DIALOG.MODAL);
      this.modal = new bootstrap.Modal(myModal);
   },

   open: function (title, text){
      document.getElementById(GLOBALS.HTML.ERROR_DIALOG.TITLE).innerHTML = title;
      document.getElementById(GLOBALS.HTML.ERROR_DIALOG.TEXT).innerHTML = "<p>" + text + "</p>";
      this.modal.show();
   },

   close: function(){
      this.modal.hide();
   }
   
}

/// Code for opening and closing The Side Bar. 
window.addEventListener('DOMContentLoaded', event => {
   // Toggle the side navigation
   const sidebarToggle = document.body.querySelector('#sidebarToggle');
   if (sidebarToggle) {
       sidebarToggle.addEventListener('click', event => {
           event.preventDefault();
           document.body.classList.toggle('sb-sidenav-toggled');
           localStorage.setItem('sb|sidebar-toggle', document.body.classList.contains('sb-sidenav-toggled'));
       });
   }
});

/// Code for listing the institutions in the side bar.
function generateSideBarInstitutionList(){
   
   //console.log("GENERATE SIDE BAR INST");

   var inst_list = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_INSTITUTIONS);
   inst_list = JSON.parse(inst_list);
   if (inst_list == null) return;
   if (inst_list.length < 1) return;
      
   //console.log(JSON.stringify(inst_list));

   var html  = ""; // For Subject List for Reports.
   var html2 = ""; // For Medical Record List. 

   for (var i = 0; i < inst_list.length; i++){
      
      html = html + '<a class = "nav-link" style = "cursor:pointer" onclick = "goToInstitution(\'' 
      + inst_list[i][GLOBALS.INST_LIST.ID] + '\',\'' + GLOBALS.ROUTING.PAGES.SUBJECTS  + '\')" >' 
      + inst_list[i][GLOBALS.INST_LIST.NAME]  + '</a>\n'

      html2 = html2 + '<a class = "nav-link" style = "cursor:pointer" onclick = "goToInstitution(\'' 
      + inst_list[i][GLOBALS.INST_LIST.ID] + '\',\'' + GLOBALS.ROUTING.PAGES.MEDRECS  + '\')" >' 
      + inst_list[i][GLOBALS.INST_LIST.NAME]  + '</a>\n'

   }
   document.getElementById(GLOBALS.HTML.NAV_INST_LIST).innerHTML = html;
   document.getElementById(GLOBALS.HTML.MEDREC_INST_LIST).innerHTML = html2;
}

/// Code to switch to the subject list of a particular institution. 
function goToInstitution(id,what_view){

   var list = JSON.parse(sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_INSTITUTIONS));
   institution_name = "";
   for (i in list){
      if (list[i][GLOBALS.INST_LIST.ID] == id){
         institution_name = list[i][GLOBALS.INST_LIST.NAME]
      }
   }

   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID,id);
   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME,institution_name);

   window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" + what_view;
}


/// Code for building a generic table as the main element of the page. 
/**
 * 
 * @param {
 *    JSON containing the following fields:
 *    - title: The name of the table.
 *    - column_list: The name of each column as it is displayed. Also this will be the name of the key in the data array.
 *    - data: The data array. It's a list composed of as many elements as there are rows. Each element is a JSON array containing the value assocciated with each column name.
 *    - gotoFunction: When a row is double clicked this function will be called passing as an only parameter the value of the unique_id for that row.
 *    - unique_id: The column that contains the unique_id for the row. Needs to be part of the column_list as well as be in each array of the data structure. 
 *    - show_unique: Boolean. If false the unique_id column will be skipped. 
 *    - container: String. If it exists, then this is the container (div) where the table will be put.
 * } table 
 */
function buildTable(table){

   var html = '\
   <div class="card mb-4">\
      <div class="card-header vm-gray">\
         <i class="fas fa-table me-1"></i>\
         ##TABLE_NAME##\
      </div>\
      <div class="card-body">\
         <table id = "##TABLE_ID##">\
         <thead>\
             <tr>\
               ##COLUMNLIST##\
             </tr>\
         </thead>\
         <tfoot>\
             <tr>\
               ##COLUMNLIST##\
             </tr>\
         </tfoot>\
         <tbody>\
            ##TABLE_BODY##\
         </tbody>\
         </table>\
      </div>\
   </div>';

   // Checking that everything is there.    
   if (!("title" in table)){
      table.title = "Unnamed table";
   }

   if (!("data" in table)){
      console.log("BUILD TABLE: No data. Exiting");
      return;
   }
   else{
      if (table.data.length < 1){
         console.log("BUILD TABLE: Empty data. Exiting");
      }
   }

   if (!("column_list" in table)){
      console.log("BUILD TABLE: Column list. Exiting");
      return;
   }

   if ("unique_id" in table){
      if (!("show_unique" in table)){
         table.show_unique = false;
      }
   }
   else{
      table.unique_id = "";
      table.show_unique = false;
   }

   if (!("gotoFunction" in table)){
      table.gotoFunction = "";
   }

   // Setting the table name and ID. 
   html = html.replace("##TABLE_NAME##",table.title);
   html = html.replace("##TABLE_ID##",GLOBALS.HTML.MAIN.TABLE);

   // Building the column list.
   var html_col_list = "";
   for (var i = 0; i < table.column_list.length; i++){
      
      // If the this is the unique id column and we are not supposed to show it, we move on.
      if ((table.column_list[i] == table.unique_id) && (!table.show_unique)) continue;

      html_col_list = html_col_list + "<th>" + table.column_list[i] + "</th>\n";
   }

   html = html.replace("##COLUMNLIST##",html_col_list);
   html = html.replace("##COLUMNLIST##",html_col_list);

   // Building the table body. 
   var table_body = "";

   for (var r = 0; r < table.data.length; r++){

      // Create a new row.
      var tr = "<tr ##ONDBCLICK## >\n";
      var ondbclick = "";
      var idfound = false;
      if (table.gotoFunction != ""){
         ondbclick = 'ondblclick = "' + table.gotoFunction +   '(##ID##)"'
      }

      for (var c = 0; c < table.column_list.length; c++){
         
         var col_name = table.column_list[c];

         if (col_name == table.unique_id){

            // If this this the id column the id is replaced as the parameter on the double click function, if defined. 
            ondbclick = ondbclick.replace("##ID##","'" + table.data[r][col_name] + "'");
            idfound = true;

            // If the this is the unique id column and we are not supposed to show it, we move on.
            if (!table.show_unique){
               continue;
            }
         }

         // Othewise we add the column for this row. 
         tr = tr + "<td>" +  table.data[r][col_name]  + "</td>\n"

      }

      // If ID wasn't found, then we replace the doubleclick function call with nothing. 
      if (!idfound){
         ondbclick = ondbclick.replace("##ID##","");
      }

      // Now we replace the place holder on the tr with the call to the double click function.
      tr = tr.replace("##ONDBCLICK##",ondbclick);

      // Finalize the row.
      tr = tr + "</tr>\n";

      // console.log(tr);

      // And add it to the body. 
      table_body = table_body + tr;
   }

   // Insert the body table into the html
   html = html.replace("##TABLE_BODY##",table_body);

   if ("container" in table){
      document.getElementById(table.container).innerHTML = html;
   }
   else{
      document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = html;
   }

   //console.log(html);

   // Enable the Simple Data Tables. 
   const displayTable = document.getElementById(GLOBALS.HTML.MAIN.TABLE);
   let dataTable = new simpleDatatables.DataTable(displayTable); 

}

/// Code for generating the BreadCrumb Trail below the title.

/**
 * 
 * @param {
 *    JSON Array with the following fields
 *    active_last: Boolean. If true the active class is applied ONLY the the last elememt of the trail. Otherwise it's only applied to all excepet the last. 
 *    map: JSON Array where each member has two fields: name (the display of the trail) and where_to the page to load when clicked. 
 * } trail 
 */

function generateTrail(trail){

   document.getElementById(GLOBALS.HTML.MAIN.TRAIL).innerHTML = "";

   if (!("active_last" in trail)){
      trail.active_last = true;
   }

   if (!("map" in trail)){
      console.log("GENERATE TRAIL: No map found")
      return;
   }

   // <!-- <li class="breadcrumb-item active">Dashboard</li>
   // <li class="breadcrumb-item" style="cursor: pointer;">Non active</li> -->   

   var html = "";

   for (var i = 0; i < trail.map.length; i++){
      var li =  '<li class = "breadcrumb-item ##ACTIVE##" ##STYLE## onclick="##ONCLICK##">' + trail.map[i]["name"] + "</li>\n"

      if (i == (trail.map.length-1)){
         // This is the last one which is the one that is active. So no cursor or link.
         li = li.replace("##STYLE##","");
         li = li.replace("##ONCLICK##","");
         if (trail.active_last){
            li = li.replace("##ACTIVE##","active");
         }
         else{
            li = li.replace("##ACTIVE##","");
         }
      }
      else{
         li = li.replace("##ONCLICK##","window.location.href='" + trail.map[i]["where_to"] + "'");
         li = li.replace("##STYLE##","style = 'cursor: pointer;'")
         if (trail.active_last){
            li = li.replace("##ACTIVE##","");
         }
         else{
            li = li.replace("##ACTIVE##","active");
         }
      }

      html = html + li; 
   }

   document.getElementById(GLOBALS.HTML.MAIN.TRAIL).innerHTML = html;
}

/// Code for setting the user name in the top right.
function setUserName(){
   var fname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_FNAME);
   var lname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_LNAME);
   document.getElementById(GLOBALS.HTML.USERNAME).innerHTML = lname + ", " + fname;
}

/// Simply logging out
function logOut(){
   API.logout("logOutReturnFunction");
}

function logOutReturnFunction (response){
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }
   sessionStorage.removeItem(GLOBALS.SESSION_KEYS.TOKEN);
   Token.checkLogIn();    
}

/// Convinience function to resturn N/A instead of nulls or undefined.
function displayValue(array, index){
   if (index in array){
      if (array[index] == null){
         return "N/A"
      }
      else return array[index];
   }
   return "N/A"
}

// Straight function to transform an ISO date to a more friendly display format. 
function displayDate(isodate){
   var date_and_time = isodate.split(" ");   
   if (date_and_time.length != 2) return isodate;
   var date = date_and_time[0];
   var time = date_and_time[1];
   var date_parts = date.split("-");
   if (date_parts.length != 3) return isodate;
   var retdate = date_parts[2] + "/" + date_parts[1] + "/" + date_parts[0];
   var time_parts = time.split(":")
   if (time_parts.length != 3) return isodate;
   var rettime = time_parts[0] + ":" + time_parts[1]
   return retdate + " " + rettime;
}

// Verification on what is empty to knwo what to display for the subject identifier
function getSubjectDisplayID(){
   var subjectData = JSON.parse(sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_SUBJECT));
   if (subjectData == null) return "";
   var sname = "";
   if ((subjectData.fname == "") && (subjectData.lname == "")){
      sname = subjectData.did;
   }
   else{
      sname = subjectData.lname + ", " + subjectData.fname;
   }   
   return sname;
}
