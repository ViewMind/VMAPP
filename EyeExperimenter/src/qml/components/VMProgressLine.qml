import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    id: progressLine
    color: "transparent"
    //border.color: "transparent"
    //border.width: 1

    /**
      The progress line as provided by our graphical designer has the following structure

      Main Point A
         Sub Point 1
         Sub Point 2
         ...
         Sub Point N
      Main Point B
         Sub Point 1
         Sub Point 2
         ...
         Sub Point N
      ...
      Main Point XXXXX
         Sub Point 1
         Sub Point 2
         ...
         Sub Point N

     Each main point is represented by a large circle with a number and to it's right text.
     Each sub point is represented by a solid large dot and text next to it.
     Subpoints are joined by a continous line.
     Main point have a space between them and the line mentioned above.
     If no subpoints are present, then a single line segments, with no dots, is drawn.

     **/

    property bool vmOnlyColorCurrent: false

    // I've only here learned how to do "private stuff". Which is why other QML classes that I did don't have it, but here it's presetn.
    QtObject {
        id: internal

        property int vmMainIndex: 0
        property int vmMainSubIndex: 0
        property var vmTextStructure: []
        property var vmMainPoints: []

        readonly property color vmActive:  VMGlobals.vmBlueSelected
        readonly property color vmInactive: VMGlobals.vmBlackText

        readonly property double vmLargeD: VMGlobals.adjustWidth(32)
        readonly property double vmSmallD: VMGlobals.adjustWidth(7)
        readonly property double vmInterSubSegmentLength : VMGlobals.adjustHeight(8+10+8) // The length of the segment between two two subpoints + the diameter of 2 subpoints. Required for drawing the middle of subpoint chains.
        readonly property double vmEndsSubSegmentLength  : VMGlobals.adjustHeight(8+10)   // The length of the segment between two two subpoints + the diameter of a subpoint. Required for segmets drawn at the end of a chain o subpoints
        readonly property double vmInterSegmentLength    : VMGlobals.adjustHeight(10)     // The length of the segment between two main points or two subpoints.
        readonly property double vmAirInterSegmentBubble : VMGlobals.adjustHeight(5)      // Space between a main point and it's segment either before or after it.
        readonly property double vmLineWidth             : VMGlobals.adjustWidth(1)
        readonly property double vmHAirSubPoint          : VMGlobals.adjustWidth(23) // Horizontal space between the Left Screen Margin and a sub point
        readonly property double vmHAirMainPoint         : VMGlobals.adjustWidth(10) // Horizontal space between the Right Screen Margin and a Main Point.


        function drawMainCircles(ctx, yoffset, startIndex, endIndex){

            let y = yoffset
            let i = 0;
            let r = internal.vmLargeD/2
            let x = vmLineWidth

            //console.log("Start Index " + startIndex + " End INdex " + endIndex + " main index " + vmMainIndex);

            for (i = startIndex; i <= endIndex; i++){

                if (vmOnlyColorCurrent){
                    ctx.beginPath()
                    if (i == internal.vmMainIndex){
                        ctx.fillStyle = internal.vmActive
                        ctx.strokeStyle = internal.vmActive
                    }
                    else{
                        ctx.fillStyle = internal.vmInactive
                        ctx.strokeStyle = internal.vmInactive
                    }
                }

                // Drawing the main circle.
                ctx.roundedRect(x,y,internal.vmLargeD,internal.vmLargeD,r,r)
                ctx.stroke();

                // Drawing the number
                ctx.textAlign  = "center"
                ctx.fillText((i+1),x+r,y+r)

                // Drawing the text
                ctx.textAlign  = "left"
                ctx.fillText(internal.vmMainPoints[i],internal.vmLargeD + internal.vmHAirMainPoint,y+r)

                let subindexgap = internal.vmInterSegmentLength;

                if (i != internal.vmMainIndex){

                    // Drawing the connecting segement, if this is NOT the active main index.
                    if (i < endIndex){
                        ctx.moveTo(x+r,y+internal.vmLargeD+internal.vmAirInterSegmentBubble)
                        //console.log("y " + y + " D " + internal.vmLargeD  + " air " + internal.vmAirInterSegmentBubble + " inter seg L " + internal.vmInterSegmentLength);
                        ctx.lineTo(x+r,y+internal.vmLargeD+internal.vmAirInterSegmentBubble + internal.vmInterSegmentLength)
                        ctx.stroke();
                    }

                }
                else {

                    // No drawing but we compute the gap for all the subindexes.
                    let N = vmTextStructure[vmMainPoints[i]].length;
                    if (N === 0){
                        // Since this is the active point and there are NO subpoints we need to draw a line for it, unless it's the last one
                        if (i < (vmMainPoints.length-1)){
                            ctx.moveTo(x+r,y+internal.vmLargeD+internal.vmAirInterSegmentBubble)
                            ctx.lineTo(x+r,y+internal.vmLargeD+internal.vmAirInterSegmentBubble + internal.vmInterSegmentLength)
                            ctx.stroke();
                        }
                    }
                    else if (N >= 1){
                        // We need N circles with N-1 connecting segments + 2 end segments.
                        subindexgap = vmEndsSubSegmentLength*2 + N*vmSmallD + (N-1)*vmInterSubSegmentLength
                    }

                }

                y = y + internal.vmAirInterSegmentBubble*2 + subindexgap + internal.vmLargeD

            }

            return y;

        }

        function drawSubCircles(ctx){

            // First we compute teh start y based on the current seleted main Circle.
            let A = 2*vmAirInterSegmentBubble + vmInterSegmentLength
            let y = vmLargeD*(vmMainIndex+1) + vmAirInterSegmentBubble + vmLineWidth + (vmMainIndex*A)
            //let y = vmLargeD*(vmMainIndex+1) + 0 + vmLineWidth + (vmMainIndex*A)
            let subIndexTexts = vmTextStructure[vmMainPoints[vmMainIndex]];
            let N = subIndexTexts.length;
            let R = vmLargeD/2
            let r = vmSmallD/2
            if (N === 0) return;

            // We draw the entire line first. And then we draw the circles OVER the line.
            let subIndexLineLength = vmEndsSubSegmentLength*2 + N*vmSmallD + (N-1)*vmInterSubSegmentLength

            ctx.beginPath();
            ctx.fillStyle = VMGlobals.vmWhite
            ctx.strokeStyle = vmActive
            ctx.moveTo(R+vmLineWidth,y);
            ctx.lineTo(R+vmLineWidth,y+subIndexLineLength)
            ctx.stroke();

            ctx.textAlign  = "left"

            y = y + vmEndsSubSegmentLength
            let x = R - r + vmLineWidth

            for (let i = 0; i < N; i++){

                // Drawing the main circle.
                ctx.beginPath();
                if (vmMainSubIndex === i){
                    ctx.fillStyle = vmActive
                }
                else{
                    ctx.fillStyle = VMGlobals.vmWhite
                }
                ctx.roundedRect(x,y,vmSmallD,vmSmallD,r,r)
                ctx.stroke();
                ctx.fill();

                // Drawing the text.
                ctx.beginPath();
                ctx.fillStyle = vmActive
                if (vmMainSubIndex === i){
                    ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontBaseSize).arg(mainWindow.vmSegoeHeavy.name);
                }
                else{
                    ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontBaseSize).arg(mainWindow.vmSegoeNormal.name);
                }
                ctx.fillText(subIndexTexts[i],x+vmSmallD+vmHAirSubPoint,y+r)

                y = y + vmInterSubSegmentLength + vmSmallD
            }

        }

    }

    function setup(textStructure){
        // Text structure is a an object where each key is mapped to an array of string.
        // Each key is a string which behaves as the main text in the progress line.
        internal.vmTextStructure = textStructure
        internal.vmMainPoints = Object.keys(textStructure)

        var M = internal.vmMainPoints.length

        // To estimate the width we need to know the width of the widest text + the large Radius + margin spacing.
        var widestFont = 0; // To finde the widest text
        var N = 0; // To find the largest subchain, in number of items.
        for (var i = 0; i < M; i++){
            var key = internal.vmMainPoints[i]
            textMeasure.text = key
            //console.log("Measuring text " + key + " and it is " + textMeasure.width)
            widestFont = (widestFont < textMeasure.width) ? textMeasure.width : widestFont
            N = (internal.vmTextStructure[key].length > N) ? internal.vmTextStructure[key].length : N
        }
        //console.log([widestFont,internal.vmLargeD,internal.vmHAirMainPoint])
        width = widestFont + internal.vmLargeD + internal.vmHAirMainPoint + internal.vmLineWidth*2

        // The width is expanded a bit to make sure all text fits.
        width = width*1.1;

        // To estimate the height it's the Number of Main States (N) + (N-2) connectors + length of the largest subchain.
        var largestChainHeight = internal.vmInterSegmentLength + 2*internal.vmAirInterSegmentBubble
        if (N > 0){
            largestChainHeight = (N - 1)*internal.vmInterSubSegmentLength + 2*internal.vmEndsSubSegmentLength
                    + N*internal.vmSmallD + 2*internal.vmAirInterSegmentBubble
        }

        //console.log("Largest chain " + largestChainHeight);

        height = largestChainHeight + M*internal.vmLargeD //+ 2*internal.vmAirInterSegmentBubble
                + (M-2)*(internal.vmInterSegmentLength + 2*internal.vmAirInterSegmentBubble)
                + 2*internal.vmLineWidth

        //console.log("Progress line Computed size as " +  width + "x" + height)

        reset();
    }

    function reset(){
        internal.vmMainIndex = 0;
        internal.vmMainSubIndex = 0;
        drawCanvas.requestPaint();
    }

    function indicateNext(){
        var M = internal.vmTextStructure[internal.vmMainPoints[internal.vmMainIndex]].length;
        var N = internal.vmMainPoints.length

        if (internal.vmMainSubIndex < M-1){
            internal.vmMainSubIndex++;
        }
        else {
            if (internal.vmMainIndex < N-1){
                internal.vmMainSubIndex = 0;
                internal.vmMainIndex++;
            }
        }
        drawCanvas.requestPaint();
    }

    function indicatePrevious(){
        if (internal.vmMainSubIndex > 0){
            internal.vmMainSubIndex--;
        }
        else {
            if (internal.vmMainIndex > 0){
                internal.vmMainIndex--;
                var M = internal.vmTextStructure[internal.vmMainPoints[internal.vmMainIndex]].length;
                if (M > 0){
                    internal.vmMainSubIndex = M-1;
                }
            }
        }
        drawCanvas.requestPaint();
    }

    function getCurrentTexts(){
        let ret = [];
        ret.push(internal.vmMainPoints[internal.vmMainIndex]);
        if (internal.vmTextStructure[internal.vmMainPoints[internal.vmMainIndex]].length > 0){
            ret.push(internal.vmTextStructure[internal.vmMainPoints[internal.vmMainIndex]][internal.vmMainSubIndex])
        }
        else ret.push("");
        return ret;
    }

    TextMetrics {
        id: textMeasure
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarge
    }

    Canvas{
        id: drawCanvas
        anchors.fill: parent
        contextType: "2d"
        onPaint: {

            var ctx = getContext("2d")
            var mainIndex = 0;
            var subIndex = 0;
            var i = 0;
            var y = 0;

            ctx.reset();

            // CTX configurations that will not change.
            ctx.lineWidth = internal.vmLineWidth
            ctx.textBaseline = "middle"

            // We will always start with the active state.
            ctx.fillStyle = internal.vmActive
            ctx.strokeStyle = internal.vmActive

            ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontLarge).arg(mainWindow.vmSegoeHeavy.name);

            ctx.beginPath();
            y = internal.drawMainCircles(ctx,internal.vmLineWidth,0,internal.vmMainIndex)

            ctx.beginPath();
            ctx.fillStyle = internal.vmInactive
            ctx.strokeStyle = internal.vmInactive

            internal.drawMainCircles(ctx,y,internal.vmMainIndex+1,internal.vmMainPoints.length-1);

            internal.drawSubCircles(ctx)


            //            // The three font types.
            //            ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontLarge).arg(mainWindow.vmSegoeHeavy.name);
            //            ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontBaseSize).arg(mainWindow.vmSegoeNormal.name);
            //            ctx.font = '%1px "%2"'.arg(VMGlobals.vmFontBaseSize).arg(mainWindow.vmSegoeHeavy.name);


        }
    }

}

