var COLORS = {};
COLORS.RBAR_GREEN  = "#049948";
COLORS.RBAR_RED    = "#cb2026";
COLORS.RBAR_YELLOW = "#e5b42c";
COLORS.RBAR_BLUE   = "#00557f";

var ResultBar = {

   /**
    * @param {Configuration array} resBar 
    * resBar should contain the following.
    * x: X coordinate of the upper left corner of the bounding rectangle of the entire bar. 
    * y: Y coordinate fo the upper left corner of the bounding rectangle of the entire bar. 
    * w: The width fo the bounding rectangle
    * h: The height of the bounding rectangle. 
    * n: Number of segments for bar (can be 2,3 or 4)
    * indicator: Which segment does the indicator show. 
    */
   
   resultBar: function (resBar){

      // Constants to define the drawing of the bar.
      K_INDICATOR_HEIGHT     = 0.2; // % of the drawing area height.
      K_INDICATOR_WIDTH      = 1.2; // % of the indicator height. 
      K_INDICATOR_LINE_WIDTH = 0.01; // % of the segment size. 

      var svg = "";

      var colors = [];
      var last_color_index = 0;
      if (resBar.n === 2){
         colors = [ COLORS.RBAR_GREEN, COLORS.RBAR_RED ];
         last_color_index = 1;
      }
      else if (resBar.n === 3){
         colors = [ COLORS.RBAR_GREEN, COLORS.RBAR_YELLOW, COLORS.RBAR_RED ];
         last_color_index = 2;
      }
      else if (resBar.n === 4){
         colors = [ COLORS.RBAR_BLUE, COLORS.RBAR_GREEN, COLORS.RBAR_YELLOW, COLORS.RBAR_RED ];
         last_color_index = 3;
      }
      else{
         console.log("Invalid number of segments for result bar of " + resBar.n);
         return;
      }

      // Computing all necessary values. 
      var indicator = {};
      var segment_size = resBar.w/resBar.n;
      var bar_height = resBar.h*(1 - K_INDICATOR_HEIGHT*2);
      var circle_R = bar_height/2;
      var segment_endpoint_straight_length = segment_size - circle_R;

      // Indicator dimensions   
      indicator.h = resBar.h*K_INDICATOR_HEIGHT;
      indicator.w = indicator.h;
      indicator.bh = bar_height;
      indicator.bw = segment_size*K_INDICATOR_LINE_WIDTH;
      indicator.y = resBar.y + indicator.h;
      
      indicator.w = indicator.h*K_INDICATOR_WIDTH

      // We draw the left end circle. 
      var x_circle = resBar.x + circle_R;
      var y_circle = resBar.y + indicator.h + circle_R;
      svg = svg + '   <circle cx = "' + x_circle + '" cy = "' + y_circle + '" r = "' + circle_R + '" fill = "' + colors[0] + '"/>\n'

      // We draw the right end circle.
      x_circle = resBar.x + resBar.w - circle_R;
      svg = svg + '   <circle cx = "' + x_circle + '" cy = "' + y_circle + '" r = "' + circle_R + '" fill = "' + colors[last_color_index] + '" />\n'

      // Now we draw all the segments.
      var bar_x = resBar.x + circle_R;
      var bar_y = resBar.y + indicator.h;
      for (var i = 0; i < colors.length; i++){
         //ctx.fillStyle = colors[i];
         var w;
         if ((i == 0) || (i == last_color_index)){
            w = segment_endpoint_straight_length;
         }
         else{
            w = segment_size;
         }
         svg = svg + '   <rect x = "'  + bar_x + '" y = "' + bar_y + '" width = "' + w + '" height = "' + bar_height + '" fill = "' + colors[i] + '"/>\n'
         bar_x = bar_x + w;
      }

      // Computing the position of the indicator. 
      indicator.x = resBar.x + segment_size/2 + resBar.indicator*segment_size;
      var indicator_svg = this.drawIndicator(indicator);
      svg = svg + indicator_svg;
      return svg;
   },
   
   /**
    * 
    * @param {The indicator structure 
    * The indicator array should be composed of
    * x: The x coordinate where the tip of the top indicator should be touching.
    * y: The y coordinate where the tip of the top indicator should be touching.
    * h: The height of the triangle of the indicator.
    * w: The width of the triangle of the indicator.
    * bw: The width of the indicator bar.
    * bh: The height of the indicator bar.} indicator
    */
   drawIndicator: function (indicator){

      // Indicators are simply done in black. 
      var svg = "";

      // Top indicator
      var p1 = indicator.x + "," + indicator.y;
      var p2 = (indicator.x-indicator.w/2) + "," + (indicator.y - indicator.h);
      var p3 = (indicator.x+indicator.w/2) + "," + (indicator.y - indicator.h);

      svg = svg  + '   <polygon points="' + p1 + ' ' + p2 + ' ' + p3 + '" />\n';

      // Bottom indicator. 
      var p1 = indicator.x + "," + (indicator.y+indicator.bh);
      var p2 = (indicator.x-indicator.w/2) + "," + (indicator.y + indicator.h + indicator.bh);
      var p3 = (indicator.x+indicator.w/2) + "," + (indicator.y + indicator.h + indicator.bh);
      svg = svg  + '   <polygon points="' + p1 + ' ' + p2 + ' ' + p3 + '" />\n';

      // Indicator line.
      svg = svg + '   <rect x="' + (indicator.x-indicator.bw/2) + '" y = "' + indicator.y + '" width = "' + indicator.bw + '" height = "' + indicator.bh  + '" />'
      
      return svg;

   },
   
   /**
    * 
    * @param {The value to compute its corresponding resBarIndex} value
    * @param {Vector containing the result bar breakpoint values. An n segment bar will contain n-1 breakpoints} break_points 
    * @param {Boolean. If true higher values are better. Otherwise lower value are better} higher_better 
    */
   
    computeSegmentIndicator: function (value, break_points,higher_better){
   
      var resBarIndex = -1;
      for (var i = 0; i < break_points.length; i++){
         if (value < break_points[i]){
            resBarIndex = i;
            break;
         }
      }
   
      // Never broke, so it's the last index. 
      if (resBarIndex == -1) resBarIndex = break_points.length;
   
      // The current computed index if for when a lower value is a better result. as the color fo the bar are always drawn from best to worst. 
      if (higher_better){
         if (break_points.length == 3){
            switch(resBarIndex){
               case 0: resBarIndex = 3;
               break;
               case 1: resBarIndex = 2;
               break;
               case 2: resBarIndex = 1;
               break;
               case 3: resBarIndex = 0;
               break;
            }
         }
         else if (break_points.length == 2){
            if (resBarIndex == 0) resBarIndex = 2;
            else if (resBarIndex == 2) resBarIndex = 0;
         }
         else if (break_points.length == 1){
            if (resBarIndex == 0) resBarIndex = 1;
            else resBarIndex = 0;
         }
      }
   
      return resBarIndex;
   
   }

}