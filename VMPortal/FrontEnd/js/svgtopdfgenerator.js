

class SVG2PDFGenerator {

   #svg_element_id;
   #svg_image_element_index;
   #images;
   #success;
   #callback;

   /**
    * 
    * @param {The SVG Element to be Turned into PDF} svg_element_id 
    * @param {Function that is calleed when the PDF Generation finishes} callback 
    */

   constructor(svg_element_id,callback){
      this.#svg_element_id = svg_element_id;
      this.#callback = callback;
   }

   generatePDF(){
      this.#images = document.getElementById(this.#svg_element_id).querySelectorAll("image");
      this.#svg_image_element_index = -1;
      this.#success = false;
      this.nextImage(false);
   }

   getCallback(){
      return this.#callback;
   }

   setSuccess(){
      this.#success = true;
   }

   wasSuccessful(){
      return this.#success;
   }

   nextImage(failure_ocurred){
      
      // If something failed there is nothing else to do. 
      if (failure_ocurred){
         window[svg2pdf_handle.getCallback()]();
         return;
      }

      // The next image is turned to URL Data. 
      this.#svg_image_element_index++;
      if (this.#svg_image_element_index < this.#images.length){
         console.log("Transforming image " + this.#svg_image_element_index);
         this.imageToDataURL(this.#images[this.#svg_image_element_index]);
      }
      else{
         var svg = document.getElementById(this.#svg_element_id); 

         //console.log(svg);

         var W = svg.getAttribute("width");
         var H = svg.getAttribute("height");

         //console.log("WxH: " + W + "x" + H);

         var svgData = new XMLSerializer().serializeToString(svg);
         var svgURL = 'data:image/svg+xml; charset=utf8, ' + encodeURIComponent(svgData);

         var svgImg = new Image();
         var svg2pdf_handle = this;

         svgImg.onload = function () {
            console.log("Loaded SVG")
            var canvas =  document.createElement('canvas');
            //var canvas =  document.getElementById('thecanvas');
            canvas.width = W;
            canvas.height = H;      
            canvas.getContext('2d').drawImage(svgImg, 0, 0, canvas.width, canvas.height);
            let doc = new jsPDF();
            var PAGE_WIDTH  = doc.internal.pageSize.getWidth();
            var PAGE_HEIGHT = doc.internal.pageSize.getHeight();      
            doc.addImage(canvas, 'PNG', 0, 0, PAGE_WIDTH, PAGE_HEIGHT,"","FAST");
            svg2pdf_handle.setSuccess();
            doc.save("report.pdf");  
            window[svg2pdf_handle.getCallback()]();
         }

         svgImg.onerror = function(){
            window[svg2pdf_handle.getCallback()]();
            return;               
         }
      
         svgImg.src = svgURL;
            
      }

   }

   imageToDataURL(image){
      
      var img = new Image();
      var svg2pdfGen = this;
   
      img.onload = function(){
         // we should now be able to draw it without tainting the canvas
         var canvas = document.createElement('canvas');
         canvas.width = this.width;
         canvas.height = this.height;
         // draw the loaded image
         canvas.getContext('2d').drawImage(this, 0, 0);
         // set our <image>'s href attribute to the dataURL of our canvas
         image.setAttributeNS("", 'href', canvas.toDataURL());          
         console.log("Calling next image");
         svg2pdfGen.nextImage(false);
      }
   
      // Catching the error if there is one, for loading the image. 
      img.onerror = function () {
         console.warn('failed to load an image at : ', this.src);
         nextImage(true);
      };
   
      img.src = image.getAttributeNS("", 'href');
   
   }

}
