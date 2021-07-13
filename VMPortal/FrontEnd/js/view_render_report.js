var displayResults;
var studyConfiguration;
var reportReference;
var studyDate;  // required for the report generation. 

// SVG Font Definitios used are maed global for simplicity. 
var DEF_FONT_GOTHAM_BOLD;
var DEF_FONT_GOTHAM_BOOK;
var DEF_FONT_GOTHAM_MEDIUM;

// For generating the PDF. 
var svg2pdf = new SVG2PDFGenerator("report-svg","onPDFGenDone");

function loadViewRenderReport(){

   var sname = getSubjectDisplayID();
   var inst_name = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_INSTITUTION_NAME);
   var report_id = sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_REPORT);

   var navsubjlist = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.SUBJECTS;

   var navreplist = "index.html?" + GLOBALS.ROUTING.PARAMS.GOTO + "=" +  GLOBALS.ROUTING.PAGES.REPORTS;

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Report View"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": inst_name, "where_to": navsubjlist});
   breadcrumb.map.push({"name": sname, "where_to": navreplist});
   breadcrumb.map.push({"name": report_id, "where_to": ""})
   generateTrail(breadcrumb);   

   sessionStorage.setItem(GLOBALS.PAGE_COMM.SELECTED_REPORT,report_id);   

   // Generating the tool view. 
   var html = 
   '<div class = "row">\
        <div class = "col-lg 12">\
           <div class="card mb-4">\
              <div class="card-header vm-gray">\
                 <i class="fas fa-tools me-1"></i>\
                 Report View Tools\
              </div>\
              <div class="card-body">\
                 <div class="row" style="display: flex; align-items: center;">\
                    <div class="col-lg-3">\
                       <label for="lang-selection" class="form-label">Report Language Select</label>\
                       <select class="form-select" aria-label="Report Laguage Select" id="langSelector">\
                           <option value="en" selected>English</option>\
                           <option value="es">Español</option>\
                       </select>\
                    </div>\
                    <div class="col-lg-1"></div>\
                    <div class="col-lg-3">\
                       <button type="button" style = "width: 100%" class="btn btn-outline-dark" onclick="renderReport(false)">Refresh</button>\
                    </div>\
                    <div class="col-lg-1"></div>\
                    <div class="col-lg-3">\
                       <button type="button" style = "width: 100%" class="btn btn-outline-dark" id="btnPrintPDF" onclick="printReportAsPDF()">PDF</button>\
                    </div>\
                 </div>\
              </div>\
           </div>\
        </div>\
    </div>\
    <div class = "row">\
       <div class = "col-lg 12">\
          <div class="card mb-4">\
             <div class="card-header vm-gray">\
                <i class="fas fa-file-medical-alt me-1"></i>\
                Study Results\
             </div>\
             <div class="card-body" id = "svg-container">\
             </div>\
          </div>\
       </div>\
    </div>'

   document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = html;
   API.getReport(report_id,"onGetReportData");

}

function onGetReportData(response){
   if (response.message != "OK"){
      ErrorDialog.open("Server Error",response.message);
      return;
   }

   // There should only be one item. 
   results = response.data[0];

   // Getting the study type.
   var study_type = results[ACCESS.EVALUATIONS.STUDY_TYPE];
   if (!(study_type in REPORT_REFERENCE)){
      ErrorDialog.open("Unknown Study Type","Could nto find reference data for study " + study_type);
      return;
   }

   // Stored for global reference. 
   reportReference    = REPORT_REFERENCE[study_type];
   displayResults     = JSON.parse(results[ACCESS.EVALUATIONS.RESULTS]);
   studyConfiguration = JSON.parse(results[ACCESS.EVALUATIONS.STUDY_CONFIG]);
   //console.log("Study config: " + results[ACCESS.EVALUATIONS.STUDY_CONFIG]);
   studyDate          = results[ACCESS.EVALUATIONS.STUDY_DATE]

   // Getting the reference text and values
   //console.log(JSON.stringify(response));
   //console.log(REPORT_REFERENCE);
   DEF_FONT_GOTHAM_BOLD = "<style type='text/css'>@font-face {font-family: GothamBold; src: url('data:application/font-woff2;charset=utf-8;base64," + FONTS.GothamBold + "')}</style>"
   DEF_FONT_GOTHAM_BOOK = "<style type='text/css'>@font-face {font-family: GothamBook; src: url('data:application/font-woff2;charset=utf-8;base64," + FONTS.GothamBook + "')}</style>"
   DEF_FONT_GOTHAM_MEDIUM = "<style type='text/css'>@font-face {font-family: GothamMedium; src: url('data:application/font-woff2;charset=utf-8;base64," + FONTS.GothamMedium + "')}</style>"

   renderReport(false);
   
}

