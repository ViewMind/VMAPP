
function Route(){

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
   // else if (whereToGo == GLOBALS.ROUTING.PAGES.VIEWREPORT){
   //    loadViewRenderReport();
   //    gotoIndex = false;
   // }
   else if (whereToGo == GLOBALS.ROUTING.PAGES.USER_ACCOUNT){
      loadViewAccountData();
      gotoIndex = false;
   }
   else if (whereToGo != null){
      console.log("Unknown location " + whereToGo);
   }

   if (gotoIndex){
      loadIndexView();
   }
   
}