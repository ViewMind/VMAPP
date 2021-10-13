import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewStudyDone
    width: mainWindow.width
    height: mainWindow.height

    // The order of the graphs.
    readonly property int vmINDEX_GRAPH_POINTS: 0
    readonly property int vmINDEX_FIXATIONS: 1
    readonly property int vmINDEX_FREQ: 2

    readonly property int vmAPI_REPORT_REQUEST: 2

    readonly property int vmFAIL_CODE_NONE: 0
    readonly property int vmFAIL_CODE_SERVER: 2

    readonly property int vmMAX_L_COMMMENT_LINES: 5
    readonly property int vmMAX_C_COMMENT_CHARS: 50

    property var vmStudyNameMap: []
    property var vmStdStudyNameByIndex: [];
    property var vmSubTitleList: [];

    property int vmQCIndex: 0
    property bool vmQCIndexOK: false

    property string vmQCIndexExplanation: ""
    property string vmQCIndexExplantionTitle: ""


    function loadStudiesAndGraphs(){

        // Selecting the different types of graphps.
        var buttonList = [
                    loader.getStringForKey("viewQC_GraphPoints"),
                    loader.getStringForKey("viewQC_GraphFixations"),
                    loader.getStringForKey("viewQC_GraphFreq")
                ];

        vmSubTitleList = [
                    loader.getStringForKey("viewQC_SubPoints"),
                    loader.getStringForKey("viewQC_SubFixations"),
                    loader.getStringForKey("viewQC_SubFrequency")
                ];

        qcGraphList.clear();
        for (var i =0 ; i < buttonList.length; i++){
            qcGraphList.append({"vmItemIndex" : i, "vmIsSelected" : false, "vmText": buttonList[i], "vmChangeToTrigger": 0});
        }

        // Creating the study name language map
        var nameList = loader.getStringListForKey("viewQC_StudyNameMap");
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
        loadGraph();
    }

    function loadGraph(){
        //console.log("Called log graph");
        var selectedStudy = vmStdStudyNameByIndex[qcStudyView.currentIndex];
        var selectedGraph = qcGraphsView.currentIndex;
        var data = loader.getStudyGraphData(selectedStudy,selectedGraph);
        //console.log(JSON.stringify(data));

        if (selectedGraph === vmINDEX_FREQ){
            graph.vmGraphType = "line";

            // Getting the reference data to figure out the scale.
            // All values are the same. So the first of the upper bound is the highest value and the last of the lowest bound is the lower value.
            graph.vmFixedScale = [data["UpperREFData"][0],0];
                                  //data["LowerREFData"][0]];

            //console.log("Fixed Scaled")
            //console.log(graph.vmFixedScale);

        }
        else{
            graph.vmGraphType = "bar";
            graph.vmFixedScale = [];
        }

        // Get Explanation Text
        vmQCIndexExplanation = "";
        if (selectedGraph === vmINDEX_FREQ){
            vmQCIndexExplanation = loader.getStringForKey("viewQC_limitSampleF")
        }
        else if (selectedGraph === vmINDEX_FIXATIONS){
            vmQCIndexExplanation = loader.getStringForKey("viewQC_limitFixations")
        }
        else if (selectedGraph === vmINDEX_GRAPH_POINTS){
            vmQCIndexExplanation = loader.getStringForKey("viewQC_limitDataPoints")
        }

        // Separating the explation text: The first line is the title.
        vmQCIndexExplanation = vmQCIndexExplanation.replace("<<I>>",data["QCThreshold"]);
        var text_parts = vmQCIndexExplanation.split("\n");
        vmQCIndexExplantionTitle = text_parts[0];
        text_parts.shift();
        vmQCIndexExplanation =text_parts.join("\n");

        if ((qcGraphsView.currentIndex < 0) || (qcGraphsView.currentIndex >= vmSubTitleList.length)) return;

        viewGraphTitle.text = vmSubTitleList[qcGraphsView.currentIndex];

        graph.vmDataPoints = data["GraphData"];
        var temp = {"upper" : data["UpperREFData"],"lower" : data["LowerREFData"]};
        graph.vmRefDataPoints = temp;

        vmQCIndex = Math.round(data["QCIndex"]);
        vmQCIndexOK = data["QCOk"];
        // console.log("Setting reg to " + JSON.stringify(temp));
        graph.graph();

    }

    function verifyCommentLength(comment){
        // Preparing the error message.
        var error = loader.getStringForKey("viewQC_commentSizeError");
        // Language string needs to have these place holders for the maximum number of lines and characters
        error = error.replace("<<L>>",vmMAX_L_COMMMENT_LINES)
        error = error.replace("<<C>>",vmMAX_C_COMMENT_CHARS)

        // Checking that every line is less htan the number of chars and that the total number of lines is less than the max.
        var lines = comment.split("\n");
        if (lines.length > vmMAX_L_COMMMENT_LINES) return error;
        for (var l = 0; l < lines.length; l++){
            if (lines[l].length > vmMAX_C_COMMENT_CHARS) return error;
        }

        return "";
    }

    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            connectionDialog.close();

            var failCode = loader.wasThereAnProcessingUploadError();

            // This check needs to be done ONLY when on this screen.
            if (( failCode !== vmFAIL_CODE_NONE ) && (swiperControl.currentIndex === swiperControl.vmIndexViewQC)){

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

    VMDialogBase {

        property string vmTitle: loader.getStringForKey("viewQC_WaitTitle")
        property string vmMessage: loader.getStringForKey("viewQC_WaitSubtitle")

        id: connectionDialog;
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
        vmNoCloseButton: true

        onWidthChanged: {
            connectionDialog.repositionSlideAnimation();
        }

        onHeightChanged: {
            connectionDialog.repositionSlideAnimation();
        }

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
            transformOrigin: Item.TopLeft
            scale: viewHome.vmScale
            visible: true
            onScaleChanged: {
                connectionDialog.repositionSlideAnimation();
            }
        }

        function repositionSlideAnimation(){
            slideAnimation.y = (connectionDialog.height - slideAnimation.height*viewHome.vmScale)/2
            slideAnimation.x = (connectionDialog.width - slideAnimation.width*viewHome.vmScale)/2
        }

    }

    VMDialogBase {
        id: discardOrCommentDialog

        width: mainWindow.width*0.6
        height: mainWindow.height*0.8

        property bool vmConfigureForDiscard: true
        readonly property real vmVerticalSpaceSmall: mainWindow.height*0.01
        readonly property real vmVerticalSpaceMedium: mainWindow.height*0.02
        readonly property real vmVerticalSpaceLarge: mainWindow.height*0.03

        property var reasonStringCodeList: []

        onOpened: {
            dorcComments.vmErrorMsg = "";
            dorcComments.setText("");
            if (!vmConfigureForDiscard){
                dorcComments.enabled = true;
            }
            else{
                dorcDiscardResonSelect.setSelection(0);
                dorcComments.enabled = false;
            }
        }

        Text {
            id: dorcDiagTitle
            font.family: gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top:  parent.top
            anchors.topMargin: discardOrCommentDialog.vmVerticalSpaceLarge;
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: {
                var title_string = "";
                if (discardOrCommentDialog.vmConfigureForDiscard) title_string = "viewQC_discardTitle";
                else title_string = "viewQC_commentTitle"
                return loader.getStringForKey(title_string);
            }
        }

        Text {
            id: dorcDiagSubTitle
            font.family: robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.horizontalCenter: dorcDiagTitle.horizontalCenter
            anchors.top: dorcDiagTitle.bottom
            anchors.topMargin: discardOrCommentDialog.vmVerticalSpaceMedium
            color: "#bcbec0"
            text: {
                var title_string = "";
                if (discardOrCommentDialog.vmConfigureForDiscard) title_string = "viewQC_discardSubTitle";
                else title_string = "viewQC_commentSubtitle"
                return loader.getStringForKey(title_string);
            }
        }

        Text {
            id: dorcDiscardReasonLabel
            font.family: robotoR.name
            font.pixelSize: 11*viewHome.vmScale
            anchors.left: dorcDiscardResonSelect.left
            anchors.bottom: dorcDiscardResonSelect.top
            anchors.bottomMargin: discardOrCommentDialog.vmVerticalSpaceSmall
            color: "#297fca"
            text: loader.getStringForKey("viewQC_discardReasonLabel");
            visible: discardOrCommentDialog.vmConfigureForDiscard
        }

        VMComboBox2 {
            id: dorcDiscardResonSelect
            z: 2
            width: dorcComments.width
            anchors.left: dorcComments.left
            anchors.bottom: dorcComments.top
            anchors.bottomMargin: discardOrCommentDialog.vmVerticalSpaceSmall
            Component.onCompleted: {
                var reasonMap = loader.getStringListForKey("viewQC_discardReasons");

                // In order to make reasons both function with a code and be future proof the string map will contain both the reason string code
                // (so that it's order in the string list is irrelevant) and the string itself (language dependant string to show). They are separed by a -
                discardOrCommentDialog.reasonStringCodeList = [];
                var displayList = [];

                for (var i = 0; i < reasonMap.length; i++){
                    var key_and_value = reasonMap[i].split("-");
                    discardOrCommentDialog.reasonStringCodeList.push(key_and_value[0])
                    displayList.push(key_and_value[1])
                }

                dorcDiscardResonSelect.setModelList(displayList);
            }
            visible: discardOrCommentDialog.vmConfigureForDiscard
            onVmCurrentIndexChanged: {
                var reason = discardOrCommentDialog.reasonStringCodeList[vmCurrentIndex];
                if (reason === "other") dorcComments.enabled = true;
                else dorcComments.enabled = false;
            }
        }

        VMTextInputArea {
            id: dorcComments
            width: parent.width*0.6
            height: parent.height*0.4
            anchors.bottom: dorcButtonRow.top
            anchors.bottomMargin: {
                if (discardOrCommentDialog.vmConfigureForDiscard) return discardOrCommentDialog.vmVerticalSpaceLarge*3
                else discardOrCommentDialog.vmVerticalSpaceLarge*5
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            id: dorcButtonRow
            spacing: parent.width*0.5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: discardOrCommentDialog.vmVerticalSpaceMedium
            anchors.horizontalCenter: parent.horizontalCenter

            VMButton{
                id: btnCancelAction
                height: mainWindow.height*0.072
                vmText: loader.getStringForKey("viewQC_btnCancel");
                vmFont: viewHome.gothamM.name
                vmInvertColors: true
                onClicked: {
                    discardOrCommentDialog.close()
                }
            }

            VMButton{
                id: btnAction
                height: mainWindow.height*0.072
                vmText: {
                    var title_string = "";
                    if (discardOrCommentDialog.vmConfigureForDiscard) title_string = "viewQC_btnDiscard";
                    else title_string = "viewQC_btnSend"
                    return loader.getStringForKey(title_string);
                }
                vmFont: viewHome.gothamM.name
                onClicked: {
                    dorcComments.vmErrorMsg = "";
                    var error = verifyCommentLength(dorcComments.getText())
                    if (error === ""){
                        if (discardOrCommentDialog.vmConfigureForDiscard){
                            var discard_code_reason = discardOrCommentDialog.reasonStringCodeList[dorcDiscardResonSelect.vmCurrentIndex];
                            var comment = "";
                            if (discard_code_reason === "other") comment = dorcComments.getText();
                            loader.setDiscardReasonAndComment(discard_code_reason,comment)
                        }
                        else{
                            loader.setDiscardReasonAndComment("",dorcComments.getText())
                        }
                        loader.sendStudy();
                        discardOrCommentDialog.close()
                        connectionDialog.open();
                    }
                    else{
                        dorcComments.vmErrorMsg = error;
                    }
                }
            }
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
        text: loader.getStringForKey("viewQC_Title");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.horizontalCenter: viewTitle.horizontalCenter
        anchors.top: viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.01
        color: "#bcbec0"
        text: loader.getStringForKey("viewQC_subTitle");
    }

    Text {
        id: viewGraphTitle
        font.family: robotoR.name
        font.pixelSize: 15*viewHome.vmScale
        anchors.horizontalCenter: graph.horizontalCenter
        anchors.bottom: graph.top
        anchors.bottomMargin: mainWindow.height*0.01
        color: "#3c3e30"
        text: ""
    }

    ListModel {
        id: qcGraphList
    }

    ListModel {
        id: qcStudyList
    }

    //////////////////////////////////////////////// SELECTORS ///////////////////////////////////////////////////

    Row {

        id: studySelectorRow
        spacing: 0
        anchors.top: graphSelectionRow.top
        anchors.left: graph.left

        Rectangle {
            id: headerStudy
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: headerGraph.width;
            height: headerGraph.height
            Text {
                id: studyNameText
                text: loader.getStringForKey("viewQC_Study");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        ListView {
            id: qcStudyView
            width: headerStudy.width*1.5*qcStudyList.count;
            height: headerStudy.height;
            model: qcStudyList
            orientation: ListView.Horizontal
            delegate: VMSelectorRect {
                width: headerStudy.width*1.5
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

    Row {

        id: graphSelectionRow
        spacing: 0
        anchors.bottom: viewGraphTitle.top
        anchors.right: graph.right
        anchors.bottomMargin: mainWindow.height*0.02;

        Rectangle {
            id: headerGraph
            color: "#ffffff"
            border.width: mainWindow.width*0.002
            border.color: "#EDEDEE"
            radius: 4
            width: mainWindow.width*0.08;
            height: mainWindow.height*0.043
            Text {
                id: graphTitleText
                text: loader.getStringForKey("viewQC_GraphTitle");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        ListView {
            id: qcGraphsView
            width: headerGraph.width*1.5*qcGraphList.count;
            height: headerGraph.height;
            model: qcGraphList
            orientation: ListView.Horizontal
            delegate: VMSelectorRect {
                width: headerGraph.width*1.5
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
    }

    //////////////////////////////////////////////// CANVAS /////////////////////////////////////////////////////

    Canvas {
        id: graph
        width: mainWindow.width*0.75
        height: mainWindow.height*0.45
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.05;
        anchors.bottom: buttonRow.top
        anchors.bottomMargin: mainWindow.height*0.03;

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
        property string vmGraphType: "bar";  // Other option is line.
        property var vmDataPoints: [];   // Array of datapoints.
        property var vmRefDataPoints: [];
        property int vmNXLabels: 5;
        property int vmNYLabels: 5;
        property var vmFixedScale: [];

        // Texts
        property string vmTrialText: "Trial";

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

            //console.log(JSON.stringify(vmRefDataPoints));

            for (i = 0; i < vmRefDataPoints.upper.length; i++){
                if (max < vmRefDataPoints[i]) max = vmRefDataPoints[i];
                if (min > vmRefDataPoints[i]) min = vmRefDataPoints[i];
            }

            for (i = 0; i < vmRefDataPoints.lower.length; i++){
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

            // Computing the extremes, if the scale is not fixed.
            var highestValue;
            var lowestValue;
            if (vmFixedScale.length != 2){
                var minmax = getExtremesFromDataPoints();
                highestValue = minmax[0];
                lowestValue = minmax[1];
            }
            else{
                highestValue = vmFixedScale[0];
                lowestValue = vmFixedScale[1];
            }

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
            var key, plotvalues;
            if (vmGraphType == "bar"){
                for (key in vmRefDataPoints){
                    x = xStart;
                    plotvalues = vmRefDataPoints[key];
                    for (i = 0; i < plotvalues.length; i++){
                        y = air_h/2 + (effectiveHeight - plotvalues[i]*verticalScale);
                        //console.log("Bar ref y pos " + y + " from " + plotvalues[i]);
                        //y = air_h/2 + (effectiveHeight - verticalScale*y)
                        ctx.moveTo(x,y)
                        ctx.lineTo(x+barWidth,y);
                        x = x + xInterval;
                    }
                }
            }
            else {
                for (key in vmRefDataPoints){
                    plotvalues = vmRefDataPoints[key];
                    x = xStart;
                    y = plotvalues[0]*verticalScale + verticalOffset;
                    ctx.moveTo(x,y);
                    for (i = 1; i < plotvalues.length; i++){
                        x = x + xInterval;
                        y = plotvalues[i]*verticalScale + verticalOffset;
                        //console.log("Line REF y: " + y + " from " + vmRefDataPoints[i]);
                        ctx.lineTo(x,y);
                        ctx.moveTo(x,y)
                    }
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

    //////////////////////////////////////////////// INDEX QC /////////////////////////////////////////////////////

    Rectangle {

        readonly property real wMultiplier : 0.9

        id: qcIndexRect
        color: "#ebf3fa"
        width: wMultiplier*(mainWindow.width - graph.width - graph.anchors.leftMargin);
        height: graph.height
        radius: width*0.02
        anchors.top: graph.top
        anchors.left: graph.right
        anchors.leftMargin: ((1-wMultiplier)/2)*(mainWindow.width - graph.width - graph.anchors.leftMargin)

        Text {
            id: qcIndexTitle
            text: loader.getStringForKey("viewQC_QC_Index")
            color: "#297fca"
            font.family: gothamB.name
            font.pixelSize: 20*viewHome.vmScale
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.03
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: qcIndexValue
            text: vmQCIndex
            font.family: gothamB.name
            font.pixelSize: 30*viewHome.vmScale
            color: vmQCIndexOK ? ("#47994a") : ("#cc3b26")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: qcIndexTitle.bottom
            anchors.topMargin: mainWindow.height*0.02
        }

        Text {
            id: qcIndextTextValueReference
            text: vmQCIndexExplantionTitle
            color: "#297fca"
            font.family: gothamB.name
            font.pixelSize: 12*viewHome.vmScale
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: qcIndextTextExplanation.top
            anchors.bottomMargin: mainWindow.height*0.01
        }

        Text {
            id: qcIndextTextExplanation
            text: vmQCIndexExplanation
            color: "#297fca"
            font.family: gothamR.name
            font.pixelSize: 12*viewHome.vmScale
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.05
        }

    }



    //////////////////////////////////////////////// BUTTONS /////////////////////////////////////////////////////

    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.04
        spacing: mainWindow.width*0.20

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey("viewQC_btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexFinishedStudies;
            }
        }

        VMButton{
            id: btnSend
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey("viewQC_btnSend");
            vmFont: viewHome.gothamM.name
            onClicked: {
                discardOrCommentDialog.vmConfigureForDiscard = false;
                discardOrCommentDialog.open();
            }
        }

        VMButton{
            id: btnDiscard
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey("viewQC_btnDiscard");
            vmFont: viewHome.gothamM.name
            onClicked: {
                discardOrCommentDialog.vmConfigureForDiscard = true;
                discardOrCommentDialog.open();
            }
        }


    }

}