function printReportAsPDF(){
   svg2pdf.generatePDF();
}

function onPDFGenDone(){
   if (!svg2pdf.wasSuccessful()){
      ErrorDialog.open("PDF Generation","Unable to generate PDF");
   }
}

function renderReport(){

   // The 0.98 is patch. As the value is a bit larger than the actual with of the card container, by about 2% than the actual value I want. 
   var W = document.getElementById("svg-container").clientWidth*0.98; 
   var H = W*1.4; // A4 Aspect Ratio. 

   // Modifying the PDF on click listner
   document.getElementById("btnPrintPDF").onclick = function() { printReportAsPDF(W,H); }   

   var report = '<svg id = "report-svg" width = "' + W + '" height = "' + H + '" style = "border: solid 0px blue;">\n'      

   // Adding embbeded fonts
   var fonts = "<defs>\n   " + DEF_FONT_GOTHAM_MEDIUM + "\n    " + DEF_FONT_GOTHAM_BOOK + "\n    " + DEF_FONT_GOTHAM_BOLD + "\n</defs>"

   report = report + fonts;   
   lang = document.getElementById("langSelector").value;

   // Computing layout values
   var xMargin = W*0.05;
   var widthPageUse = W*0.90;
   var resultSegmentHeight = H*0.09;
   var topBannerHeight = H*0.12
   var nameDisplayHeight = H*0.07;
   var colorExpHeight = H*0.1;
   var divStrokeWidth = H*0.001;
   var backgroundColor = "#ffffff";

   // var header = '<image href="resources/viewmind.png" height="' + logoH +  '" width="' + logoW + '" x = "' +  + '" y = "' + + '"/>'
   var renderParameters =  {}
   renderParameters.x = xMargin;
   renderParameters.y = 0;
   renderParameters.w = widthPageUse
   renderParameters.h = topBannerHeight;
   renderParameters.backgroundColor = backgroundColor;
   renderParameters.divStokeWidth = divStrokeWidth;
   renderParameters.refFontSize = 1; 
   renderParameters.pageHeight = H;
   report = report + renderHeader(renderParameters);

   renderParameters.y = topBannerHeight;
   renderParameters.h = nameDisplayHeight;

   report = report + renderReportNames(renderParameters)
   
   renderParameters.y = topBannerHeight+nameDisplayHeight
   renderParameters.h = colorExpHeight;
   
   report = report + renderColorReference(renderParameters)

   // Rendering the title of the study. 
   var titleText =  REPORT_REFERENCE[lang][ACCESS.REPTEXT.RESULTS] + ": " + reportReference["study_title_" + lang];
   ///console.log(titleText)
   var metrics = new SVGFontMetrics("svg-container");
   metrics.setFontSize(0.014*H + "px");
   metrics.setFontDefinition(DEF_FONT_GOTHAM_BOLD);
   
   var bb = metrics.getTextDimensions(titleText);
   //console.log(bb);
   var air_title = bb.h*0.8;
   var xtitle = xMargin + (widthPageUse - bb.w)/2;
   var ytitle = topBannerHeight + nameDisplayHeight + colorExpHeight + air_title;
   var titleElement = metrics.getSVGTextElement(titleText,xtitle,ytitle,true);
   titleElement.replace("#000000","#505150");

   report = report + titleElement + "\n";

   // Adding the divisor.
   var liney = ytitle + bb.h + air_title - divStrokeWidth;
   report = report + '<line x1="'+ xMargin + '" y1="' + liney + '" x2="' + (xMargin + widthPageUse) + '" y2=" ' + liney + ' " style="stroke:#4f504f;stroke-width:' + divStrokeWidth + '" />\n';
   
   // Drawing each result segment. 
   var yval = ytitle + bb.h + air_title;

   for (key in reportReference){

      if (key.includes("study_title")) continue;

      var ref = reportReference[key]

      // We check if this a segment bar that we actually need to draw.
      if (!validForConfiguration(ref)) continue;

      var rs = new ResultSegment();
      rs.segmentRefereceText = ref[ACCESS.RESSEG.RANGE_TEXT + lang];
      rs.segmentTitle        = ref[ACCESS.RESSEG.TITLE + lang];
      rs.segmentSubtitle     = ref[ACCESS.RESSEG.EXPLANATION + lang];

      // Firguring out if its better higher or lower. 
      var smallerBetter = ref[ACCESS.RESSEG.SMALLER_BETTER];
      if (smallerBetter == "false") rs.isHigherBetter = true;
      else rs.isHigherBetter = false;

      // By the default these two values are false. So we changed them only if necessary. 
      if (ref[ACCESS.RESSEG.MULT100] == "true") rs.multBy100 = true;
      if (ref[ACCESS.RESSEG.ROUND_FOR_DISP] == "true") rs.roundForDisplay = true;

      // The actual value to display from the results. 
      rs.value = displayResults[ref[ACCESS.RESSEG.RES_KEY]];
      
      // The cutoff values in the original description files always had two more values than necessary. 
      rs.valueBreakPoints = ref[ACCESS.RESSEG.CUTOFFVALS].split("|");

      // The first and last of the cutoff values are not necessary. 
      rs.valueBreakPoints.shift();
      rs.valueBreakPoints.pop();

      rs.x = xMargin;
      rs.y = yval;
      rs.w = widthPageUse;
      rs.h = resultSegmentHeight;
      
      rs.backgroundColor = backgroundColor;

      yval = yval + rs.h + rs.h*0.02; // The 2% is to provide some space between the blocks. 
   
      // Line breakes are designated with |. 
      rs.segmentSubtitle = rs.segmentSubtitle.replace("|","\n")

      report = report + rs.render();

   }

   report = report + "</svg>"   

   //console.log(report);

   document.getElementById("svg-container").innerHTML = report;

}

