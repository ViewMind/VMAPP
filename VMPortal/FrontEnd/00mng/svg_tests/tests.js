function generateSVG(){
   var W = 1000
   var H = W*1.4;
   var svg = '<svg id = "report-svg" width = "' + W + '" height = "' + H + '" style = "border: solid 1px blue;">\n'      
   svg = svg + "<defs>\n" + DEF_FONT_GOTHAM_BOLD + "\n" +  DEF_FONT_GOTHAM_BOOK + "\n</defs>"
   svg = svg + '<rect x="0" y="0" width="' + W + '" height="' + H + '" fill="#546a6b"></rect>\n'
   svg = svg + '<text x="100" y="100" font-size="30px" font-family="GothamBold" fill="#eeeeee">This is Gotham Bold</text>\n'
   svg = svg + '<text x="100" y="300" font-size="20px" font-family="GothamBook" fill="#eeeeee">This is Gotham Book</text>\n'
   svg = svg + '<image id = "theimage" href="viewmind.png" width = "1000" height = "1400" x = "0" y="0"/>'//</image>'
   svg = svg + '</svg>'
   document.getElementById("container").innerHTML = svg;
}

function genPDF(){
   //var svg = document.getElementById("report-svg");   

   var image_element = document.getElementById("theimage");
   toDataURL(image_element);

   // var svgData = new XMLSerializer().serializeToString( svg );

   // //console.log(svgData)

   // var canvas = document.createElement("canvas");
   // canvas.width = 1000;
   // canvas.height = 1400;
   // var ctx = canvas.getContext("2d");

   // // Internally display the image. 
   // var img = document.createElement( "img" );

   // //console.log(btoa(svgData));
    
   // img.setAttribute( "src", "data:image/svg+xml;base64," + btoa( svgData ) );
   
   // img.onload = function() {
   //    console.log("onload");
   //    ctx.drawImage( img, 0, 0 );
   //    var imgData = canvas.toDataURL('image/png');
   //    let doc = new jsPDF();
   //    var PAGE_WIDTH  = doc.internal.pageSize.getWidth();
   //    var PAGE_HEIGHT = doc.internal.pageSize.getHeight();      
   //    doc.addImage(imgData, 'PNG', 0, 0, PAGE_WIDTH, PAGE_HEIGHT,"","FAST");
   //    doc.save("report.pdf");  
   // };   

   // img.onerror = function() {
   //    console.log("There was an error loading the image");
   // }   
}

function toDataURL(image){

   console.log("image: ");
   console.log(image);
   
   var xlinkNS = "http://www.w3.org/1999/xlink";
   var img = new Image();

   img.onload = function(){
      console.log("Loading on To data URL");
      // we should now be able to draw it without tainting the canvas
      var canvas = document.createElement('canvas');
      canvas.width = this.width;
      canvas.height = this.height;
      // draw the loaded image
      canvas.getContext('2d').drawImage(this, 0, 0);
      // set our <image>'s href attribute to the dataURL of our canvas
      image.setAttributeNS(xlinkNS, 'href', canvas.toDataURL());    
      //exportDoc();  
   }

   // No CORS set in the response      
   img.onerror = function () {
      console.log("Error on To data URL");
      // save the src
      var oldSrc = this.src;
      // there is an other problem
      this.onerror = function () {
          console.warn('failed to load an image at : ', this.src);
      };
      // retry
      this.src = '';
      this.src = oldSrc;
   };

   //console.log(image.getAttributeNS("",'href'));

   img.src = image.getAttributeNS("", 'href');
}

function exportDoc(){

   var svg = document.getElementById("report-svg");   
   var svgData = new XMLSerializer().serializeToString( svg );

   //console.log(svgData)

   var canvas = document.createElement("canvas");
   canvas.width = 1000;
   canvas.height = 1400;
   var ctx = canvas.getContext("2d");

   // Internally display the image. 
   var img = document.createElement( "img" );

   //console.log(btoa(svgData));
    
   img.setAttribute( "src", "data:image/svg+xml;base64," + btoa( svgData ) );
   
   img.onload = function() {
      console.log("onload");
      ctx.drawImage( img, 0, 0 );
      var imgData = canvas.toDataURL('image/png');
      let doc = new jsPDF();
      var PAGE_WIDTH  = doc.internal.pageSize.getWidth();
      var PAGE_HEIGHT = doc.internal.pageSize.getHeight();      
      doc.addImage(imgData, 'PNG', 0, 0, PAGE_WIDTH, PAGE_HEIGHT,"","FAST");
      doc.save("report.pdf");  
   };   

   img.onerror = function() {
      console.log("There was an error loading the image");
   }      

}