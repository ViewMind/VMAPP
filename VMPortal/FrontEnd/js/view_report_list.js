var discarded_reports = [];

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
   var colPUser       = "Medical Professional";
   var colDiscard     = "Discarded";
   var colEvalID      = "Evaluation ID";

   // Response data will contain both the portal user data and the evaluation list
   var evaluation_list = response.data.evaluation_list;
   var portal_users    = response.data.portal_users; // If this is empty the column is not shown.
   var has_users       = (Object.keys(portal_users).length > 0)

   // Setting up the table. 
   var table = {};
   table.title = "Double click on a report to show results";
   if (has_users){
      //console.log("Adding the PUser column");
      table.column_list = [colEvalID, colStudyType, colDiscard, colStudyDate, colProcDate, colPUser ,colEvaluator, colEvalEmail, ACCESS.COMMON.KEY];
   }
   else{
      table.column_list = [colEvalID, colStudyType, colDiscard, colStudyDate, colProcDate, colEvaluator, colEvalEmail, ACCESS.COMMON.KEY];
   }
   table.unique_id = ACCESS.COMMON.KEY;
   table.show_unique = false;
   table.gotoFunction = "goToReportViewPage";
   table.data = [];

   // List of discarded reports
   discarded_reports =  [];

   // Now we create the data structure. 
   for (var i = 0; i < evaluation_list.length; i++){
      var row = {};
      row[colStudyType]       = evaluation_list[i][ACCESS.EVALUATIONS.STUDY_TYPE]
      row[colStudyDate]       = displayDate(evaluation_list[i][ACCESS.EVALUATIONS.STUDY_DATE]);
      row[colProcDate]        = displayDate(evaluation_list[i][ACCESS.EVALUATIONS.PROCESSING_DATE]);
      row[colEvaluator]       = evaluation_list[i][ACCESS.EVALUATIONS.EVALUATOR_LASTNAME] + ", " + evaluation_list[i][ACCESS.EVALUATIONS.EVALUATOR_NAME]
      row[colEvalEmail]       = evaluation_list[i][ACCESS.EVALUATIONS.EVALUATOR_EMAIL]

      // The evaluation ID contains a .extension which needs to be removed. 
      var evalId = evaluation_list[i][ACCESS.EVALUATIONS.FILE_LINK];
      evalId = evalId.split(".")[0];

      row[colEvalID]          = evalId;

      // This will have the discard code. However all we care about if it's discarded or not. 
      var discard             = evaluation_list[i][ACCESS.EVALUATIONS.DISCARD_REASON]; 
      //console.log("Discard reason: " + discard);
      if ((discard == null) || (discard == "")){
         row[colDiscard] = "No";
      }
      else{
         row[colDiscard] = "Yes";
         discarded_reports.push(evaluation_list[i][ACCESS.COMMON.KEY])
      }

      if (has_users){         
         row[colPUser] = portal_users[evaluation_list[i][ACCESS.EVALUATIONS.PORTAL_USER]];
         //console.log("Adding the PUser " + row[colPUser]);
      }

      row[ACCESS.COMMON.KEY]  = evaluation_list[i][ACCESS.COMMON.KEY]

      

      table.data.push(row);
   }

   buildTable(table);

}

function goToReportViewPage(report_id){

   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_REPORT,report_id);   
   // window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" + GLOBALS.ROUTING.PAGES.VIEWREPORT;
   var lang = "en";   

   report_id = parseInt(report_id);

   discard = discarded_reports.includes(report_id);
   if (!discard){
      WaitDialog.open("Retrieving report PDF");
   }

   API.getReport(report_id,lang,discard,"onGetReportPDF");   

}

function onGetReportPDF(response){
   WaitDialog.close();

   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }
   else{
      //console.log("Attempting to show PDF " + response.filename + " in a new tab");
      //console.log("Checking if " + ACCESS.REPORT.DISCARD_REASON  + "  in response data");
      if (ACCESS.REPORT.DISCARD_REASON in response.data){
         // Discard reason, means that this is not a report. 
         html = "<p><b>Discard Reason Code: </b> " + response.data[ACCESS.REPORT.DISCARD_REASON] + "</b></p>";
         html = html + "<p><b>Comments</b></p></p>" + response.data[ACCESS.REPORT.COMMENT] + "</p>";
         ErrorDialog.open("This study was discarded",html);
      }
      else{
         // Regular report. 
         var url = URL.createObjectURL(response.data);      
         window.open("_blank").location = url;
      }
   }
}