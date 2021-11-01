class MultiKeyValuePairInput {

   #id
   #table_selected_index;
   #object_name
   #table_data
   #element_base_html

   // Ids for the components
   #value_field_id
   #select_field_id
   #other_field_id
   #table_body_id
   #other_block_id

   /**
    * 
    * @param {Object} parameters - All parameters required for constructing the Input
    * @param {String} parameters.id - The main id of the element
    * @param {String} parameters.keyname - The string name for the key values
    * @param {String} parameters.valuename - The string name for the value fields.
    * @param {Array}  parameters.options - List of string values that will fill the combo box. The last option will always be "Others". 
    * @param {Object} parameters.selected - If not empty it should be a key value array to fill the table. 
    * @param {Integer} parameters.width - Bootsrap columns width. As such it can only be a number from 1 to 12. Default is 7
    * @param {String} parameters.objectname - The string representing the object name given to the class instance. Necessary for calling the functions and referencig the objec.t 
    */

   constructor(parameters){

      this.#id = parameters.id;
      this.#table_selected_index = -1
      this.#object_name = parameters.objectname;

      // Generating the ids based on the unique id.
      let id = this.#id // Copied to a shorter variable for convenience. 

      this.#value_field_id = id + "_value_field"
      this.#select_field_id = id + "_select_field"
      this.#other_field_id = id + "_other_field"
      this.#table_body_id = id + "_table_body"
      this.#other_block_id = id + "_other_block";

      let col_number_width = 7;
      if ("width" in parameters){
         col_number_width = parameters.width;
      }

      let onplusclicked = "MultiKeyValuePairInput.addButtonClicked('" + this.#object_name + "')"
      let onselectchanged = "MultiKeyValuePairInput.onSelectionChanged('" + this.#object_name + "')"

      let html = '\
      <div class = row>\
          <div class = "col-lg-' + col_number_width + '">\
              <div class="row align-items-center">\
                 <div class = "col-sm-2">\
                     <label for="%%SELECT_ID%%" class="form-label" style="float: right;">%%KEYNAME%%</label>\
                 </div>\
                 <div class = "col-sm-4">\
                     <select class="form-select" id="%%SELECT_ID%%" onchange="' + onselectchanged + '">\
                        %%OPTIONS%%\
                     </select>\
                 </div>\
                 <div class = "col-sm-2">\
                     <label for="%%VALUE_FIELD_ID%%" class="form-label" style="float: right;">%%VALUENAME%%</label>\
                 </div>\
                 <div class = "col-sm-2">\
                     <input type="text" class="form-control" id="%%VALUE_FIELD_ID%%">\
                 </div>\
                 <div class = "col-sm-2">\
                     <button class="btn btn-primary" style="background-color: #011f42;" onclick="' + onplusclicked + '" ><i class="fas fa-plus"></i></button>\
                 </div>\
              </div>\
              <div id = "%%OTHER_BLOCK_ID%%" class="row align-items-center" style="margin-top: 10px; display: none">\
                  <div class = "col-sm-2">\
                      <label for="%%OTHER_FIELD_ID%%" class="form-label" style="float: right;">Specify Other</label>\
                  </div>\
                  <div class = "col-sm-9">\
                      <input type="text" class="form-control" id="%%OTHER_FIELD_ID%%">\
                  </div>\
              </div>\
              <div class="row align-items-center" style="margin-top: 10px;">\
                 <div class = "col-sm-12">\
                    <table class="table table-hover">\
                        <thead style="position: sticky; top: 0; z-index: 1; background:#011f42; ">\
                            <tr style="text-align: center; color: #ffffff;">\
                              <th>%%KEYNAME%%</th>\
                              <th>%%VALUENAME%%</th>\
                              <th>Remove</th>\
                            </tr>\
                          </thead>\
                          <tbody id="%%TABLE_BODY_ID%%">\
                             %%TABLE_BODY%%\
                          </tbody>\
                    </table>\
                 </div>\
              </div>\
          </div>\
      </div>'
     

      // Replacing all main ids.
      html = html.replaceAll("%%SELECT_ID%%",this.#select_field_id);
      html = html.replaceAll("%%VALUE_FIELD_ID%%",this.#value_field_id);
      html = html.replaceAll("%%OTHER_FIELD_ID%%",this.#other_field_id);
      html = html.replaceAll("%%TABLE_BODY_ID%%",this.#table_body_id);
      html = html.replaceAll("%%OTHER_BLOCK_ID%%",this.#other_block_id);

      // Replacing the name 
      html = html.replaceAll("%%KEYNAME%%",parameters.keyname);
      html = html.replaceAll("%%VALUENAME%%",parameters.valuename);

      // Creating the options.
      let options = "";
      for (var i = 0; i < parameters.options.length; i++){
         options = options + "<option value='" + parameters.options[i] + "'>" + parameters.options[i] + "</option>"
      }
      options = options + "<option value='Other'>Other</option>"
      html = html.replace("%%OPTIONS%%",options)


      // Storing the table data. 
      this.#table_data = parameters.selected;

      // Creating the Table. 
      let table_body = this.updateTableBody(true)
      html = html.replace("%%TABLE_BODY%%",table_body)

      // At this point we have the base HTML
      this.#element_base_html = html;

      // Setting the HTML so as to create the objects. 
      //document.getElementById(parameters.container).innerHTML = html;

   }

   getBaseHTML(){
      return this.#element_base_html;
   }

   setSelected(i){
      this.#table_selected_index = i
      this.updateTableBody(false)
   }

   removeRow(index){
      let new_data = [];
      for (var i = 0; i < this.#table_data.length; i++){
         if (i != index)
         new_data.push(this.#table_data[i]);
      }
      this.#table_data = new_data;
      this.updateTableBody(false)
   }

   getSelection(){
      return this.#table_data
   }

   checkIfSpecifyOtherEnabled(){
      document.getElementById(this.#value_field_id).value = "";
      if (document.getElementById(this.#select_field_id).value == "Other"){
         document.getElementById(this.#other_block_id).style = "margin-top: 10px;"
      }
      else{
         document.getElementById(this.#other_block_id).style = "margin-top: 10px; display: none"
         document.getElementById(this.#other_field_id).value = "";
      }
   }

   addRow(){
      let key = ""
      if (document.getElementById(this.#select_field_id).value == "Other"){
         key = document.getElementById(this.#other_field_id).value;
      }
      else{
         key = document.getElementById(this.#select_field_id).value
      }

      // Not adding empty fields. 
      if (key == "") return;
      let value = document.getElementById(this.#value_field_id).value;
      if (value == "") return;

      this.#table_data.push({key: key, value: value})
      this.updateTableBody(false)
   }

   updateTableBody(just_get_string){
      // Creating the Table. 
      let table_body = ""
      for (var i = 0; i < this.#table_data.length; i++){
         let value = this.#table_data[i].value;
         let key = this.#table_data[i].key;
         
         let style = "style='text-align: center;";
         if (i == this.#table_selected_index){
            style = style + " background-color: #aaaaaa;"
         }
         style = style + "'";

         // Enable this line to enable hilighting a selected row.
         //let row = "<tr onclick='MultiKeyValuePairInput.rowClicked(" + '"' + this.#object_name + '"'  + "," + '"' + i + '"' + ")' " + style + " >\n<td>" + key + "</td>\n<td>" + value + "</td>\n"
         let row = "<tr " + style + " >\n<td>" + key + "</td>\n<td>" + value + "</td>\n"
         let onclick = "MultiKeyValuePairInput.removeButtonClicked('" + this.#object_name  +"','" + i + "')"
         row = row + '<td><button onclick="' + onclick + '" class="btn btn-primary" style="background-color: #011f42;" ><i class="fas fa-minus"></i></button></td></tr>\n'

         //console.log(row)

         table_body = table_body + row

      }
      if (just_get_string) return table_body;
      else document.getElementById(this.#table_body_id).innerHTML = table_body;
   }

   static rowClicked(object,index){
      //console.log("ROW CLICKED on Object " + object + " index at " + index)
      window[object].setSelected(index)
   }

   static removeButtonClicked(object,index){
      window[object].removeRow(index);
   }

   static onSelectionChanged(object){
      window[object].checkIfSpecifyOtherEnabled();
   }

   static addButtonClicked(object){
      window[object].addRow();
   }


   
}