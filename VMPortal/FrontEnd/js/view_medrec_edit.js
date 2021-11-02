 
var evaluationsInput;
var medicationsInput;

var rmn_field_groups = [
   ["Hippocampal Occupation Index (HOC)", "Hippocampus"],
   ["Left Hippocampus", "Right Hippocampus"],
   ["Amygdala", "Left Amygdala", "Right Amygdala"],
   ["Superior Lateral Ventricle", "Inferior Lateral Ventricle"],
   ["Total Gray Matter", "Total White Matter", "Total Cerebral Cortex"],
   ["Left Entorhinal Cortex", "Right Entorhinal Cortex"],
   ["Left Cortical Thickness", "Right Cortical Thickness"]
   ];

var drug_list = ["Anticholinergics drugs",
"IACHE",
"Benzodiacepines",
"Cannabinoids",
"Antiarrhythmic",
"Antiepileptic",
"Antipsychotic",
"SSRI",
"SNRI",
"Antihistamine drugs"];

var evaluation_list = ["MMSE",
   "ACE-R",
   "IFS",
   "BOSTON",
   "Rey",
   "RMN"];


function loadViewMedRecEdit(){

   medical_record_id = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_MEDREC);
   institution_name = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME);
   institution_id   = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID);
   var navindex = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.MEDRECS;

   if (institution_name === undefined){
      // Something went wrong.
      console.log("Unknown Institution ID " + institution_id);
      window.location.href = "index.html";
   }

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Edit Medical Record"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": institution_name, "where_to": navindex});
   generateTrail(breadcrumb);   

   console.log("Requesting MedRec " + medical_record_id)

   API.getMedicalRecord(medical_record_id,institution_id,"onObtainedMedicalRecord");

}
   

function onObtainedMedicalRecord(response){

   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   } 

   let margin_columns = 1;
   let center_columns = 10;
   let half_center_cols = 5
   let third_center_cols = [3, 4, 3];

   // console.log(JSON.stringify(response.data));

   let subject_link = ""      
   let record = {};

   if (ACCESS.MEDREC.SUBJECT_LINK in response.data.record){
      subject_link = response.data.record[ACCESS.MEDREC.SUBJECT_LINK];
      record = JSON.parse(response.data.record[ACCESS.MEDREC.RECORD])
   }

   //console.log(JSON.stringify(record));

   let html = '\
   <div class="row">\
       <div class="col-lg-' + margin_columns + ' +"></div>\
       <div class="col-lg-' + half_center_cols + ' +">\
          <input class="form-control" id = "' + ACCESS.MEDREC.NAME  + '" placeholder="Name">\
       </div>\
       <div class="col-lg-' + half_center_cols + ' +">\
          <input class="form-control" id = "' + ACCESS.MEDREC.LAST  + '" placeholder="Last Name">\
       </div>\
       <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   \
   <div class="row" style="margin-top: 20px;">\
        <div class="col-lg-' + margin_columns + ' +"></div>\
        <div class="col-lg-' + center_columns + ' +">\
           <input class="form-control" id="' + ACCESS.MEDREC.UNIQUE_ID  + '" placeholder="Unique Identifier">\
        </div>\
        <div class="col-lg-' + center_columns + ' +"></div>\
   </div>\
   \
   <div class="row" style="margin-top: 20px;">\
      <div class="col-lg-' + margin_columns + ' +"></div>\
      <div class="col-lg-' + center_columns + ' +">\
         <select class="form-control" id="subject_link">\
             %%SUBJECT_LIST%%\
         </select>\
      </div>\
      <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   \
   <div class = "row" style="margin-top: 20px;">\
      <div class = "col-lg-12">\
         <h3 style="text-align: center">Medications</h3>\
      </div>\
   <\div>\
   \
   <div class="row" style="margin-top: 20px;">\
      <div class="col-lg-' + margin_columns + ' +"></div>\
      <div class="col-lg-' + center_columns + ' +">\
         %%MEDICATIONS%%\
      </div>\
      <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   \
   <div class = "row" style="margin-top: 20px;">\
      <div class = "col-lg-12">\
         <h3 style="text-align: center">Evaluations</h3>\
      </div>\
   </div>\
   \
   <div class="row" style="margin-top: 20px;">\
      <div class="col-lg-' + margin_columns + ' +"></div>\
      <div class="col-lg-' + center_columns + ' +">\
         %%EVALUATIONS%%\
      </div>\
      <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   \
   <div class = "row" style="margin-top: 20px;">\
      <div class = "col-lg-12">\
         <h3 style="text-align: center">RMN</h3>\
      </div>\
   </div>'

   // Creating all the inputs for the RMN fields. 
   for (var i = 0; i < rmn_field_groups.length; i++){

      let L = rmn_field_groups[i].length;
      let widths = [half_center_cols, half_center_cols];      
      if (L == 3) widths = third_center_cols;

      let group = rmn_field_groups[i];

      let row = '\
      <div class = "row" style="margin-top: 20px;">\
         <div class="col-lg-' + margin_columns + ' +"></div>'

      for (var j = 0; j < L; j++){
         let id = group[j];
         id = id.toLowerCase();
         id = id.replace(" ","_");
         
         row = row + '<div class="col-lg-' + widths[j] + '"><input class="form-control" id="' + id  + '" placeholder="' + group[j] + '"></div>'
         
      }
      
      row = row + 
      '   <div class="col-lg-' + margin_columns + ' +"></div>\
      </div>'

      html = html + row;

   }

   // Continuing wiht the remaining components.
   html = html + '\
   <div class = "row" style="margin-top: 20px;">\
      <div class = "col-lg-12">\
         <h3 style="text-align: center">Presumptive Diagnosis</h3>\
      </div>\
   </div>\
   \
   <div class = "row" style="margin-top: 20px;">\
      <div class="col-lg-' + margin_columns + ' +"></div>\
      <div class="col-lg-' + center_columns + ' +">\
         <textarea class="form-control" rows = 20 id="presumptive_diagnosis"></textarea>\
      </div>\
      <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   <div class = "row" style="margin-top: 20px; margin-bottom: 20px">\
      <div class="col-lg-' + margin_columns + ' +"></div>\
      <div class="col-lg-' + half_center_cols + ' +"><button class="btn btn-primary" style="background-color: #011f42; width: 50%; float: left" onclick="medRecEditCancel()" >CANCEL</button></div>\
      <div class="col-lg-' + half_center_cols + ' +"><button class="btn btn-primary" style="background-color: #011f42; width: 50%; float: right" onclick="saveRecord()" >SAVE</button></div></div>\
      <div class="col-lg-' + margin_columns + ' +"></div>\
   </div>\
   '
   
   // Creating the medications input
   let parameters = {}
   parameters["id"] = "medicationsInput"
   parameters["container"] = "medications"
   parameters["keyname"] = "Medications"
   parameters["valuename"] = "Dose"
   parameters["options"] = drug_list;
   parameters["objectname"] = "medicationsInput"
   parameters.width = 12;
   parameters["selected"] = [];   

   medicationsInput = new MultiKeyValuePairInput(parameters)
   html = html.replace('%%MEDICATIONS%%',medicationsInput.getBaseHTML());

   // Creating the evaluations input
   parameters = {}   
   parameters["id"] = "evaluationsInput"
   parameters["container"] = "evaluations"
   parameters["keyname"] = "Evaluations"
   parameters["valuename"] = "Score"
   parameters["options"] = evaluation_list;
   parameters["objectname"] = "evaluationsInput"
   parameters.width = 12;
   parameters["selected"] = [];   

   evaluationsInput = new MultiKeyValuePairInput(parameters)
   html = html.replace('%%EVALUATIONS%%',evaluationsInput.getBaseHTML());   

   //console.log(JSON.stringify(response.data));
   let subject_list = response.data.subject_list;

   // Finally adding the list of subjects.
   let options = "<option value=''> Select the subject for this medical record</option>\n"
   for (var i = 0; i < subject_list.length; i++){
      let value = subject_list[i][ACCESS.SUBJECT.UNIQUE_ID];
      let name  = subject_list[i][ACCESS.SUBJECT.LAST_NAME] + ", " + subject_list[i][ACCESS.SUBJECT.NAME]
      + " (" + subject_list[i][ACCESS.SUBJECT.INTERNAL_ID] + ")"
      let selected = "";
      if (value == subject_link) selected = "selected";
      options = options + "<option " + selected + " value='" + value + "'>" + name + "</option>\n"
   }
   html = html.replace('%%SUBJECT_LIST%%',options);   

   document.getElementById("main_contents").innerHTML = html;

   // Actually loading the data into the UI, if this is not a new record. 
   let medical_record_id = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_MEDREC);
   if (medical_record_id != 0) loadUpUI(record);

}