function renderHeader(rp){
   
   var svg = "";
   svg = svg + '<rect x="' + rp.x + '" y = "' + rp.y + '" width = "' + rp.w + '" height = "' + rp.h + '" fill = "' + rp.backgroundColor + '"/>\n'   

   // Adding the logo
   var logoW = rp.w*0.2;
   svg = svg + '<image href="resources/viewmind.png" width="' + logoW + '" height = "' + rp.h + '" x = "' + rp.x + '" y = "' + rp.y + '"/>\n'

   // Adding the Motto.
   var lang = document.getElementById("langSelector").value;
   motto = new SVGMultiLineText(REPORT_REFERENCE[lang][ACCESS.REPTEXT.MOTTO],"svg-container");
   motto.setFontSize(0.012*rp.pageHeight + "px")
   motto.setFontDefinition(DEF_FONT_GOTHAM_BOLD)
   motto.setLineSpace(0.4);

   // We get the bounding box
   var motto_bb = motto.getBoundingBox();
   var ymotto = rp.y + (rp.h-motto_bb.h)/2;
   var xmotto = rp.w - motto_bb.w - rp.w*0.001; // Giving it a bit of right side margin. 
   var fontColor = "#628dae"
   svg = svg + motto.render(xmotto,ymotto,"right",fontColor)

   // Adding the bar besides the Motto. 
   var linewidth = 0.003*rp.w
   var linex = xmotto - linewidth*4;
   var lineStart = ymotto -  motto_bb.h*0.15;
   var lineEnd   = ymotto +  motto_bb.h + motto_bb.h*0.15;
   svg = svg + '<line x1="'+ linex + '" y1="' + lineStart + '" x2="' + linex + '" y2=" ' + lineEnd + ' " style="stroke:' + fontColor + ';stroke-width:' + linewidth + '" />\n';

   // Adding the divisor.
   var liney = (rp.y+rp.h)-rp.divStokeWidth;
   svg = svg + '<line x1="'+ rp.x + '" y1="' + liney + '" x2="' + (rp.x + rp.w) + '" y2=" ' + liney + ' " style="stroke:#4f504f;stroke-width:' + rp.divStokeWidth + '" />\n';
   return svg;

}

