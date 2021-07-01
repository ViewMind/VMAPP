var RATIOS = {}
RATIOS.RESBAR_W = 0.3;
RATIOS.RESBAR_H = 0.05;
RATIOS.REPORT_WIDTH  = 0.8; // Of the window width.
RATIOS.REPORT_HEIGHT = 1.4; // Of the report width


function pdfTest(){

   let doc = new jsPDF();
   doc.text("Hello world!", 10, 10);
   
   ctx = doc.canvas.getContext();
   var W  = doc.internal.pageSize.getWidth();
   var H = doc.internal.pageSize.getHeight();
   drawToCanvas(ctx,W,H);

   doc.save("a4.pdf");   
}

function drawReport(){

   W = window.innerWidth*RATIOS.REPORT_WIDTH;
   H = W*RATIOS.REPORT_HEIGHT;

   var report = '<svg id = "report-svg" width = "' + W + '" height = "' + H + '" style = "border: solid 1px blue;">\n'   
   var rs = new ResultSegment();

   // Adding embbeded fonts
   var fonts = "\
   <defs>\n\
      <style type='text/css'>@font-face {font-family: GothamBold; src: url('data:application/font-woff2;charset=utf-8;base64," + FONTS.GothamBold + "')}</style>\n\
      <style type='text/css'>@font-face {font-family: GothamBook; src: url('data:application/font-woff2;charset=utf-8;base64," + FONTS.GothamBook + "')}</style>\n\
   </defs>\n";

   report = report + fonts;

   rs.segmentRefereceText = "Normal value GREATER than 8";
   rs.value = 6;
   rs.segmentTitle = "RETRIEVAL MEMORY";
   rs.segmentSubtitle = "Dorsolaterla prefrontla cortex,\nHippocampus,Subiculum";
   rs.x = W*0.05;
   rs.y = H*.01;
   rs.w = W*0.9;
   rs.h = H*0.15;
   rs.valueBreakPoints = [5, 8]
   rs.isHigherBetter = true;

   report = report + rs.render();

   report = report + "</svg>"
   //console.log(report);
   document.getElementById("reportDiv").innerHTML = report;

   printReportAsPDF("report-svg",W,H)

}


function printReportAsPDF(report_id, W,H){
   var svg = document.getElementById(report_id);
   var svgData = new XMLSerializer().serializeToString( svg );

   var canvas = document.createElement("canvas");
   canvas.width = W;
   canvas.height = H;
   var ctx = canvas.getContext("2d");
   
   // Internally display the image. 
   var img = document.createElement( "img" );
   img.setAttribute( "src", "data:image/svg+xml;base64," + btoa( svgData ) );
   
   img.onload = function() {
      ctx.drawImage( img, 0, 0 );
      var imgData = canvas.toDataURL('image/png');
      let doc = new jsPDF();
      var PAGE_WIDTH  = doc.internal.pageSize.getWidth();
      var PAGE_HEIGHT = doc.internal.pageSize.getHeight();      
      doc.addImage(imgData, 'PNG', 0, 0, PAGE_WIDTH, PAGE_HEIGHT,"","FAST");
      doc.save("a4.pdf");  
   };   
}