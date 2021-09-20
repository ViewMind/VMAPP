class SVGMultiLineText {

   constructor(pipe_separated_text,render_space){
      this.text_lines = pipe_separated_text.split("|");
      this.render_space = render_space;
      this.metrics = new SVGFontMetrics(render_space);
   }

   setFontDefinition(fdef) {
      this.metrics.setFontDefinition(fdef);
   }

   setFontSize(fsize){
      this.metrics.setFontSize(fsize);
   }

   setFontWeight(fweight){
      this.metrics.setFontWeight(fweight);
   }

   setFontType(ftype){
      this.metrics.setFontType(ftype);
   }

   setLineSpace(ls){
      this.line_space_percent = ls; //This is a % of the "lowest line"
   }

   getBoundingBox(){
      var lowest_line = null;
      var acc_height = 0;
      var widest_line = 0;
      
      this.line_dimensions = [];
      this.text_elements = [];
      
      for (var i in this.text_lines){
         var dim = this.metrics.getTextDimensions(this.text_lines[i])
         
         this.line_dimensions.push(dim);
         this.text_elements.push(this.metrics.getSVGTextElement(this.text_lines[i],"##X##","##Y##",true));
         
         if (dim.w > widest_line) widest_line = dim.w;
         if ((lowest_line === null) || (lowest_line < dim.h)) lowest_line = dim.h
         
         acc_height = acc_height + dim.h
      }      
      this.line_space = this.line_space_percent*lowest_line;
      acc_height = acc_height + (this.text_lines.length-1)*this.line_space;

      // Store and return the bounding box. 
      this.bouding_box = {}; this.bouding_box.w = widest_line; this.bouding_box.h = acc_height;
      return this.bouding_box;
   }

   render(x,y,halign,color){

      var svg = "";
      var yval = y;
      for (var i in this.text_elements){
         var te = this.text_elements[i];
         if (halign == "center"){
            te = te.replace("##X##",x+(this.bouding_box.w-this.line_dimensions[i].w)/2);
         }
         else if (halign == "left"){
            te = te.replace("##X##",x+(this.bouding_box.w-this.line_dimensions[i].w));
         }
         else{
            // Default is right align.
            te = te.replace("##X##",x);
         }
         te = te.replace("##Y##",yval);
         te = te.replace("#000000",color);
         svg = svg + te;
         yval = yval + this.line_dimensions[i].h + this.line_space;
      }     
      
      return svg;

   }



}