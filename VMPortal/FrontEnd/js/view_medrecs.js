
function loadViewMedRecList(){

   institution_name = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME);
   institution_id   = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID);

   if (institution_name === undefined){
      // Something went wrong.
      console.log("Unknown Institution ID " + institution_id);
      window.location.href = "index.html";
   }

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Medical Records"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": institution_name, "where_to": ""});
   generateTrail(breadcrumb);   

   API.listMedicalRecordsForInsititution(institution_id,"onReceiveMedRecList");   

}


function onReceiveMedRecList(response){
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }   

   let html = '\
   <div class = "row">\
      <div class="col-lg-11"></div>\
      <div class="col-lg-1"><button class="btn btn-primary" style="background-color: #011f42; float: right" onclick="goToMedicalRecordPage(0)" ><i class="fas fa-plus"></i></button></div>\
   </div>\
   <div class = "row">\
      <div class = "col-lg-12" id = "table_container">\
      </div>\
   </div>'

   // Creating the basic layout.
   document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = html;   

   // Definining the Column Names.
   var colLastName  = "Last Name";
   var colFirstName = "First Name";   
   var colSubjectID = "Subject ID";
   var colVMID      = "ViewmindID";

   // Setting up the table. 
   var table = {};
   table.title = "Double click on a person to see the medical record";
   table.column_list = [colLastName, colFirstName, colSubjectID, colVMID];
   table.unique_id = colVMID;
   table.show_unique = false;
   table.gotoFunction = "goToMedicalRecordPage";
   table.container = "table_container"
   table.data = [];

   // Now we create the data structure. 
   for (var i = 0; i < response.data.length; i++){
      var row = {};
      row[colLastName]  = displayValue(response.data[i],ACCESS.MEDREC.NAME);
      row[colFirstName] = displayValue(response.data[i],ACCESS.MEDREC.LAST);
      row[colSubjectID] = displayValue(response.data[i],ACCESS.MEDREC.UNIQUE_ID);
      row[colVMID]      = displayValue(response.data[i],ACCESS.MEDREC.VMID);

      subjectData = {}

      subjectData.fname = row[colLastName] 
      subjectData.lname = row[colFirstName];
      subjectData.did   = row[colSubjectID]
      subjectData.age   = row[colAge];
      subjectData.id    = row[colVMID];

      table.data.push(row);
   }

   buildTable(table);   
   
}

function goToMedicalRecordPage(vm_id){
   console.log("Asked to load the record for patient " + vm_id)
}

function doTest(){

   console.log("Testing 1...2...3");

   let json_data = {
      anobject: {
         name: "value",
         name2: "value2"
      },
      key: "anothervalue",
      somearray: [1, 2, 3, 4, 5]
   }

   API.modifyMedicalRecord(1,json_data,"testCallBack")

}

function testCallBack(response){
   
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }   

   console.log(response.data);

}