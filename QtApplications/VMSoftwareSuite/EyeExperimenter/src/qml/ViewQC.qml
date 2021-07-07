import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewStudyDone
    width: viewCalibrationStart.vmWIDTH
    height: viewCalibrationStart.vmHEIGHT
    readonly property string keysearch: "viewQC_"

    // The order of the graphs.
    readonly property int vmINDEX_GRAPH_POINTS: 0
    readonly property int vmINDEX_FIXATIONL: 1
    readonly property int vmINDEX_FIXATIONR: 2
    readonly property int vmINDEX_GLITCHES: 3
    readonly property int vmINDEX_FREQ: 4

    readonly property int vmAPI_REPORT_REQUEST: 2

    readonly property int vmFAIL_CODE_NONE: 0
    readonly property int vmFAIL_CODE_SERVER: 2

    property var vmStudyNameMap: []
    property var vmStdStudyNameByIndex: [];
    property var vmSubTitleList: [];

    function loadStudiesAndGraphs(){

        // Selecting the different types of graphps.
        var buttonList = [
                    loader.getStringForKey(keysearch+"GraphPoints"),
                    loader.getStringForKey(keysearch+"GraphFixationsL"),
                    loader.getStringForKey(keysearch+"GraphFixationsR"),
                    loader.getStringForKey(keysearch+"GraphGlitches"),
                    loader.getStringForKey(keysearch+"GraphFreq"),
                ];

        vmSubTitleList = [
                    loader.getStringForKey(keysearch+"SubPoints"),
                    loader.getStringForKey(keysearch+"SubFixationsL"),
                    loader.getStringForKey(keysearch+"SubFixationsR"),
                    loader.getStringForKey(keysearch+"SubGlitches"),
                    loader.getStringForKey(keysearch+"SubFrequency"),
                ];

        qcGraphList.clear();
        for (var i =0 ; i < buttonList.length; i++){
            qcGraphList.append({"vmItemIndex" : i, "vmIsSelected" : false, "vmText": buttonList[i], "vmChangeToTrigger": 0});
        }

        // Creating the study name language map
        var nameList = loader.getStringListForKey(keysearch + "StudyNameMap");
        vmStudyNameMap = [];
        var key;
        for (i = 0; i < nameList.length; i++){
            var name = nameList[i].replace("\n","");
            if ((i % 2) === 0){
                // This is a key:
                key = name;
            }
            else{
                // This is a value.
                vmStudyNameMap[key] = name;
            }
        }

        // Getting the study list.
        var studies = loader.getStudyList();
        qcStudyList.clear();
        vmStdStudyNameByIndex = [];
        for (i =0 ; i < studies.length; i++){
            var trName = vmStudyNameMap[studies[i]];
            qcStudyList.append({"vmItemIndex" : i, "vmIsSelected" : false, "vmText": trName, "vmChangeToTrigger": 1});
            vmStdStudyNameByIndex.push(studies[i]);
        }

        qcGraphList.setProperty(0,"vmIsSelected",true)
        qcStudyView.currentIndex = 0;
        qcStudyList.setProperty(0,"vmIsSelected",true)
        qcGraphsView.currentIndex = 0;
        //loadGraph();
    }

    function loadGraph(){
        //console.log("Called log graph");
        var selectedStudy = vmStdStudyNameByIndex[qcStudyView.currentIndex];
        var selectedGraph = qcGraphsView.currentIndex;
        var data = loader.getStudyGraphData(selectedStudy,selectedGraph);
        //console.log(JSON.stringify(data));

        if (selectedGraph === vmINDEX_FREQ){
            graph.vmGraphType = "line";
        }
        else{
            graph.vmGraphType = "bar";
        }

        if ((qcGraphsView.currentIndex < 0) || (qcGraphsView.currentIndex >= vmSubTitleList.length)) return;

        viewSubTitle.text = vmSubTitleList[qcGraphsView.currentIndex];

        graph.vmDataPoints = data["GraphData"];
        graph.vmRefDataPoints = data["REFData"];
        //console.log("Setting ref to" + JSON.stringify(data["REFData"]));
        graph.graph();

    }

    Connections {
        target: loader
        onFinishedRequest: {
            // Close the connection dialog and open the user selection dialog.
            connectionDialog.close();

            var failCode = loader.wasThereAnProcessingUploadError();

            // This check needs to be done ONLY when on this screen.
            if (( failCode !== vmFAIL_CODE_NONE ) && (swiperControl.currentIndex == swiperControl.vmIndexViewQC)){

                var titleMsg = viewHome.getErrorTitleAndMessage("error_db_server_error");
                vmErrorDiag.vmErrorMessage = titleMsg[1];
                vmErrorDiag.vmErrorTitle = titleMsg[0];
                vmErrorDiag.open();
                return;
            }

            if (loader.getLastAPIRequest() === vmAPI_REPORT_REQUEST)
               swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        }
    }

    Dialog {

        property string vmTitle: loader.getStringForKey(keysearch+"WaitTitle")
        property string vmMessage: loader.getStringForKey(keysearch+"WaitSubtitle")

        id: connectionDialog;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectConnectionDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The instruction text
        Text {
            id: diagConnectionTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmTitle
        }


        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.top:  diagConnectionTitle.bottom
            anchors.topMargin: mainWindow.height*0.038
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text:  connectionDialog.vmMessage;
            z: 2 // Sometimes the border of the image covers the text. This fixes it.
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: mainWindow.height*0.043
            anchors.horizontalCenter: parent.horizontalCenter
            scale: viewHome.vmScale
            visible: true
        }

    }


    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.032
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: loader.getStringForKey(keysearch + "Title");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.016
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#bcbec0"
        text: ""
    }

    ListModel {
        id: qcGraphList
    }

    ListModel {
        id: qcStudyList
    }

    //////////////////////////////////////////////// SELECTORS ///////////////////////////////////////////////////

    Rectangle {
        id: headerStudy
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        width: mainWindow.width*0.15;
        height: mainWindow.height*0.043
        anchors.top: graph.top
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.04;

        Text {
            id: studyNameText
            text: loader.getStringForKey(keysearch+"Study");
            width: parent.width
            font.family: gothamB.name
            font.pixelSize: 15*viewHome.vmScale
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        id: studySelectBackground
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        width: headerStudy.width
        height: mainWindow.height*0.2;
        anchors.top: headerStudy.bottom
        anchors.left: headerStudy.left

        ScrollView {
            id: viewStudySelectorArea
            anchors.fill: parent
            clip: true
            ListView {
                id: qcStudyView
                anchors.fill: parent
                model: qcStudyList
                delegate: VMSelectorRect {
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i !== currentIndex){
                            qcStudyList.setProperty(i,"vmIsSelected",false)
                        }
                    }
                    loadGraph();
                }
            }
        }
    }

    Rectangle {
        id: headerGraph
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#EDEDEE"
        radius: 4
        width: headerStudy.width
        height: headerStudy.height
        anchors.left: headerStudy.left
        anchors.bottom: qcGraphsView.top
        Text {
            id: graphTitleText
            text: loader.getStringForKey(keysearch+"GraphTitle");
            width: parent.width
            font.family: gothamB.name
            font.pixelSize: 15*viewHome.vmScale
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    ListView {
        id: qcGraphsView
        anchors.bottom: graph.bottom;
        anchors.left: headerStudy.left
        width: headerStudy.width;
        height: headerStudy.height*qcGraphList.count
        model: qcGraphList
        delegate: VMSelectorRect {
        }
        onCurrentIndexChanged: {
            for (var i = 0; i < model.count; i++){
                if (i !== currentIndex){
                    qcGraphList.setProperty(i,"vmIsSelected",false)
                }
            }
            loadGraph();
        }

    }

    //////////////////////////////////////////////// CANVAS ///////////////////////////////////////////////////s

    Canvas {
        id: graph
        width: mainWindow.width*0.75
        height: mainWindow.height*0.6
        anchors.right: parent.right;
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.01
        anchors.rightMargin: mainWindow.width*0.05

        // Constants.
        readonly property real vmGraphWidth : 0.9
        readonly property real vmGraphHeight: 0.8
        readonly property real vmBarGraphWhiteSpaceTotal: 0.15
        readonly property real vmGraphEffectiveWidth: 0.9
        readonly property real vmTextBottomMargin: 0.05
        readonly property real vmTextRightMargin: 0.1
        readonly property real vmXTickHeight: 0.02
        readonly property real vmYTickWidth: 0.01
        readonly property real vmLineWidth: 0.001
        readonly property real vmLineGraphVerticalWhiteSpace: 0.05

        // The graph type and data points.
        property var vmGraphType: "bar";  // Other option is line.
        property var vmDataPoints: [];   // Array of datapoints.
        property var vmRefDataPoints: [];
        property int vmNXLabels: 5;
        property int vmNYLabels: 5;

        // Texts
        property var vmTrialText: "Trial";

        // Color properties.
        property string vmAxisColor: "#cacaca";
        property string vmBackgroundColor: "#fafafa";
        property string vmBarBorderColor: "#297fca"
        property string vmBarFillColor: "#6389ab"
        property string vmRefLineColor: "#aa0000";
        property string vmTextColor: "#5c5c5c";
        property string vmLineGraphColor: "#297fca"

        function getExtremesFromDataPoints(){
            if (vmDataPoints.length === 0) return [];
            var max = vmDataPoints[0];
            var min = vmDataPoints[0]
            for (var i = 1; i < vmDataPoints.length; i++){
                if (max < vmDataPoints[i]) max = vmDataPoints[i];
                if (min > vmDataPoints[i]) min = vmDataPoints[i];
            }

            for (i = 0; i < vmRefDataPoints.length; i++){
                if (max < vmRefDataPoints[i]) max = vmRefDataPoints[i];
                if (min > vmRefDataPoints[i]) min = vmRefDataPoints[i];
            }

            return [max, min];
        }

        function computeXTicksAndValues(xStart,xInterval,barWidth){
            // Computing the xTick positions.
            var xTicksPositions = [];
            var xTrialIndex = [];

            var nIntervalForLabel = Math.ceil(vmDataPoints.length/(vmNXLabels + 1));
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

        function graph(){
            requestPaint();
        }

        onPaint: {

            var ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = vmBackgroundColor;
            ctx.fillRect(0, 0, width, height);

            // Drawing the axis.
            var air_w = (1 - vmGraphWidth)*width;
            var air_h = (1 - vmGraphHeight)*height;

            ctx.beginPath(); // Begin path is necessary to separate the effects of changing line types, colors, etc from each successive lien drawing. Otherwise changes made later affect this.
            ctx.strokeStyle = vmAxisColor
            ctx.setLineDash([]); // No dash lines.
            ctx.lineWidth = width*vmLineWidth;
            // Draw the vertical axis.
            ctx.moveTo(air_w/2,height-air_h/2);
            ctx.lineTo(air_w/2,air_h/2);
            // Draw the horizontal axis.
            ctx.moveTo(air_w/2,height-air_h/2);
            ctx.lineTo(width-air_w/2,height-air_h/2);
            ctx.stroke();

            //console.log("Horizontal AXIS @" + (height-air_h/2));

            // Computing the extremes
            var minmax = getExtremesFromDataPoints();
            var highestValue = minmax[0];
            var lowestValue = minmax[1];

            // These values depend on the graph type or are helper values that can only be defined in one place.
            var xStart;
            var xInterval;
            var verticalScale;
            var verticalWhitespaceFromHorizontalAxis;
            var xTicksPositions = [];
            var xTrialIndex = [];
            var yTickPositions = [];
            var yTickValues = [];
            var yTickInterval;
            var yLabel;
            var barWidth
            var effectiveWidth;
            var effectiveHeight;
            var verticalOffset;
            var x; // Helper variable used serveral times.
            var ans;

            ///////////////////////////////////////////////// DOING THE GRAPH (BAR) //////////////////////////////////////////////////////
            if (vmGraphType == "bar"){

                // We are doing a bar graph. We compute all values we need to draw.
                effectiveWidth = width*vmGraphWidth*vmGraphEffectiveWidth;
                effectiveHeight = height*vmGraphHeight;
                var totalAmountOfWhiteSpace = effectiveWidth*vmBarGraphWhiteSpaceTotal;
                var spaceBetweenBars = totalAmountOfWhiteSpace/(vmDataPoints.length-1);
                barWidth = (effectiveWidth - totalAmountOfWhiteSpace)/vmDataPoints.length;
                verticalScale = height*vmGraphHeight/highestValue;

                // We start adding the bars. So we first compute where our x starts.
                var xBar = ((1-vmGraphEffectiveWidth)/2)*width*vmGraphWidth + air_w/2
                xStart = xBar;
                xInterval = barWidth + spaceBetweenBars;
                verticalWhitespaceFromHorizontalAxis = 0;

                ctx.strokeStyle = vmBarBorderColor;
                ctx.fillStyle = vmBarFillColor;

                //console.log(vmDataPoints);

                for (var i = 0; i < vmDataPoints.length; i++){
                    var barHeight = verticalScale*vmDataPoints[i];
                    var y = air_h/2 + (effectiveHeight - barHeight);
                    //console.log("Drawing rect at (" + xBar + "," + y + ") " + " W: " + barWidth + ". H: " + barHeight)
                    ctx.fillRect(xBar,y,barWidth,barHeight);
                    xBar = xBar + xInterval;
                }

                ans = computeXTicksAndValues(xStart,xInterval,barWidth);
                //console.log(JSON.stringify(ans));
                xTicksPositions = ans.ticks
                xTrialIndex = ans.labels

                // Computing the yTickPostion;
                yTickInterval = Math.round(highestValue/(vmNYLabels+1));
                yLabel = yTickInterval;
                for (i = 0; i < vmNYLabels; i++){
                    yTickValues.push(yLabel);
                    yTickPositions.push(air_h/2 + (effectiveHeight - verticalScale*yLabel));
                    yLabel = yLabel + yTickInterval;
                }

            }
            ///////////////////////////////////////////////// DOING THE GRAPH (LINE) //////////////////////////////////////////////////////
            else{
                effectiveWidth = width*vmGraphWidth*vmGraphEffectiveWidth;
                effectiveHeight = height*vmGraphHeight*(1 - vmLineGraphVerticalWhiteSpace);
                verticalWhitespaceFromHorizontalAxis = vmLineGraphVerticalWhiteSpace*height*vmGraphHeight;
                verticalScale = -effectiveHeight/(highestValue - lowestValue);
                verticalOffset = air_h/2 - highestValue*verticalScale;

                // A just in case check.
                if (vmDataPoints.length < 2) return

                ctx.beginPath();
                ctx.strokeStyle = vmLineGraphColor;


                // Starting x value
                x = ((1-vmGraphEffectiveWidth)/2)*width*vmGraphWidth + air_w/2
                xStart = x;
                xInterval = effectiveWidth/(vmDataPoints.length-1);

                ctx.moveTo(x,verticalScale*vmDataPoints[0] + verticalOffset);

                for (i = 1; i < vmDataPoints.length; i++){
                    x = x + xInterval
                    y = verticalScale*vmDataPoints[i] + verticalOffset;
                    ctx.lineTo(x,y);
                    ctx.moveTo(x,y);
                }
                ctx.stroke();

                // Computing the xTick positions.
                ans = computeXTicksAndValues(xStart,xInterval,0);
                xTicksPositions = ans.ticks
                xTrialIndex = ans.labels

                // Computing the yTickPostion;
                yTickInterval = (highestValue - lowestValue)/(vmNYLabels+1);
                yLabel = yTickInterval + lowestValue;
                //console.log("yTickInterval" + yTickInterval);
                for (i = 0; i < vmNYLabels; i++){
                    yTickValues.push(yLabel.toFixed(1));
                    yTickPositions.push(verticalScale*yLabel + verticalOffset);
                    yLabel = yLabel + yTickInterval;
                    //console.log("yLabel " + yLabel);
                }

            }

            ///////////////////////////////////////////////// DRAWING THE REFERENCE //////////////////////////////////////////////////////
            // After doing the graph we do the reference.
            ctx.beginPath();
            ctx.strokeStyle = vmRefLineColor;
            //ctx.setLineDash([2,1,2])
            x = xStart;
            if (vmGraphType == "bar"){
                for (i = 0; i < vmRefDataPoints.length; i++){
                    y = air_h/2 + (effectiveHeight - vmRefDataPoints[i]*verticalScale);
                    //console.log("Bar ref y pos " + y + " from " + vmRefDataPoints[i]);
                    //y = air_h/2 + (effectiveHeight - verticalScale*y)
                    ctx.moveTo(x,y)
                    ctx.lineTo(x+barWidth,y);
                    x = x + xInterval;
                }
            }
            else {
                y = vmRefDataPoints[0]*verticalScale + verticalOffset;
                ctx.moveTo(x,y);
                for (i = 1; i < vmRefDataPoints.length; i++){
                    x = x + xInterval;
                    y = vmRefDataPoints[i]*verticalScale + verticalOffset;
                    //console.log("Line REF y: " + y + " from " + vmRefDataPoints[i]);
                    ctx.lineTo(x,y);
                    ctx.moveTo(x,y)
                }
            }
            ctx.stroke();

            ///////////////////////////////////////////////// DRAWING X TICKS //////////////////////////////////////////////////////
            ctx.beginPath();
            ctx.strokeStyle = vmAxisColor
            ctx.setLineDash([]);
            var startY = height-air_h/2; // The Y position of the horizontal axis.
            var endY = startY + height*vmXTickHeight;
            for (i = 0; i < xTicksPositions.length; i++){
                ctx.moveTo(xTicksPositions[i],startY);
                ctx.lineTo(xTicksPositions[i],endY);
            }
            ctx.stroke();

            ///////////////////////////////////////////////// DRAWING X LABELS //////////////////////////////////////////////////////
            ctx.beginPath();
            ctx.textAlign = "center";
            ctx.fillStyle = vmTextColor;
            ctx.strokeStyle = vmTextColor;
            ctx.font = '%1pt "%2"'.arg(9*viewHome.vmScale).arg(robotoR.name);
            var yTextValue = (1-vmTextBottomMargin)*height;
            for (i = 0; i < xTicksPositions.length; i++){
                var text = vmTrialText + " " + xTrialIndex[i];
                ctx.fillText(text,xTicksPositions[i],yTextValue);
            }

            ///////////////////////////////////////////////// DRAWING Y TICKS //////////////////////////////////////////////////////
            ctx.beginPath();
            ctx.strokeStyle = vmAxisColor
            ctx.setLineDash([]);
            var startX = air_w/2; // The X position of the vertical axis.
            var endX = startX - width*vmYTickWidth;
            for (i = 0; i < yTickPositions.length; i++){
                ctx.moveTo(startX,yTickPositions[i]);
                ctx.lineTo(endX,yTickPositions[i]);
            }
            ctx.stroke();

            ///////////////////////////////////////////////// DRAWING Y LABELS //////////////////////////////////////////////////////
            ctx.beginPath();
            ctx.textAlign = "right";
            ctx.textBaseline = "middle";
            ctx.fillStyle = vmTextColor;
            ctx.strokeStyle = vmTextColor;
            ctx.font = '%1pt "%2"'.arg(9*viewHome.vmScale).arg(robotoR.name);
            var xTextValue = endX-endX*vmTextRightMargin
            for (i = 0; i < yTickPositions.length; i++){
                ctx.fillText(yTickValues[i],xTextValue,yTickPositions[i]);
            }


        }

    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.04
        spacing: mainWindow.width*0.15

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: "BACK"//loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexFinishedStudies;
            }
        }

        VMButton{
            id: btnSend
            height: mainWindow.height*0.072
            vmText: "SEND";
            vmFont: viewHome.gothamM.name
            onClicked: {
                loader.sendStudy();
                connectionDialog.open();
            }
        }
    }

}
