function loadViewReportList(){

   var subjectData = JSON.parse(sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_SUBJECT));
   var inst_name = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME);
   var navindex = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.SUBJECTS;

   var sname = getSubjectDisplayID();

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = sname;

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": inst_name, "where_to": navindex});
   breadcrumb.map.push({"name": sname, "where_to": ""})
   generateTrail(breadcrumb);   

   API.listReportsForSubject(subjectData.id,"onReceiveReportList");

}


function onReceiveReportList(response){

   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }   

   //console.log(JSON.stringify(response));

   // Definining the Column Names.
   var colStudyType   = "Study Type";
   var colStudyDate   = "Study Date";
   var colProcDate    = "Processing Date";
   var colEvaluator   = "Evaluator";
   var colEvalEmail   = "Evaluator Email";

   // Setting up the table. 
   var table = {};
   table.title = "Double click on a report to show results";
   table.column_list = [colStudyType, colStudyDate, colProcDate, colEvaluator, colEvalEmail, ACCESS.COMMON.KEY];
   table.unique_id = ACCESS.COMMON.KEY;
   table.show_unique = false;
   table.gotoFunction = "goToReportViewPage";
   table.data = [];

   // Now we create the data structure. 
   for (var i = 0; i < response.data.length; i++){
      var row = {};
      row[colStudyType]       = response.data[i][ACCESS.EVALUATIONS.STUDY_TYPE]
      row[colStudyDate]       = displayDate(response.data[i][ACCESS.EVALUATIONS.STUDY_DATE]);
      row[colProcDate]        = displayDate(response.data[i][ACCESS.EVALUATIONS.PROCESSING_DATE]);
      row[colEvaluator]       = response.data[i][ACCESS.EVALUATIONS.EVALUATOR_LASTNAME] + ", " + response.data[i][ACCESS.EVALUATIONS.EVALUATOR_NAME]
      row[colEvalEmail]       = response.data[i][ACCESS.EVALUATIONS.EVALUATOR_EMAIL]
      row[ACCESS.COMMON.KEY]  = response.data[i][ACCESS.COMMON.KEY]
      table.data.push(row);
   }

   buildTable(table);

}

function goToReportViewPage(report_id){

   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_REPORT,report_id);   
   // window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" + GLOBALS.ROUTING.PAGES.VIEWREPORT;
   var lang = "en";
   WaitDialog.open("Retrieving report PDF");
   API.getReport(report_id,lang,"onGetReportPDF");   

}

function onGetReportPDF(response){
   WaitDialog.close();
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }
   else{
      //console.log("Attempting to show PDF " + response.filename + " in a new tab");
      var url = URL.createObjectURL(response.data);
      
      window.open("_blank").location = url;
   }
}