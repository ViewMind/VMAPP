 
// Loading all the modules. 
const { jsPDF } = require("jspdf"); // will automatically load the node version
const ReportPage = require("./report_node_modules/report_page.js");
const ResultBar = require("./report_node_modules/result_bar.js");
const FourPointGraph = require("./report_node_modules/four_dot_graph.js");
const Utils = require("./report_node_modules/report_utils.js");
const FixationDrawer = require("./report_node_modules/fixation_drawer.js");
const PolygonGraph = require("./report_node_modules/polygon_graph.js");
const SmoothLineGraph = require("./report_node_modules/smooth_line_graph.js");
const ColorCode = require("./report_node_modules/color_code.js");
const ResultSegment = require("./report_node_modules/result_segment.js");
const BulletedList = require("./report_node_modules/titled_bulleted_list.js");
const PresumptiveDiagnosisBox = require("./report_node_modules/presumptive_diagnosis_box");


// Default export is a4 paper, portrait, using millimeters for units
const doc = new jsPDF();
var PAGE_WIDTH  = doc.internal.pageSize.getWidth();
var PAGE_HEIGHT = doc.internal.pageSize.getHeight();

Utils.Utils.loadFonts(doc,"./resources/encoded_fonts");

// Creating the test structure.
var everyPageData = {};
everyPageData.motto = ["Measurement of Cognitive Performance", "through Eye-Tracking"];
everyPageData.leftBoxData = {"Patient" : "Jason Garmin", "Doctor" : "Gerardo Fernandez", "Evaluator": "Matias Shulz"}
everyPageData.rightBoxData = {"Age" : "61", "Date" : "29 of January 2021"}      
everyPageData.path = "./"

const page = new ReportPage(doc,everyPageData);
//var titles = [{py: 0, text: "Gaze Durartion"},{py: 0.25, text: "Some other title"}, {py: 0.75, text: "One final title, just for kicks"}]
var titles = [{py: 0, text: "Some results"}]
var effectiveArea = page.createNewReportPage(titles);

// Test Result Bar. 
// * @param {Object} [config.boundingBox] - The boundign box for the entire result segment expects fields, x, y, w, h
// * @param {number} [config.value] - The value of the variable being represented. As it needs to be displayed.
// * @param {string} [config.color_code] - Where the indicator needs to land in the result bar. Can be "red", "yellow", "green" or "blue";
// * @param {number} [config.ncolors] - Number of colors in the color bar. Can be 2, 3 or 4. 
// * @param {string} [config.title] - The text naming the value.
// * @param {string} [config.range_text] - The text explaining normal values. 
var rseg = {};
rseg.boundingBox = {}
rseg.boundingBox.x = effectiveArea.x;
rseg.boundingBox.w = effectiveArea.w;
rseg.boundingBox.y = effectiveArea.y;
rseg.boundingBox.h = effectiveArea.h/10;

var segments = [];
segments.push({
   title: "Visual Search",
   range_text: "Normal value under 300",
   value: 362,
   color_code: "red",
   ncolors: 4
})
segments.push({
   title: "Gaze Durartion",
   range_text: "Normal value under 650",
   value: 965,
   color_code: "yellow",
   ncolors: 3
})
segments.push({
   title: "Integrative Memory Performance",
   range_text: "Normal value under 18",
   value: 28.55687,
   ndecimals: 1,
   color_code: "green",
   ncolors: 4
})
segments.push({
   title: "Associative Memory Performance",
   range_text: "Normal value over 58",
   value: 61,
   color_code: "blue",
   ncolors: 4
})
segments.push({
   title: "Parahipocampal Region Preservation",
   range_text: "Normal value over 60",
   value: 73,
   color_code: "green",
   ncolors: 3
})
segments.push({
   title: "Executive functions",
   range_text: "Normal value under 60",
   value: 74,
   color_code: "red",
   ncolors: 2   
})

const resSeg = new ResultSegment(doc);
y = rseg.boundingBox.y;
for (var i = 0; i < segments.length; i++){
   var config = rseg;  
   config.boundingBox.y = y; 
   config = Object.assign({}, config, segments[i]); 
   resSeg.renderConfiguration(config);
   y = y + config.boundingBox.h
}

// Printing the Color code.
var cc = {};
cc.boundingBox = {};
cc.boundingBox.x = effectiveArea.x;
cc.boundingBox.w = effectiveArea.w;
cc.boundingBox.h = effectiveArea.h*0.15;
cc.boundingBox.y = effectiveArea.y +  effectiveArea.h - cc.boundingBox.h;
cc.color_texts = {
   //blue: "Functioning\nabove average",
   green: "No relevant problems\nwere encountered",
   yellow: "Mild impairment, no\nnecessarily pathological",
   red: "Severe impariment,\nand/or pathological\nproblem"
}
cc.title = "For each cognitive function analyzed, the results are indicated by the following color coding:";
const colorCode = new ColorCode(doc,cc);


