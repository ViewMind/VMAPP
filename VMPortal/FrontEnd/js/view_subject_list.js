
var savedSubjectBasicList;

function loadViewSubjectList(){

   institution_name = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME);
   institution_id   = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID);

   if (institution_name === undefined){
      // Something went wrong.
      console.log("Unknown Institution ID " + institution_id);
      window.location.href = "index.html";
   }

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Subject List"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": institution_name, "where_to": ""});
   generateTrail(breadcrumb);   

   API.listPatientsForInstitutionForUser(institution_id,"onReceivePatientList");

}

function onReceivePatientList(response){

   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }

   //console.log(JSON.stringify(response));

   // Definining the Column Names.
   var colLastName  = "Last Name";
   var colFirstName = "First Name";
   var colSubjectID = "Subject ID";
   var colBirthDate = "Birth Date";
   var colAge       = "Age";
   var colGender    = "Gender";
   var colVMID      = "ViewMind ID";

   // Setting up the table. 
   var table = {};
   table.title = "Double click on a subject to see the reports";
   table.column_list = [colLastName, colFirstName, colSubjectID, colBirthDate, colAge, colGender, colVMID];
   table.unique_id = colVMID;
   table.show_unique = true;
   table.gotoFunction = "goToSubjectReportPage";
   table.data = [];

   // Now we create the data structure. 
   savedSubjectBasicList = {};
   for (var i = 0; i < response.data.length; i++){
      var row = {};
      row[colLastName]  = displayValue(response.data[i],ACCESS.SUBJECT.LAST_NAME);
      row[colFirstName] = displayValue(response.data[i],ACCESS.SUBJECT.NAME);
      row[colSubjectID] = displayValue(response.data[i],ACCESS.SUBJECT.INTERNAL_ID);
      row[colBirthDate] = displayValue(response.data[i],ACCESS.SUBJECT.BIRTH_DATE);
      row[colAge]       = displayValue(response.data[i],ACCESS.SUBJECT.AGE);
      row[colGender]    = displayValue(response.data[i],ACCESS.SUBJECT.GENDER);
      row[colVMID]      = displayValue(response.data[i],ACCESS.SUBJECT.UNIQUE_ID);

      subjectData = {}

      subjectData.fname = row[colLastName] 
      subjectData.lname = row[colFirstName];
      subjectData.did   = row[colSubjectID]
      subjectData.age   = row[colAge];
      subjectData.id    = row[colVMID];
      savedSubjectBasicList[row[colVMID]] = subjectData;

      table.data.push(row);
   }

   buildTable(table);

}

function goToSubjectReportPage(subject_id){

   // Saving the selected Subject Data. 

   //console.log("Storing subject data: " + JSON.stringify(savedSubjectBasicList[subject_id]));

   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_SUBJECT,JSON.stringify(savedSubjectBasicList[subject_id]));

   window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" + GLOBALS.ROUTING.PAGES.REPORTS;
}