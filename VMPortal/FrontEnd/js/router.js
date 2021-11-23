
function Route(){

   
   let permissions = JSON.parse(sessionStorage.getItem(GLOBALS.SESSION_KEYS.PERMISSIIONS));

   //console.log("Permissions: " + sessionStorage.getItem(GLOBALS.SESSION_KEYS.PERMISSIIONS));

   // Check if the user is allowed access to medical records
   if (permissions[GLOBALS.PERMISSIIONS.SEE_MED_RECS]){
      //console.log("Enabling MedRec Menu")
      document.getElementById(GLOBALS.HTML.MEDREC_SIDE_MENU).style = "";
   }

   // Check if the user is allowed to the administrative panel. 
   if (permissions[GLOBALS.PERMISSIIONS.SEE_ADMIN_PANEL]){
      document.getElementById(GLOBALS.HTML.ADMIN_PANEL).style = "";
   }

   generateSideBarInstitutionList();

   var querystring = window.location.search;
   var urlparams = new URLSearchParams(querystring);   
   var whereToGo = urlparams.get(GLOBALS.ROUTING.PARAMS.GOTO);

   var gotoIndex = true;

   if (whereToGo == GLOBALS.ROUTING.PAGES.SUBJECTS){
      loadViewSubjectList()
      gotoIndex = false;
   }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.REPORTS){
      loadViewReportList();
      gotoIndex = false;
   }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.USER_ACCOUNT){
      loadViewAccountData();
      gotoIndex = false;
   }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.MEDRECS){
      loadViewMedRecList()
      gotoIndex = false;
   }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.MEDRECEDIT){
      loadViewMedRecEdit()
      gotoIndex = false;
   }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.ADMINEVAL){
      laodMasterEvaluatorViewList();
      gotoIndex = false;
   }
   else if (whereToGo != null){
      console.log("Unknown location " + whereToGo);
   }

   if (gotoIndex){
      loadIndexView();
   }
   
}