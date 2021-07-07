
class ResultSegment {

   // Information properties. 
   segmentTitle =  "";
   segmentSubtitle =  "";
   segmentRefereceText = "";
   value = 0;   
   multBy100 = false;
   roundForDisplay = false;
   backgroundColor = "#eeeeee";
   divStrokeWidth = 1;

   // Some constants. They are no supposed to be changed. 
   #FONT_COLOR              = "#505150";
   #CENTER_LINE_FOR_SUB_POS = 0.45; //% of height. 
   #AIR_TOP                 = 0.08 //% of height.
   #AIR_BOTTOM              = 0.08 //% of height.
   #AIR_BETWEEN_LINES       = 0.03 //% of height
   #AIR_MARGIN_LEFT         = 0.02 //% of width
   #AIR_MARGIN_RIGHT        = 0.02 //% of width   

   // Drawing specifications. Should be overwritten. 
   x = 0;
   y = 0;
   w = 100;
   h = 100;

   // Result bar specifications. Should be overwrittent. 
   valueBreakPoints = [];
   isHigherBetter = false;

   /**
    * Draws the segment values. 
    */
   render() {

      var svg = "";
      svg = svg + '   <rect x="' + this.x + '" y = "' + this.y + '" width = "' + this.w + '" height = "' + this.h + '" fill = "' + this.backgroundColor + '"/>\n'
      var x,y;

      // Rendering the title
      var font_base_size = (this.h)*0.1;
      var font = ' font: ' + font_base_size+ 'px GothamBold '
      x = this.x + this.w*this.#AIR_MARGIN_LEFT;
      y = this.y + this.h*this.#AIR_TOP;
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' +  font + '" dominant-baseline = "hanging" fill = "' + this.#FONT_COLOR + '">' + this.segmentTitle  + '</text>\n'

      // segment subtitle might be 1 or two lines. Checking. 
      font = ' font: ' + (font_base_size*0.7) + 'px GothamBook '
      var subtitle_lines = this.segmentSubtitle.split("\n");
      if (subtitle_lines.length == 1){
         x = this.x + this.w*this.#AIR_MARGIN_LEFT;
         y = this.y + this.h*this.#CENTER_LINE_FOR_SUB_POS
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "middle" fill = "' + this.#FONT_COLOR + '">' + this.segmentSubtitle  + '</text>\n'         
      }
      else{
         x = this.x + this.w*this.#AIR_MARGIN_LEFT;
         y = this.y + this.h*this.#CENTER_LINE_FOR_SUB_POS - this.#AIR_BETWEEN_LINES*this.h;
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "auto" fill = "' + this.#FONT_COLOR + '">' + subtitle_lines[0]  + '</text>\n'         
         y = this.y + this.h*this.#CENTER_LINE_FOR_SUB_POS + this.#AIR_BETWEEN_LINES*this.h;
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "hanging" fill = "' + this.#FONT_COLOR + '">' + subtitle_lines[1]  + '</text>\n'         
      }

      // The reference text. 
      x = this.x + this.w*this.#AIR_MARGIN_LEFT;
      y = this.y + this.h - this.h*this.#AIR_BOTTOM
      font = ' font: ' + (font_base_size*0.8) + 'px GothamBold '
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "auto" fill = "' + this.#FONT_COLOR + '">' + this.segmentRefereceText  + '</text>\n'

      // Rendering the value
      x = this.x + this.w - this.w*this.#AIR_MARGIN_RIGHT;
      y = this.y + this.h/2;
      font = ' font: ' + (font_base_size*2) + 'px GothamBold '
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "middle" fill = "' + this.#FONT_COLOR + '" text-anchor="end">' + this.displayValue()  + '</text>\n'

      // Now drawing the result bar. 
      var resBar = {};
      resBar.w = this.w*0.25
      resBar.h = this.h*0.2;
      resBar.x = this.x + this.w*0.5
      resBar.y = this.y + this.h/2 - resBar.h/2
      resBar.n = this.valueBreakPoints.length+1;
      resBar.indicator = ResultBar.computeSegmentIndicator(this.value,this.valueBreakPoints,this.isHigherBetter);

      svg = svg + ResultBar.resultBar(resBar);

      // Finally we add the divisor at the end. 
      var liney = this.y - this.divStrokeWidth
      svg = svg + '<line x1="'+ this.x + '" y1="' + liney + '" x2="' + (this.x + this.w) + '" y2=" ' + liney + ' " style="stroke:#4f504f;stroke-width:' + this.divStrokeWidth + '" />\n';      

      //console.log(svg)
      return svg;

   }

   displayValue(){

      var fvalue = parseFloat(this.value);
      if (this.multBy100) fvalue = fvalue*100;
      if (this.roundForDisplay) fvalue = Math.round(fvalue);
      else fvalue = Math.round(fvalue * 10) / 10
      return fvalue

   }


}