page.createNewReportPage();
var linegraph = {};
linegraph.boundingBox = {}
linegraph.boundingBox.x = effectiveArea.x
linegraph.boundingBox.y = effectiveArea.y + effectiveArea.h*0.2;
linegraph.boundingBox.w = effectiveArea.w
linegraph.boundingBox.h = effectiveArea.h*0.6
linegraph.numberOfXTicks = 10
linegraph.numberOfYTicks = 3;
linegraph.xdecimals = 0;
linegraph.enableGrid = false;
linegraph.xlabel = "Index number"
linegraph.ylabel = "Somthing that should represent the y units"
linegraph.dataSets = [];
linegraph.legend = [];
linegraph.nspline = 5;

var color = [Utils.Colors.GRAPH_BLUE, Utils.Colors.GRAPH_PURPLE];
var n_data_sets = color.length;
for (var d = 0; d < n_data_sets; d++){
   var max_data_set = (Math.random()+0.1)*100;
   var dataset = [];
   for (var i = 0; i < 50; i++){
      dataset.push([i,Math.random()*max_data_set]);
   }
   linegraph.dataSets.push(dataset)
   linegraph.legend.push({color: color[d], text: "DataSet number " + d})
}

const lineGraph = new SmoothLineGraph(doc);
lineGraph.render(linegraph)


page.createNewReportPage();

var polygraph = {};
// polygraph.l = PAGE_WIDTH/2;
// polygraph.x = 0.25*PAGE_WIDTH;
// polygraph.y = effectiveArea.y + (effectiveArea.h - polygraph.l)/2;
// polygraph.vertex_labels = ["Processing\nSpeed","Reaction Time\nFacilitated","Reaction Time\nInterference","Index\nError","Decision Making\nFacilitated","Decision Making\nInterference","Executive\nFunctions"];
// polygraph.values        = [-1,0,0,0,0,0,0];
// polygraph.levels        = ["+1","0","-1","-2"];
// // var n = 12;
// // polygraph.vertex_labels = [];
// // polygraph.levels  = ["3","2","1","0"];
// // polygraph.values  = [];
// // for (var i = 0; i < n; i++){
// //    polygraph.values.push((i % 4));
// //    polygraph.vertex_labels.push("A very long label " + i + " with\nAt least two lines");
// // }

polygraph.l = 92.40068444444442
polygraph.x = 58.800435555555545;
polygraph.y = 376.60483348720095;
//polygraph.l = PAGE_WIDTH/2;
//polygraph.x = 0.25*PAGE_WIDTH;
polygraph.y = effectiveArea.y + (effectiveArea.h - polygraph.l)/2;
//console.log(PAGE_HEIGHT)
polygraph.vertex_labels = [
   'Visual Search',
   'Gaze Duration',
   'Integrative\nMemory',
   'Associative\nMemory',
   'Executive\nFunctions'
 ]
polygraph.values = [ 1, 4, 4, 4, 4 ]
polygraph.levels = [ '+1', '0', '-1', '-2' ]

const poly_graph = new PolygonGraph(doc,polygraph);

page.createNewReportPage();
var polygraph2 = {};
polygraph2.l = PAGE_WIDTH/2;
polygraph2.x = 0.25*PAGE_WIDTH;
polygraph2.y = effectiveArea.y + (effectiveArea.h - polygraph.l)/2;
polygraph2.vertex_labels = ["Processing\nSpeed","Reaction Time\nFacilitated","Reaction Time\nInterference","Index\nError","Decision Making\nFacilitated","Decision Making\nInterference","Executive\nFunctions"];
polygraph2.values        = [10,20,0,40,50,60,100];
polygraph2.levels        = ["25","50","75","100"];
polygraph2.range         = [5,80];
polygraph2.separator     = 2; // Indicates the second level or the the 75 valued line. 
// var n = 12;
// polygraph.vertex_labels = [];
// polygraph.levels  = ["3","2","1","0"];
// polygraph.values  = [];
// for (var i = 0; i < n; i++){
//    polygraph.values.push((i % 4));
//    polygraph.vertex_labels.push("A very long label " + i + " with\nAt least two lines");
// }
const poly_graph2 = new PolygonGraph(doc,polygraph2);

page.createNewReportPage();
items = ["Visual Search: Basal Ganglia, Superior Colicullus",
         "Gaze: Lateral Parietal Cortex, Medial Prefrontal Cortex (mPFC), Anterior Superior\nTemporal Sulcus (aSTS)",
         "Integrative Memory Functions: Enthorinal Cortex, Perirhinal Cortex",
         "Associative Memory Performance: Hipocampus",
         "Executive Functions: Prefrontal Cortex"
      ]
const bulletedList = new BulletedList(doc,"Preserved Cognitive Functions & Brain Areas:",items,effectiveArea.x,effectiveArea.y + effectiveArea.h*0.02)

var bb = bulletedList.getBoundingBox();
doc.setDrawColor("#ff0000");
doc.rect(bb.x,bb.y,bb.w,bb.h,"S");

page.createNewReportPage();

