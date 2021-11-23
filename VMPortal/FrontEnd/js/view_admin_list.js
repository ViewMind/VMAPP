function laodMasterEvaluatorViewList(){

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "ViewMind's Evaluations"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   generateTrail(breadcrumb);   

   API.listAllEvaluations("onReceiveList");

}

function onReceiveList(response){

   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }

   //console.log(JSON.stringify(response));

   // Definining the Column Names.
   var colID             = "Evaluation ID"
   var colStudy          = "Study Type"
   var colSubject        = "Subject"
   var colDoctor         = "Doctor"
   var colEvaluator      = "Evaluator"
   var colInst           = "Institution"
   var colInstance       = "Instance"
   var colProcessingDate = "Processing Date"

   var evals        = response.data.evaluations;
   var subjects     = response.data.subjects;
   var institutions = response.data.institutions;
   var doctors      = response.data.portal_users;

   // Setting up the table. 
   var table = {};
   table.title = "Total number of evaluations: " + evals.length;
   table.column_list = [colID, colStudy, colSubject, colDoctor, colEvaluator, colInst, colInstance, colProcessingDate];
   table.unique_id = colID;
   table.show_unique = true;
   table.gotoFunction = "";
   table.data = [];

   // Now we create the data structure. 
   
   for (var i = 0; i < evals.length; i++){
      var row = {};

      var evalId = evals[i][ACCESS.EVALUATIONS.FILE_LINK];
      evalId = evalId.split(".")[0];
      row[colID]          = evalId;

      row[colStudy]     = displayValue(evals[i],ACCESS.EVALUATIONS.STUDY_TYPE);

      var subject = evals[i][ACCESS.EVALUATIONS.SUBJECT_ID];
      subject = subjects[subject]
      row[colSubject]   = subject;

      var doctor = evals[i][ACCESS.EVALUATIONS.PORTAL_USER];
      doctor = doctors[doctor]
      row[colDoctor]   = doctor;

      row[colEvaluator] = displayValue(evals[i],ACCESS.EVALUATIONS.EVALUATOR_LASTNAME) + ", " + displayValue(evals[i],ACCESS.EVALUATIONS.EVALUATOR_NAME)

      var inst = evals[i][ACCESS.EVALUATIONS.INSTITUTION_ID];
      inst = institutions[inst];
      row[colInst] = inst;

      row[colInstance]     = displayValue(evals[i],ACCESS.EVALUATIONS.INSTITUTION_INSTANCE);
      row[colProcessingDate]     = displayValue(evals[i],ACCESS.EVALUATIONS.PROCESSING_DATE);

      table.data.push(row);
   }

   buildTable(table);

}