function renderReportNames(rp){
   var svg = "";
   // Background rect. 
   svg = svg + '<rect x="' + rp.x + '" y = "' + rp.y + '" width = "' + rp.w + '" height = "' + rp.h + '" fill = "' + rp.backgroundColor + '"/>\n'   

   metrics = new SVGFontMetrics("svg-container");
   metrics.setFontSize(0.010*rp.pageHeight + "px")
   metrics.setFontDefinition(DEF_FONT_GOTHAM_BOLD);

   // The color of the text
   var textColor = "#505150"

   // The middle.
   var ymiddle = rp.y + rp.h/2;   
   
   // Required for getting th patient age 
   var subjectData = JSON.parse(sessionStorage.getItem(GLOBALS.PAGE_COMM.SELECTED_SUBJECT));

   // Rendering the subjects name
   var te;
   var lang = document.getElementById("langSelector").value;
   var textName = REPORT_REFERENCE[lang][ACCESS.REPTEXT.PATIENT] + ": " + getSubjectDisplayID();   
   var bb = metrics.getTextDimensions(textName);
   var vair = bb.h*0.3;
   te = metrics.getSVGTextElement(textName,rp.x,ymiddle - vair - bb.h,true);
   te = te.replace("#000000",textColor);
   svg = svg + te + "\n";

   // Rendering the requesting doctor's name.
   var fname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_FNAME);
   var lname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_LNAME);
   var textDoctor = REPORT_REFERENCE[lang][ACCESS.REPTEXT.REQDR] + ": " + lname + ", " + fname;
   bb = metrics.getTextDimensions(textDoctor);
   te = metrics.getSVGTextElement(textDoctor,rp.x,ymiddle + vair,true);
   te = te.replace("#000000",textColor);
   svg = svg + te + "\n";

   // Rendering the date.   
   var textDate = REPORT_REFERENCE[lang][ACCESS.REPTEXT.DATE] + ": " + displayDate(studyDate)   
   bb = metrics.getTextDimensions(textDate);
   var offsetw = bb.w*1.1;
   te = metrics.getSVGTextElement(textDate,rp.x + rp.w - offsetw,ymiddle + vair,true);
   te = te.replace("#000000",textColor);
   svg = svg + te + "\n";


   // Rendering the age
   var textAge = REPORT_REFERENCE[lang][ACCESS.REPTEXT.AGE] + ": " + subjectData.age; 
   bb = metrics.getTextDimensions(textAge);
   te = metrics.getSVGTextElement(textAge,rp.x + rp.w - offsetw,ymiddle - vair - bb.h,true);
   te = te.replace("#000000",textColor);
   svg = svg + te + "\n";

   // Adding the divisor.
   var liney = (rp.y+rp.h)-rp.divStokeWidth;
   svg = svg + '<line x1="'+ rp.x + '" y1="' + liney + '" x2="' + (rp.x + rp.w) + '" y2=" ' + liney + ' " style="stroke:#4f504f;stroke-width:' + rp.divStokeWidth + '" />\n';
   return svg;
}