// Testing fixation plotter. 
var fixplotconfig = {}
fixplotconfig.boundingRect = {};
fixplotconfig.boundingRect.x = effectiveArea.w*0.3;
fixplotconfig.boundingRect.y = effectiveArea.y + effectiveArea.h*0.4;
fixplotconfig.boundingRect.w = effectiveArea.w*0.3;
fixplotconfig.boundingRect.h = effectiveArea.h/6;
fixplotconfig.resolution = [ 1122, 1094 ];
//fixplotconfig.resolution = [ 1920, 1080 ];
//fixplotconfig.binding = [ [0,2], [1,2] ]
fixplotconfig.gonogo = "G<-";
fixplotconfig.fixations = [[556.6129032258065,558.2903225806451],[397.96,694.58],[756.0980392156863,715.2549019607843],[397,715.439024390244],[756.7837837837837,721.5945945945946]];
//fixplotconfig.binding = [ [0,0], [1,0] ]
const fixDrawer = new FixationDrawer(doc);

fixDrawer.render(fixplotconfig);

fixplotconfig.boundingRect.x = fixplotconfig.boundingRect.x + fixplotconfig.boundingRect.w*1.2
fixplotconfig.gonogo = "R->";
fixplotconfig.label = "Some Trial"
fixDrawer.render(fixplotconfig);


page.createNewReportPage();

// Test 4 Point Graph 
var config = {};
config.boundingRect = {};
config.boundingRect.x = effectiveArea.w*0.05;
config.boundingRect.y = effectiveArea.y + effectiveArea.h*0.2;
config.boundingRect.w = effectiveArea.w*0.7;
config.boundingRect.h = effectiveArea.h*0.5;
config.legend =  true
config.leftColumnText  = "CONTROLS"
config.rightColumnText = "PATIENT"
config.yLabel = "Response Time";
config.yUnits = "ms"
config.legendValueType0 = "Direct Action Task"
config.legendValueType1 = "Indirect Action Task"
config.leftSet = [ {value: 25, variance: 10}, { value: 52, variance: 20} ];
config.rightSet = [ {value: 80, variance: 8} , { value: 103, variance: 15} ];
//
config.dbug = false;

const fourPtGraph1 = new FourPointGraph(doc,config);
fourPtGraph1.render();

page.createNewReportPage();
config.legend =  false
config.title = "Something nice"
config.leftSet = [ {value: 25, variance: 0}, { value: 52, variance: 0} ];
config.rightSet = [ {value: 80, variance: 0} , { value: 103, variance: 0} ];
config.boundingRect.x = effectiveArea.w*0.05;
config.boundingRect.y = effectiveArea.y + effectiveArea.h*0.2;
config.boundingRect.w = effectiveArea.w*0.4*1.08;
config.boundingRect.h = effectiveArea.h*0.4;
//config.dbug = true;
const fourPtGraph2 = new FourPointGraph(doc,config);
fourPtGraph2.render();

config.title = "And another thing"
delete config.yLabel;
config.boundingRect.x = config.boundingRect.x  + config.boundingRect.w*1.02;
config.boundingRect.w = effectiveArea.w*0.4;
config.boundingRect.y = effectiveArea.y + effectiveArea.h*0.2;
//config.dbug = true;
const fourPtGraph3 = new FourPointGraph(doc,config);
fourPtGraph3.render();

// Label needs to be called by the left most one.
fourPtGraph2.renderCenteredLegedBelowGraph(effectiveArea.w*0.4*1.08 + effectiveArea.w*0.4)


page.createNewReportPage();
config = {};
config.binding =  [ [0,2], [1,0]  ];
config.resolution = [ 1920, 1080];
config.boundingRect = {};
config.boundingRect.x = effectiveArea.x + 2;
config.boundingRect.y = effectiveArea.y + 2;
config.boundingRect.w = 192;
config.boundingRect.h = 108;
config.fixations = [       
   [740 ,410],
   [778 ,425],
   [689 ,622],
   [717 ,623],
   [669 ,665],
   [716 ,694],
   [1181,298],
   [1233,277],
   [1261,259],
   [1295,277],
   [1294,300],
   [1224,296],
   [1208,319],
   [1253,316],
   [1253,328]
]
//config.enable_legacy_scaling = true;

//console.log("Test Fixation")
const testFix = new FixationDrawer(doc);
testFix.render(config)


page.createNewReportPage();
config = {};
let w = effectiveArea.w*0.98;
config.boundingBox = {};
config.boundingBox.w = w;
config.boundingBox.x = effectiveArea.x + (effectiveArea.w - w)/2
config.boundingBox.y = effectiveArea.y + effectiveArea.h*0.1;
config.boundingBox.h = effectiveArea.h*0.12;
config.left_title = "Presumptive Diagnosis"
config.right_title = "A.I. Performance Index"
config.value = "74%";
config.text  = "Generalized cognitive and brain alteration. We recommend\nrepeating the Colors Test with 2 targets."
config.separator = 0.66

const pbox = new PresumptiveDiagnosisBox(doc,config);

doc.save("testbed.pdf");
