//////////////////// REQUIRES
const { jsPDF } = require("jspdf"); // will automatically load the node version
const path = require('path')
const fs   = require('fs')
const Utils = require("./report_node_modules/report_utils.js");
const ReportPage = require("./report_node_modules/report_page.js");
const LayoutParser = require("./report_node_modules/layout_parser.js");
const TextReplacer = require("./report_node_modules/text_replacer");
const PresumptiveDiagnosisBox = require("./report_node_modules/presumptive_diagnosis_box.js");

//////////////////// SCRIPT START
if (process.argv.length != 5){
   console.log("PDF Generation: Expected 4 arguments but found " + (process.argv.length-1));
   process.exit(1);
}

// Getting the arguments. 
const this_script = process.argv[1];
const selected_language = process.argv[2];
const report_file = process.argv[3]
const output_pdf = process.argv[4];

// Determining the current path via script. 
const script_path = path.dirname(this_script);

// Loading the layout resource. 
const layout_resource =  JSON.parse(fs.readFileSync(script_path + "/resources/report_layouts.json"));

// Loading the language resource. 
let   languages   =  JSON.parse(fs.readFileSync(script_path + "/resources/report_strings.json"));
if (!(selected_language in languages)){
   console.log("PDF Generation: Unimplemented report language: " + selected_language);
   process.exit(1);
}
const language = languages[selected_language];
delete languages;

// Loading the actual report file. 
const report = JSON.parse(fs.readFileSync(report_file));

// Creating the jsPDF instance and loading all the required fonts. 
const doc = new jsPDF();
const font_dir = script_path + "/resources/encoded_fonts";
Utils.Utils.loadFonts(doc,font_dir);

// Getting the list of page layouts.
const report_type = report.report_type;

if (!(report_type in layout_resource)){
   console.log("Layout for report " + report_type + " does not exist");
   process.exit(1);
}

const layout_pages = layout_resource[report_type].pages;
const standard_deviation_values = layout_resource[report_type].standard_deviations;

// Creating the page structure.
let page_data = {};
page_data.motto = language.common.motto.split("\n");
//console.log(page_data.motto);
page_data.leftBoxData = {};
page_data.rightBoxData = {};

page_data.leftBoxData[language.common.doctor] = report.medic;
page_data.leftBoxData[language.common.evaluator] = report.evaluator;
page_data.leftBoxData[language.common.date] = report.date;

page_data.rightBoxData[language.common.patient] = report.patient;
page_data.rightBoxData[language.common.age] = report.age;

page_data.path = script_path;

// Saving the wait that "standard deviation" is abbreviated in the language. 
let sd_text = language.common.range_text_sd;
if (sd_text === undefined){
   console.log("Could not find SD Text");
   process.exit(1);
}

//console.log("Page data");
//console.log(page_data);

const report_page = new ReportPage(doc,page_data);

// Instance of TextReplacer to transform reference strings into actual text values for this specific report.
const text_replacer = new TextReplacer(language,report);

// We need to calculate the color and polygon indexes for each variable in the report. 
let error = LayoutParser.computeIndexes(layout_resource[report_type],report);
if (error != ""){
   console.log("Error computing indexes for report variables: " + error);
   process.exit(1);
}
// else{
//    console.log(report);
// }


// Computing the presumptive diagnosis, if available for this report type. 
if ("pdiag" in layout_resource[report_type]){
   let pdiag_index = LayoutParser.computePresumptiveDiagnosis(layout_resource[report_type],report);
   if (pdiag_index.index == -1){
      console.log("Error computing PDiag Index: " + pdiag_index.error);
      process.exit(1);
   }
   report.presumptive_diagnosis = pdiag_index.index;
   //console.log("PDIAG index is " + report.presumptive_diagnosis);
}

//console.log("PresumptiveDiagnosis index: " + report.presumptive_diagnosis);

// This line below is just used to force a presuptive diagnosis to know how the text fits in the box. 
// report.presumptive_diagnosis = 6;

// Each variable is stored keyed by the resulting color categorization. 
let variableColorCodes = {}

// Iterating over all report pages.
for (var page_number = 0; page_number < layout_pages.length; page_number++){

   let page_titles = layout_pages[page_number].title;

   //console.log("Page Titles: " + page_number);
   //console.log(page_titles);

   for (var i = 0; i < page_titles.length; i++){

      // The first step is to parse the title. 
      let parsed_string = text_replacer.reportString(page_titles[i].text);
      if (parsed_string === false){
         console.log("Getting title " + i + " on page " + page_number + " for report type: " + report_type + ": " + text_replacer.getError());
         process.exit(1);
      }

      page_titles[i].text = parsed_string;
      
   }
   //console.log(page_titles);

   // Create the new page.
   let effective_area = report_page.createNewReportPage(page_titles);

   //console.log("Effective area", effective_area);

   // Once the page is created we need to parse every item in the page according to its type.
   let items = layout_pages[page_number].items;

   // Storing the bounding boxes of elements to be referenced by other elements.
   let itemBoundingBoxes = {};

   for (var i = 0; i < items.length; i++){
      
      let item = items[i];
      
      let ret = {};
      
      // Adding objects required for calculations in the different parsers. 
      item.effective_area = effective_area;
      item.item_bounding_boxes = itemBoundingBoxes;
      item.variable_color_codes = variableColorCodes;

      if (item.type == "result_segment"){

         // In this case the item needs to be enhanced with the sd_text and the sd_structure, if it exists.
         item.sd_text = sd_text;
         item.sd = standard_deviation_values;

         ret = LayoutParser.parseResultSegment(doc,item,text_replacer);

         if (!(ret.color in variableColorCodes)){
            variableColorCodes[ret.color] = [];            
         }
         variableColorCodes[ret.color].push(ret.id);
      }
      else if (item.type == "color_code"){      
         ret = LayoutParser.parseColorCode(doc,item,text_replacer);        
      }
      else if (item.type == "bulleted_list"){
         ret = LayoutParser.parseBulletedList(doc,item,text_replacer);
      }
      else if (item.type == "text"){
         ret = LayoutParser.parsePlainText(doc,item,text_replacer);
      }
      else if (item.type == "polygon_graph"){
         ret = LayoutParser.parsePolygonGraph(doc,item,text_replacer);
      }
      else if (item.type == "side_by_side_4_pt_graph"){
         ret = LayoutParser.parseSideBySide4DotGraph(doc,item,text_replacer);
      }
      else if (item.type == "fixation_plot"){
         ret = LayoutParser.parseFixationPlot(doc,item,text_replacer);
      }
      else if (item.type == "pdiag"){
         ret = LayoutParser.parsePresumptiveDiagnosis(doc,item,text_replacer);
      }
      else if (item.type == "single_4_pt_graph"){
         ret = LayoutParser.parseSingle4DotGraph(doc,item,text_replacer);
      }
      else{
         console.log("WARNING: Unknown item type " + item.type);
         continue;
      }

      if (ret.error != ""){
         console.log("Error on item type " + item.type + "  on page " + page_number +  " item " + i +  " : " + ret.error + ". Item ID: " + ret.id);
         process.exit(1);
      }

      itemBoundingBoxes[ret.id] = ret.boundingBox;

   }

}

// Saving the report.
doc.save(output_pdf);
// Here we go.
console.log("Saved report to " + output_pdf);