function loadUpUI(record){

   // Loading data if it exists. First the Single IDS
   let single_ids = [ACCESS.MEDREC.NAME, ACCESS.MEDREC.LAST, ACCESS.MEDREC.UNIQUE_ID, "presumptive_diagnosis"];
   for (var i = 0; i < single_ids.length; i++){
      let element_id = single_ids[i]
      if (element_id in record) document.getElementById(element_id).value = record[element_id];
   }

   if ("RMN" in record){
      for (var key in record.RMN){
         //console.log("Setting " + key + " with " + record.RMN[key])
         document.getElementById(key).value = record.RMN[key];
      }
   }
   
   if ("evaluations" in record) evaluationsInput.setTableData(record.evaluations);
   if ("medications" in record) medicationsInput.setTableData(record.medications);
     
}

function saveRecord(){

   let record = {};
   let single_ids = [ACCESS.MEDREC.NAME, ACCESS.MEDREC.LAST, ACCESS.MEDREC.UNIQUE_ID, ACCESS.MEDREC.SUBJECT_LINK, "presumptive_diagnosis"];

   for (var i = 0; i < single_ids.length; i++){
      record[single_ids[i]] = document.getElementById(single_ids[i]).value;
   }

   // Storing RMN data.
   record["RMN"] = {};
   for (var i = 0; i < rmn_field_groups.length; i++){

      let L = rmn_field_groups[i].length;
      let group = rmn_field_groups[i];

      for (var j = 0; j < L; j++){
         let id = group[j];
         id = id.toLowerCase();
         id = id.replace(" ","_");
         record.RMN[id] = document.getElementById(id).value;
      }

   }   

   record["evaluations"] = evaluationsInput.getSelection();
   record["medications"] = medicationsInput.getSelection();

   medical_record_id = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_MEDREC);   
   institution_id   = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_ID);
   record[ACCESS.MEDREC.INSTITUTION] = institution_id;

   //console.log("Saving record with id " + medical_record_id + " from institution " + institution_id);

   WaitDialog.open("Saving record ...");
   API.modifyMedicalRecord(medical_record_id,record,"onSaveRecordFinished");

}

function medRecEditCancel(){
   // Going back to medical record list. 
   window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.MEDRECS;   
}

function onSaveRecordFinished(response){
   WaitDialog.close();
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }

   // Going back to medical record list. 
   window.location.href = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.MEDRECS;
}