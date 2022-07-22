import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: calibrationValidationDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    signal calibrationValidated();
    signal requestReCalibration();

    // Horizontal space between the border of the screen and the start of the screen representation and between the screen representation and the middle of the dialog
    property double vmHorizotalMarginToScreenRepresentation: 0.08*dialog.width
    // The width of the screen representation
    property double vmScreenRepresentationWidth: dialog.width/2 - 2*vmHorizotalMarginToScreenRepresentation
    // The height of the screen representation
    property double vmScreenRepresentationHeight: vmScreenRepresentationWidth
    // The ratio of the screen representation width to the actual HMD resolution width
    property double vmKx: 1;
    // The ratio of the screen representation height to the actual HMD resolution height
    property double vmKy: 1;
    // The list of upper left corner for each of the targets.
    property var vmCalibrationData: ({})

    ////////////////////////// CONVERSION FUNCTIONS: 2D HMD Data to Plot Data ////////////////////////

    function configuringRenderingParameters(cdata,width,height){
        vmCalibrationData = cdata;

        vmKx = vmScreenRepresentationWidth/width;
        vmKy = vmScreenRepresentationHeight/height;
        vmScreenRepresentationHeight = vmScreenRepresentationWidth*height/width;

    }

    ////////////////////////// DRAWING FUNCTIONS ////////////////////////
    function renderScreenRepresentation(ctx){
        let ry = 0.02*vmScreenRepresentationHeight;
        let rx = 0.02*vmScreenRepresentationWidth;
        ctx.beginPath();
        ctx.fillStyle = VMGlobals.vmGrayUnselectedBorder
        ctx.strokeStyle = VMGlobals.vmGrayUnselectedBorder
        ctx.roundedRect(0,0,vmScreenRepresentationWidth,vmScreenRepresentationHeight,rx,ry)
        ctx.fill();
    }

    function redrawCanvas(){
        leftEyeCanvas.requestPaint();
        rightEyeCanvas.requestPaint();
    }

    function renderTargets(ctx){

        let upperLeftCorners = vmCalibrationData["calibration_target_location"];
        let Dx = vmCalibrationData["calibration_target_diameter"]*vmKx;
        let Dy = vmCalibrationData["calibration_target_diameter"]*vmKy;
        let Rx = Dx/2;
        let Ry = Dy/2;

        for (let i = 0; i < upperLeftCorners.length; i++){
            let x = upperLeftCorners[i]["x"]*vmKx;
            let y = upperLeftCorners[i]["y"]*vmKy;
            ctx.beginPath();
            ctx.fillStyle = VMGlobals.vmWhite
            ctx.strokeStyle = VMGlobals.vmGrayDialogDivider
            ctx.roundedRect(x,y,Dx,Dy,Rx,Ry)
            ctx.stroke();
            ctx.fill();
        }

    }

    function renderDataPoints(ctx, left){
        var datapoints;

        let array_names = [];
        let base_key_name = "validation_target_";

        if (left){
            datapoints = vmCalibrationData["left_eye_validation_data"];
        }
        else {
            datapoints = vmCalibrationData["right_eye_validation_data"];
        }

        let R = vmCalibrationData["calibration_target_diameter"]*vmKx*0.05;

        for (let key in datapoints){

            // We filter out about the first third of data points in order to remove
            // data points for transitions.
            let NFilter = Math.round(datapoints[key].length/3);

            for (let i = NFilter; i < datapoints[key].length; i++){
                let x = datapoints[key][i]["x"]*vmKx;
                let y = datapoints[key][i]["y"]*vmKy;
                ctx.beginPath();
                ctx.fillStyle = VMGlobals.vmBlueSelected
                ctx.strokeStyle = VMGlobals.vmBlueSelected
                ctx.roundedRect(x,y,2*R,2*R,R,R)
                ctx.stroke();
                ctx.fill();
            }
        }


    }

    function renderCompleteScreenRepresentation(ctx,left){
        renderScreenRepresentation(ctx);
        renderTargets(ctx)
        renderDataPoints(ctx,left)
    }

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        visible = true
    }

    function close(){
        visible = false
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(1093)
        height: VMGlobals.adjustHeight(614)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: loader.getStringForKey("viewevaluation_calib_res")
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.horizontalCenter: dialog.horizontalCenter
    }

    Text {
        id: leftEyeTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewevaluation_left_eye")
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: leftEyeCanvas.horizontalCenter
    }

    Text {
        id: rightEyeTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewevaluation_right_eye")
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: rightEyeCanvas.horizontalCenter
    }

    /////////////////////////////////////////////////////////////////////

    Rectangle {
        id: halfScreenDivider
        width: VMGlobals.adjustWidth(1)
        radius: VMGlobals.adjustWidth(1)
        height: leftEyeCanvas.height*0.9
        anchors.verticalCenter: leftEyeCanvas.verticalCenter
        anchors.horizontalCenter: dialog.horizontalCenter
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    /////////////////////////////////////////////////////////////////////

    Canvas {
        id: leftEyeCanvas
        width: vmScreenRepresentationWidth
        height: vmScreenRepresentationHeight
        contextType: "2d"
        anchors.left: dialog.left;
        anchors.leftMargin: vmHorizotalMarginToScreenRepresentation
        anchors.top: leftEyeTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)

        onPaint: {

            var ctx = getContext("2d")
            ctx.reset();
            renderCompleteScreenRepresentation(ctx,true)

        }

    }

    Canvas {
        id: rightEyeCanvas
        width: vmScreenRepresentationWidth
        height: vmScreenRepresentationHeight
        contextType: "2d"
        anchors.right: dialog.right;
        anchors.rightMargin: vmHorizotalMarginToScreenRepresentation
        anchors.top: leftEyeTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)

        onPaint: {

            var ctx = getContext("2d")
            ctx.reset();
            renderCompleteScreenRepresentation(ctx,false)
        }

    }


    /////////////////////////////////////////////////////////////////////

    Rectangle {
        id: divider
        width: dialog.width
        height: VMGlobals.adjustHeight(1)
        anchors.bottom: dialog.bottom
        anchors.left: dialog.left
        anchors.bottomMargin: VMGlobals.adjustHeight(84) - height
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    VMButton {
        id: continueButton
        vmText: loader.getStringForKey("viewevaluation_continue");
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            calibrationValidated()
        }
    }

    VMButton {
        id: restartCalibrationButton
        vmText: loader.getStringForKey("viewevaluation_calib_restart");
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.left: dialog.left
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            requestReCalibration()
        }
    }



}
