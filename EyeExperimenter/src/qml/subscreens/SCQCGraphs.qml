import QtQuick
import "../"
import "../components"

Rectangle {

    id: subScreenQCGraphs
    anchors.fill: parent

    property var vmStudyNameMap: []
    property var vmStdStudyNameByIndex: []
    property int vmCurrentlySelectedStudy : 0;
    property int vmCurrentlySelectedGraph : 0;
    property int vmQCindex: 0
    property int vmQCThreshold: 0

    property bool vmIsLastGraph: false
    property var vmNumberOfGraphsPerStudy;

    // When there is only one graph the logic needs to change. So we create this flag for convenience
    property bool vmOnly1Graph: false;

    readonly property int vmINDEX_ICI:          0
    readonly property int vmINDEX_FIXATIONS:    1
    readonly property int vmINDEX_GRAPH_POINTS: 0
    readonly property int vmINDEX_FREQ:         3



    function initializeForGraphTraversal(){
        let studyAndFlagMap = loader.getStudyList();

        vmStdStudyNameByIndex = [];
        vmNumberOfGraphsPerStudy = [];
        let totalNStudies = 0;
        for (let i in studyAndFlagMap){
            let name_and_flag = studyAndFlagMap[i];
            vmStdStudyNameByIndex.push(name_and_flag["name"]);
            if (name_and_flag["3D"]){
                vmNumberOfGraphsPerStudy.push(1);
                totalNStudies = totalNStudies + 1;
            }
            else {
                vmNumberOfGraphsPerStudy.push(2);
                totalNStudies = totalNStudies + 2;
            }
        }

        vmCurrentlySelectedStudy = 0;
        vmCurrentlySelectedGraph = 0;
        vmIsLastGraph = false;

        if (totalNStudies == 1){
            vmOnly1Graph = true;
        }
        else {
            vmOnly1Graph = false;
        }

//        console.log("Total number of studies is " + totalNStudies);
//        console.log("Is Only 1 Graph: " + vmOnly1Graph);

        // We now load the first graph
        loadGraph();


    }

    function setStudyAndGraphName(study,graph_name){
        evalTitle.text = study;
        graphName.text = "/ " + graph_name;
    }

    function isLastGraph(){
        let condition = (vmCurrentlySelectedGraph == (vmNumberOfGraphsPerStudy[vmCurrentlySelectedStudy]-1));
        condition = condition && (vmCurrentlySelectedStudy == vmStdStudyNameByIndex.length-1)
        vmIsLastGraph = condition;
    }

    function moveGraph(forward){
        if (forward){
            if (vmCurrentlySelectedGraph < (vmNumberOfGraphsPerStudy[vmCurrentlySelectedStudy]-1)){
                vmCurrentlySelectedGraph++;
            }
            else {
                if (vmCurrentlySelectedStudy < (vmStdStudyNameByIndex.length-1)){
                    vmCurrentlySelectedStudy++;
                    vmCurrentlySelectedGraph = 0;
                }
            }
        }
        else {
            // We are moving back.
            if (vmCurrentlySelectedGraph > 0){
                vmCurrentlySelectedGraph--;
            }
            else {
                if (vmCurrentlySelectedStudy > 0){
                    vmCurrentlySelectedStudy--;
                    vmCurrentlySelectedGraph = vmNumberOfGraphsPerStudy[vmCurrentlySelectedStudy]-1;
                }
            }
        }

        isLastGraph();

        // Moving the progress line along with us.
        let newNames = viewQC.moveProgressLine(forward)
        setStudyAndGraphName(newNames[0],newNames[1])

        // Reloading the graph.
        loadGraph();

    }

    function loadGraph(){

        var data = loader.getStudyGraphData(vmStdStudyNameByIndex[vmCurrentlySelectedStudy],vmCurrentlySelectedGraph);

        if (!("GraphData" in data)) return;
        //        console.log("Loaded Graph Data")
        //        console.log(JSON.stringify(data));


        let index_names = loader.getStringListForKey("viewqc_index_names");
        let indName = index_names[vmCurrentlySelectedGraph]

        let text = "";
        if (vmCurrentlySelectedGraph === vmINDEX_FREQ){
            text = loader.getStringForKey("viewQC_limitSampleF")
        }
        else if (vmCurrentlySelectedGraph === vmINDEX_FIXATIONS){
            text = loader.getStringForKey("viewQC_limitFixations")
        }
        else if (vmCurrentlySelectedGraph === vmINDEX_GRAPH_POINTS){
            text = loader.getStringForKey("viewQC_limitDataPoints")
        }
        else if (vmCurrentlySelectedGraph === vmINDEX_ICI){
            text = loader.getStringForKey("viewQC_limitICI");
        }

        // Separating the explation text: The first line is the title.
        text = text.replace("<<I>>",data["QCThreshold"]);
        vmQCThreshold = data["QCThreshold"]

        indexName.text = "<b>" + indName + "</b><br>" + text;

        // Setting up and plotting the graph.
        if ((vmCurrentlySelectedGraph === vmINDEX_GRAPH_POINTS) || (vmCurrentlySelectedGraph == vmINDEX_FIXATIONS)){
            graph.vmDataPoints = data["GraphData"];
            graph.vmBarGraph = true;
            // These values are returned as an ARRAY of the same length of the data points. Becuase an old implementation changed the reference for each trial
            graph.vmMinReferenceValue = data["LowerREFData"][0]
            graph.vmMaxReferenceValue = data["UpperREFData"][0]
            graph.autoScaleYAxis(false,true);
            graph.enableReferenceCheck(true,false)
            graph.vmMinY = 0;
            graph.graph();
        }
        else {
            graph.vmDataPoints = data["GraphData"];
            graph.vmBarGraph = false;
            // These values are returned as an ARRAY of the same length of the data points. Becuase an old implementation changed the reference for each trial
            graph.vmMinReferenceValue = data["LowerREFData"][0]
            graph.vmMaxReferenceValue = data["UpperREFData"][0]
            graph.autoScaleYAxis(false,true);
            graph.enableReferenceCheck(true,true)
            graph.vmMinY = 0;
            graph.graph();
        }


    }

    Text {
        id: evalTitle
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(31)
        anchors.leftMargin: VMGlobals.adjustWidth(31)
    }

    Text {
        id: graphName
        color: VMGlobals.vmGrayAccented
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: evalTitle.top
        anchors.left: evalTitle.right
        anchors.leftMargin: VMGlobals.adjustWidth(10)
    }

    Rectangle {

        id: summaryResultRect
        color: VMGlobals.vmGreenBKGStudyMessages
        width: VMGlobals.adjustWidth(911)
        height: VMGlobals.adjustHeight(101);
        anchors.top: evalTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.horizontalCenter: parent.horizontalCenter
        radius: VMGlobals.adjustHeight(8)

        VMPercentWheel {
            id: indexWheel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: VMGlobals.adjustWidth(15)
            onVmPercentChanged: {
                if (vmPercent > vmQCThreshold){
                    vmOuterColor = VMGlobals.vmGreenSolidQCIndicator
                    vmInnerColor = VMGlobals.vmGreenBKGStudyMessages
                }
                else {
                    vmOuterColor = VMGlobals.vmRedError
                    vmInnerColor = VMGlobals.vmRedBadIndexBackground
                }
                summaryResultRect.color = vmInnerColor
            }
            Component.onCompleted: {
                setDesiredCircleHeight(VMGlobals.adjustHeight(71));
            }
        }

        Text {
            id: indexName
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: indexWheel.right
            anchors.leftMargin: VMGlobals.adjustWidth(20)
        }


    }


    //////////////////////////////////////////////// CANVAS /////////////////////////////////////////////////////

    Canvas {

        id: graph
        contextType: "2d"
        width: summaryResultRect.width + VMGlobals.adjustWidth(10)
        height: parent.height - evalTitle.height - evalTitle.anchors.topMargin -  summaryResultRect.height - summaryResultRect.anchors.topMargin - VMGlobals.adjustHeight(30) - anchors.bottomMargin
        anchors.right: summaryResultRect.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(10) + subScreenQCGraphs.radius

        property var vmDataPoints: []                     // The acual points to plot. Whether line or bar
        property double vmMinReferenceValue: 0            // Used to draw the reference line for the minimum value
        property double vmMaxReferenceValue: 0            // Used to draw the reference line for the maximum value.

        property bool vmBarGraph: true

        property double vmMinY : 0
        property double vmMaxY : 0

        readonly property double vmXAxisLocation: height - VMGlobals.adjustHeight(19 + 10) // Text Height + Margin.
        readonly property double vmAirBetweenXAxisAndXLabel: VMGlobals.adjustHeight(10 + 2.5)
        readonly property double vmAirBetweenYAxisAndYLabel: VMGlobals.adjustWidth(6)
        readonly property double vmYLabelsWidth: VMGlobals.adjustWidth(67)
        readonly property double vmLineWidth: VMGlobals.adjustHeight(1)
        readonly property double vmXTickLength: VMGlobals.adjustHeight(5)
        readonly property double vmYValueAtTop: VMGlobals.adjustHeight(8)
        readonly property double vmXGraphSideMargins: VMGlobals.adjustWidth(10)

        readonly property int vmNumberOfXLabels: 5
        readonly property int vmNumberOfYLabels: 6

        function graph(){
            requestPaint();
        }

        QtObject {
            id: autoscale
            property bool min: true
            property bool max: true
        }

        QtObject {
            id: referenceCheck
            property bool min: true
            property bool max: true
        }

        QtObject {
            id: graphValues
            property double effectiveGraphWidth: 0
            property double xspaceBetweenPoints: 0
            property double barWidth: 0
        }

        VMScaler {
            id: xscale
        }

        VMScaler {
            id: yscale
        }

        function autoScaleYAxis(min,max){
            autoscale.max = max
            autoscale.min = min
        }

        function enableReferenceCheck(min, max){
            referenceCheck.max = max
            referenceCheck.min = min
        }

        function computeXTicksAndValues(xStart,xInterval,barWidth){
            // Computing the xTick positions.
            var xTicksPositions = [];
            var xTrialIndex = [];

            var nIntervalForLabel = Math.ceil(vmDataPoints.length/(vmNumberOfXLabels + 1));
            var startIndex = nIntervalForLabel-1;

            while (startIndex >= 0){
                var nextPoint = startIndex + nIntervalForLabel;
                //console.log(barIndex);
                if (nextPoint >= vmDataPoints.length){
                    break;
                }
                else{
                    xTicksPositions.push(xStart + startIndex*xInterval + barWidth/2)
                    xTrialIndex.push(startIndex+1);
                    startIndex = nextPoint;
                }
            }
            var ans = { "ticks": xTicksPositions, "labels": xTrialIndex };
            return ans;
        }


        function computeMaxAndMinYValues(){

            // If the fixed scale is set.
            if (vmDataPoints.length === 0) return [];
            var max = vmDataPoints[0];
            var min = vmDataPoints[0]
            for (var i = 1; i < vmDataPoints.length; i++){
                if (max < vmDataPoints[i]) max = vmDataPoints[i];
                if (min > vmDataPoints[i]) min = vmDataPoints[i];
            }

            //console.log(JSON.stringify(vmRefDataPoints));

            if (max < vmMaxReferenceValue) max = vmMaxReferenceValue
            if (min > vmMinReferenceValue) min = vmMinReferenceValue

            if (autoscale.max) vmMaxY = max;
            if (autoscale.min) vmMinY = min;

        }


        function drawAxis(ctx){

            // The axis consists of vmNumberOfYLabels horizontal lines.
            computeMaxAndMinYValues()

            // Setting the scale to which computing the y values.
            yscale.setScale(vmMinY,vmMaxY,vmXAxisLocation,vmYValueAtTop)

            // Compute the Y ticks/lines positions.
            let yinterval = (vmMaxY - vmMinY) / (vmNumberOfYLabels-1)
            let y = vmMinY;
            let xstart = vmYLabelsWidth + vmAirBetweenYAxisAndYLabel
            let xend   = width

            //console.log("Minimum Y Value " + vmMinY + " and maximum " + vmMaxY)

            // Drawing the horizontal lines in the y axis and the right text right besides each axis.
            ctx.lineWidth = vmLineWidth
            let fontString = '400 %1px "%2"'.arg(VMGlobals.vmFontBaseSize).arg(mainWindow.vmSegoeNormal.name);
            //console.log("Using font " + fontString)
            ctx.font = fontString
            ctx.textBaseline = "middle"
            ctx.textAlign  = "right"

            for (var i = 0; i < vmNumberOfYLabels; i++){

                y = vmMinY + i*yinterval
                //console.log("Line at Y " + y);
                let gy = yscale.convert(y)
                ctx.beginPath();
                ctx.strokeStyle = VMGlobals.vmGrayUnselectedBorder
                ctx.moveTo(xstart,gy)
                ctx.lineTo(xend,gy)
                ctx.stroke();

                ctx.beginPath();
                ctx.fillStyle = VMGlobals.vmBlackText
                let ytext = Math.round(y* 10) / 10 // Trick to round to only 1 decimal place.
                ctx.fillText(ytext,vmYLabelsWidth,gy)

            }

            // We now compute the x space between the location of the data point and this will, in turn, determine bar width.
            graphValues.effectiveGraphWidth = width - vmYLabelsWidth - 2*vmXGraphSideMargins
            graphValues.xspaceBetweenPoints = graphValues.effectiveGraphWidth/(vmDataPoints.length-1)
            graphValues.barWidth = graphValues.xspaceBetweenPoints/2

            // Setting the x scale
            xscale.setScale(0,vmDataPoints.length,vmYLabelsWidth+vmXGraphSideMargins,width-vmXGraphSideMargins)

            // We now compute x tick values: the indexes of the ticks to be shown.
            let trialDelta = Math.round(vmDataPoints.length/vmNumberOfXLabels);
            let x = trialDelta
            let xLabels = [];
            while (x < vmDataPoints.length){
                xLabels.push(x);
                x = x + trialDelta;
            }

            // Drawing the xticks and the xLabels.
            ctx.textBaseline = "top"
            ctx.textAlign  = "center"

            // The y value for the top of the text.
            let ytext = vmXAxisLocation + vmXTickLength + vmAirBetweenYAxisAndYLabel

            for (i = 0; i < xLabels.length; i++){
                let text = xLabels[i] + 1
                x = xscale.convert(xLabels[i])
                if (vmBarGraph){
                    x = x + graphValues.barWidth/2 // This guarantees the tick in the middle of the bar.
                }

                ctx.beginPath();
                ctx.strokeStyle = VMGlobals.vmGrayUnselectedBorder
                ctx.moveTo(x,vmXAxisLocation)
                ctx.lineTo(x,vmXAxisLocation + vmXTickLength)
                ctx.stroke();

                ctx.beginPath();
                ctx.fillStyle = VMGlobals.vmBlackText
                ctx.fillText(text,x,ytext)
            }

        }

        function drawBars(ctx){

            let w = graphValues.barWidth
            let r = 0.2*w
            vmQCindex = 0;

            for (var i = 0; i < vmDataPoints.length; i++){

                // Computing the x position of the left hand side of the rectangle
                let x = xscale.convert(i)
                let y = yscale.convert(vmDataPoints[i])
                let h = vmXAxisLocation - y

                ctx.beginPath();
                if (referenceCheck.min && (vmDataPoints[i] < vmMinReferenceValue)){
                    ctx.fillStyle = VMGlobals.vmRedError
                }
                else if ((vmDataPoints[i] > vmMaxReferenceValue) && referenceCheck.max ) {
                    ctx.fillStyle = VMGlobals.vmRedError
                }
                else {
                    ctx.fillStyle = VMGlobals.vmBlueSelected
                    vmQCindex++
                }

                // This is the base rectangle.
                let y2 = y + 0.25*h
                ctx.fillRect(x,y2,w,0.75*h)

                // Now we add the top rectangle.
                ctx.beginPath();
                ctx.roundedRect(x,y,w,0.5*h,r,r)
                ctx.fill();

            }
            let p = Math.round(vmQCindex*100/vmDataPoints.length)
            indexWheel.vmPercent = p;

        }

        function drawTriangleForReference(ctx,ypos){
            let x = vmYLabelsWidth + vmAirBetweenYAxisAndYLabel - VMGlobals.adjustWidth(9.67) // The x position of the tip.
            let x2 = x - VMGlobals.adjustWidth(3.5)
            let h = VMGlobals.adjustHeight(7)
            let ytop = ypos - h/2
            let ybottom = ypos + h/2

            ctx.beginPath()
            ctx.fillStyle = VMGlobals.vmRedError
            ctx.moveTo(x,ypos)  // Tipo of the pointer
            ctx.lineTo(x2,ytop)
            ctx.lineTo(x2,ybottom)
            ctx.closePath()
            ctx.fill();
        }

        function drawReferences(ctx){

            let xstart = vmYLabelsWidth + vmAirBetweenYAxisAndYLabel
            let xend   = width

            ctx.strokeStyle = VMGlobals.vmRedError
            ctx.lineWidth = vmLineWidth


            let y = yscale.convert(vmMinReferenceValue)
            ctx.beginPath();
            ctx.moveTo(xstart,y)
            ctx.lineTo(xend,y)
            ctx.stroke();

            drawTriangleForReference(ctx,y)

            y = yscale.convert(vmMaxReferenceValue)
            ctx.beginPath();
            ctx.moveTo(xstart,y)
            ctx.lineTo(xend,y)
            ctx.stroke();

            drawTriangleForReference(ctx,y)

        }

        function drawLineGraph(ctx){

            vmQCindex = 0;
            ctx.beginPath();
            ctx.strokeStyle = VMGlobals.vmBlueSelected
            for (var i = 0; i < vmDataPoints.length; i++){

                // Computing the x position of the left hand side of the rectangle
                let x = xscale.convert(i)
                let y = yscale.convert(vmDataPoints[i])

                let isBadPoint = (referenceCheck.min && (vmDataPoints[i] < vmMinReferenceValue));
                isBadPoint = isBadPoint|| ((vmDataPoints[i] > vmMaxReferenceValue) && referenceCheck.max );
                if (!isBadPoint) vmQCindex++


                if (i == 0){
                    ctx.moveTo(x,y)
                }
                else{
                    ctx.lineTo(x,y)
                }

            }
            ctx.stroke();
            let p = Math.round(vmQCindex*100/vmDataPoints.length)
            indexWheel.vmPercent = p;

        }

        onPaint: {

            var ctx = getContext("2d")
            ctx.reset();
            //ctx.fillStyle = "#fafafa";
            //ctx.fillRect(0,0,width,height);

            drawAxis(ctx);

            if (vmBarGraph) drawBars(ctx)
            else drawLineGraph(ctx)

            drawReferences(ctx)

        }

    }

}
