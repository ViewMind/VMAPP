const FONT_REFERENCE_VALUE_EM = 2.1;
const FONT_COLOR              = "#505150";
const CENTER_LINE_FOR_SUB_POS = 0.45; //% of height. 
const AIR_TOP                 = 0.05 //% of height.
const AIR_BOTTOM              = 0.05 //% of height.
const AIR_BETWEEN_LINES       = 0.03 //% of height
const AIR_MARGIN_LEFT         = 0.02 //% of width
const AIR_MARGIN_RIGHT        = 0.02 //% of width

class ResultSegment {

   // Information properties. 
   segmentTitle =  "";
   segmentSubtitle =  "";
   segmentRefereceText = "";
   value = 0;   

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
      svg = svg + '   <rect x="' + this.x + '" y = "' + this.y + '" width = "' + this.w + '" height = "' + this.h + '" fill = "#eeeeee"/>\n'
      var x,y;

      // Rendering the subtitle
      var font = ' font: ' + (FONT_REFERENCE_VALUE_EM) + 'em GothamBold '
      x = this.x + this.w*AIR_MARGIN_LEFT;
      y = this.y + this.h*AIR_TOP;
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' +  font + '" dominant-baseline = "hanging" fill = "' + FONT_COLOR + '">' + this.segmentTitle  + '</text>\n'

      // segment subtitle might be 1 or two lines. Checking. 
      font = ' font: ' + (FONT_REFERENCE_VALUE_EM*0.7) + 'em GothamBook '
      var subtitle_lines = this.segmentSubtitle.split("\n");
      if (subtitle_lines.length == 1){
         x = this.x + this.w*AIR_MARGIN_LEFT;
         y = this.y + this.h*CENTER_LINE_FOR_SUB_POS
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "middle" fill = "' + FONT_COLOR + '">' + this.segmentSubtitle  + '</text>\n'         
      }
      else{
         x = this.x + this.w*AIR_MARGIN_LEFT;
         y = this.y + this.h*CENTER_LINE_FOR_SUB_POS - AIR_BETWEEN_LINES*this.h;
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "auto" fill = "' + FONT_COLOR + '">' + subtitle_lines[0]  + '</text>\n'         
         y = this.y + this.h*CENTER_LINE_FOR_SUB_POS + AIR_BETWEEN_LINES*this.h;
         svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "hanging" fill = "' + FONT_COLOR + '">' + subtitle_lines[1]  + '</text>\n'         
      }

      // The reference text. 
      x = this.x + this.w*AIR_MARGIN_LEFT;
      y = this.y + this.h - this.h*AIR_BOTTOM
      font = ' font: ' + (FONT_REFERENCE_VALUE_EM*0.8) + 'em GothamBold '
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "auto" fill = "' + FONT_COLOR + '">' + this.segmentRefereceText  + '</text>\n'

      // Rendering the value
      x = this.x + this.w - this.w*AIR_MARGIN_RIGHT;
      y = this.y + this.h/2;
      font = ' font: ' + (FONT_REFERENCE_VALUE_EM*2) + 'em GothamBold '
      svg = svg + '   <text x = "' + x + '" y = "' + y + '" style = "' + font + '" dominant-baseline = "middle" fill = "' + FONT_COLOR + '" text-anchor="end">' + this.value  + '</text>\n'


      // Now drawing the result bar. 
      var resBar = {};
      resBar.w = this.w*0.25
      resBar.h = this.h*0.2;
      resBar.x = this.x + this.w*0.6
      resBar.y = this.y + this.h/2 - resBar.h/2
      resBar.n = this.valueBreakPoints.length+1;
      resBar.indicator = ResultBar.computeSegmentIndicator(this.value,this.valueBreakPoints,this.isHigherBetter);

      svg = svg + ResultBar.resultBar(resBar);

      //console.log(svg)
      return svg;

   }


}