function renderColorReference(rp){
   var svg = "";
   // Background rect. 
   svg = svg + '<rect x="' + rp.x + '" y = "' + rp.y + '" width = "' + rp.w + '" height = "' + rp.h + '" fill = "' + rp.backgroundColor + '"/>\n'   

   // The color of the text
   var textColor = "#505150"   

   // The intro line.
   var te;
   var lang = document.getElementById("langSelector").value;
   var colorIntro = REPORT_REFERENCE[lang][ACCESS.REPTEXT.COLORINTRO];   
   var bb = metrics.getTextDimensions(colorIntro);
   var vair = bb.h*0.8;
   te = metrics.getSVGTextElement(colorIntro,rp.x,rp.y + vair,true);
   te = te.replace("#000000",textColor);
   svg = svg + te + "\n";

   var colors = [ "#00557f", "#049948", "#e5b42c", "#cb2026" ];
   var texts  = [REPORT_REFERENCE[lang][ACCESS.REPTEXT.BLUE], 
                 REPORT_REFERENCE[lang][ACCESS.REPTEXT.GREEN],
                 REPORT_REFERENCE[lang][ACCESS.REPTEXT.YELLOW],
                 REPORT_REFERENCE[lang][ACCESS.REPTEXT.RED]];

   var mlines = [];
   
   var spaceBetween = rp.w*0.08;
   var spaceBetweenSquareAndText = rp.w*0.01;
   var xval = rp.x;
   var yval = rp.y + vair + bb.h + vair;
   var square_side = rp.h*0.2;

   // Computing the greatest height. 
   var greatestHeight = 0;
   for (var i = 0; i < colors.length; i++){
      var mlinet = new SVGMultiLineText(texts[i],"svg-container");
      mlinet.setFontSize(0.010*rp.pageHeight + "px")
      mlinet.setFontDefinition(DEF_FONT_GOTHAM_BOLD)
      mlinet.setLineSpace(0.4);   
      mlines.push(mlinet);
      bb = mlinet.getBoundingBox();
      if (greatestHeight < bb.h) greatestHeight = bb.h;
   }

   // Increasing it by about 20%
   greatestHeight = greatestHeight*1.2;

   //svg = svg + '<rect x="' + rp.x + '" y = "' + yval + '" width = "' + rp.w + '" height = "' + greatestHeight + '" fill = "#dddddd"/>\n'

   // Actually doing the rendering
   var squarey = yval + (greatestHeight/2) - square_side/2;
   for (var i = 0; i < colors.length; i++){
      svg = svg + '<rect x="' + xval + '" y = "' + squarey + '" width = "' + square_side + '" height = "' + square_side + '" fill = "' + colors[i] + '"/>\n'
      var mlinet = mlines[i];
      bb = mlinet.getBoundingBox();
      te = mlinet.render(xval + square_side + spaceBetweenSquareAndText,
                         yval + (greatestHeight/2) - bb.h/2,
                         "right",textColor)
      svg = svg + te + "\n";
      xval = xval + spaceBetweenSquareAndText + bb.w + spaceBetween;
   }

   // Adding the divisor.
   var liney = (rp.y+rp.h)-rp.divStokeWidth;
   svg = svg + '<line x1="'+ rp.x + '" y1="' + liney + '" x2="' + (rp.x + rp.w) + '" y2=" ' + liney + ' " style="stroke:#4f504f;stroke-width:' + rp.divStokeWidth + '" />\n';
   return svg;
}

/**
 * @brief The function will check that ANY of the parameters of the study configuration are present in the refsettings. If they are they need to match to return true
 * otherwise, it will return false. If NONE are present it will also return true. It will return 
 * @param {The reference parameters to configure a result segment} refsettings 
 */

function validForConfiguration(refsettings){
   for (study in studyConfiguration){
      //console.log("Study" + study);
      for (key in studyConfiguration[study]){
         //console.log("Checking if key " + key + " is in " + JSON.stringify(refsettings));
         if (key in refsettings){
            //console.log("Comparing " + studyConfiguration[study][key] + " with " + refsettings[key]);
            if (studyConfiguration[study][key] != refsettings[key]){
               // Only one mismatch is enought to return false. 
               return false;
            }
         }
      }
   }
   return true;
}