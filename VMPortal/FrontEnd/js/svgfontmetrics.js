class SVGFontMetrics {

   constructor(renderdiv){
      this.renderdiv = renderdiv;
      this.font_size = "";
      this.font_size = "";
      this.font_type = "";
      this.font_definition = "";
      this.font_weight = "";
   }

   reset(){
      this.font_size = "";
      this.font_type = "";
      this.font_definition = "";
      this.font_weight = "";
   }

   setFontSize(fsize){
      this.font_size = fsize;
   }

   setFontDefinition(fdef){
      this.font_definition = fdef;

      // Attemting to set the font type as well. 
      var searchFor = "font-family:";
      var index = fdef.indexOf(searchFor);
      if (index == -1) return; // font type is left as is 
      index = index + searchFor.length;
      var ftype = "";
      var boolStarted = false;
      for (var i = index; i < fdef.length; i++){
         if (fdef[i] == " "){ // Ignore spaces. 
            if (boolStarted) break;
            else continue;
         }
         else{
            boolStarted = true;
            if (fdef[i] == ";") break;
            else ftype = ftype + fdef[i];
         }
      }

      if (ftype.length > 3) // No font family is less than 3 letters.
      this.font_type = ftype;
   }

   setFontType(ftype){
      this.font_type = ftype;
   }

   // Getters
   getFontType(){
      return this.font_type;
   }

   getFontSize(){
      return this.font_size;
   }

   getFontWeight(){
      return this.font_weight;
   }

   setFontWeigth(fweight){
      this.font_weight = fweight;
   }

   getTextDimensions(text) {

      if ((text === undefined) || (text == "")){
         console.log("SVGMetrics: No Text to measure")
         var ans = {}; ans.w = 0; ans.h = 0;
         return ans;
      }

      var svg = '<svg width = "1000" height = "1000">'
          
      if (this.font_definition != ""){
         svg = svg + "   <defs>\n      " + this.font_definition + "\n   </defs>"
      }

        
      svg = svg + this.getSVGTextElement(text,0,0,false) + '</svg>';

      return this.getMeasurmentsOnElement(svg,"mtext");

   }

   getSVGTextElement(text,x,y,noid){
      var id = 'id="mtext"';
      if (noid) id = "";

      var text_element = '   <text ' + id + ' x="' + x + '" y = "' + y + '"';
      if (this.font_size != ""){
         text_element = text_element + ' font-size="' + this.font_size + '" ';
      }
      if (this.font_type != ""){
         text_element = text_element + ' font-family="' + this.font_type + '" ';
      }
      if (this.font_weight != ""){
         text_element = text_element + ' font-weight="' + this.font_weight + '" ';
      } 
      text_element = text_element + ' dominant-baseline="hanging" ';
      text_element = text_element + ' fill="#000000" ';
      text_element = text_element + ">" + text + "</text>\n";
      return text_element
   }

   getMeasurmentsOnElement(svg, elementName){
      document.getElementById(this.renderdiv).innerHTML = svg;
      var textNode = document.querySelector("#" + elementName);
      var bb = textNode.getBBox();
      document.getElementById(this.renderdiv).innerHTML = "";
      var ans = {}; ans.w = bb.width; ans.h = bb.height;
      return ans;      
   }

} 
