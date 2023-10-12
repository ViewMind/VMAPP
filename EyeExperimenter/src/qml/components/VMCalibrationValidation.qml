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
    signal requestReCalibration(bool slow);

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
    // The complete calibration validation structure.
    property var vmCalibrationData: ({})
    // Flag to determine where we are drawing the 3D mode validation screen or the 2D Mode one.
    property bool vm3DModeValidationScreen: false

    property bool vmIsLeftEyeValidated: false
    property bool vmIsRightEyeValidated: false;


    ////////////////////////// CONVERSION FUNCTIONS: 2D HMD Data to Plot Data ////////////////////////

    function configuringRenderingParameters(cdata,width,height){
        vmCalibrationData = cdata;

        //console.log(JSON.stringify(cdata));

        // Checking if each eye is validated.
        vmIsLeftEyeValidated =  vmCalibrationData["left_eye_data"]["is_validated"];
        vmIsRightEyeValidated =  vmCalibrationData["right_eye_data"]["is_validated"];

        setEyeResultTextElement(rigthEyeResult,vmIsRightEyeValidated)
        setEyeResultTextElement(leftEyeResult,vmIsLeftEyeValidated)

        vmKx = vmScreenRepresentationWidth/width;
        vmKy = vmScreenRepresentationHeight/height;
        vmScreenRepresentationHeight = vmScreenRepresentationWidth*height/width;

        if (vmCalibrationData["is_3d"]){
            //console.log("This is a 3D Calibration");
            vm3DModeValidationScreen = true;
        }
        else {
            vm3DModeValidationScreen = false;
        }

    }

    ////////////////////////// DRAWING FUNCTIONS ////////////////////////
    function renderScreenRepresentation(ctx,left){

        let fillColor = VMGlobals.vmGrayUnselectedBorder

        if (left && !vmIsLeftEyeValidated)   fillColor = VMGlobals.vmRedCalibrationFailed;
        if (!left && !vmIsRightEyeValidated) fillColor = VMGlobals.vmRedCalibrationFailed;

        let ry = 0.02*vmScreenRepresentationHeight;
        let rx = 0.02*vmScreenRepresentationWidth;
        ctx.beginPath();
        ctx.fillStyle = fillColor
        ctx.roundedRect(0,0,vmScreenRepresentationWidth,vmScreenRepresentationHeight,rx,ry)
        ctx.fill();

    }

    function setEyeResultTextElement(textElement, validated){
        textElement.text = validated ? loader.getStringForKey("viewevaluation_success") : loader.getStringForKey("viewevaluation_failed");
        textElement.color = validated ? VMGlobals.vmBlackText : VMGlobals.vmRedError
    }

    function redrawCanvas(){
        leftEyeCanvas.requestPaint();
        rightEyeCanvas.requestPaint();
    }

    function renderTargets(ctx){

        let upperLeftCorners = vmCalibrationData["calibration_target_location"];

        //console.log("Printing calibration target locations");
        //console.log(JSON.stringify(vmCalibrationData["calibration_target_location"]));

        if (upperLeftCorners === undefined) return;
        let Dx = vmCalibrationData["calibration_target_diameter"]*vmKx;
        let Dy = vmCalibrationData["calibration_target_diameter"]*vmKy;
        let Rx = Dx/2;
        let Ry = Dy/2;

        for (let i = 0; i < upperLeftCorners.length; i++){
            let x = upperLeftCorners[i]["x"]*vmKx;
            let y = upperLeftCorners[i]["y"]*vmKy;
            //console.log("Printing Target at " + x + ", " + y);
            ctx.beginPath();
            ctx.fillStyle = VMGlobals.vmWhite
            ctx.strokeStyle = VMGlobals.vmGrayDialogDivider
            ctx.roundedRect(x-Rx,y-Ry,Dx,Dy,Rx,Ry)
            ctx.stroke();
            ctx.fill();
        }

    }

    function renderColoredTargetsWithHitPercents(ctx, left){

        let upperLeftCorners = vmCalibrationData["calibration_target_location"];
        let percentData = vmCalibrationData["calibration_target_percents"];
        if (left) percentData = percentData["l"];
        else percentData = percentData["r"];

        if (upperLeftCorners === undefined) return;
        if (upperLeftCorners.length !== percentData.length){
            console.log("The number of targets is " + upperLeftCorners.length + " but the number of percents is " + percentData.length);
            return;
        }

        // We multiply the diameters times two as the circles are representations now and we want to make suer the text fits.
        let scaling = 1.2;
        let Dx = vmCalibrationData["calibration_target_diameter"]*vmKx*scaling;
        let Dy = vmCalibrationData["calibration_target_diameter"]*vmKy*scaling;

        // Since we are scaling the circle, in order for the center to be in the same position (which is x + original_diameter/2 and y + original_diameter/2)
        // we need to compute a new offset for the top left corner of the bounding square for the circle. We compute here only the offset to the original
        // top left corner. If Scaling is larger than 1 (as it should be) these offsets need to be substracted.
        let offsetX = Dx*(scaling - 1)/2;
        let offsetY = Dy*(scaling - 1)/2;

        let Rx = Dx/2;
        let Ry = Dy/2;

        let color_change_enable = false;
        // Color change is enabled only when the eye is NOT validated.
        if (left){
            if (!vmIsLeftEyeValidated) color_change_enable = true;
        }
        else {
            if (!vmIsRightEyeValidated) color_change_enable = true;
        }


        for (let i = 0; i < upperLeftCorners.length; i++){

            let calibPointData = percentData[i]

            // We draw the circle.
            let x = upperLeftCorners[i]["x"]*vmKx;
            let y = upperLeftCorners[i]["y"]*vmKy;
            ctx.beginPath();
            ctx.fillStyle = VMGlobals.vmWhite
            ctx.strokeStyle = VMGlobals.vmGrayDialogDivider
            ctx.roundedRect(x-offsetX-Rx,y-offsetY-Ry,Dx,Dy,Rx,Ry)
            ctx.stroke();
            ctx.fill();

            // We now draw numbers considering that wheter the calibration point passed or not.
            let color = VMGlobals.vmBlueSelected
            if ((!calibPointData["ok"]) && (color_change_enable)) color = VMGlobals.vmRedError;

            let pText = Math.round(calibPointData["p"]) + "%";
            let cx = x - offsetX + Rx
            let cy = y - offsetY + Ry

            ctx.beginPath()
            let fontString = '700 %1px "%2"'.arg(VMGlobals.vmFontLarge).arg(mainWindow.vmSegoeBold.name);
            //console.log("Using font " + fontString)
            ctx.font = fontString
            ctx.textBaseline = "middle"
            ctx.textAlign  = "center"
            ctx.fillStyle = color
            ctx.strokeStyle = color
            //console.log("Drawing text " + vmPercent);
            ctx.fillText(pText,cx-Rx,cy-Ry)

        }

    }

    function renderDataPoints(ctx, left){
        var datapoints;

        let array_names = [];

        let start_indexes = vmCalibrationData["calibration_data_use_start_index"];
        //console.log(JSON.stringify(start_indexes));

        if (left){
            datapoints = vmCalibrationData["left_eye_data"];
        }
        else {
            datapoints = vmCalibrationData["right_eye_data"];
        }

        let R = vmCalibrationData["calibration_target_diameter"]*vmKx*0.05;

        for (let key in datapoints){

            if (key === "is_validated") continue; // Its part of the structure but does not contain validation data.

            // let start_index = start_indexes[key];
            let start_index = 0;

            let parts = key.split("_");

            if (parts.length !== 3){
                console.log("Unexpected key format when plotting calibration data points: " + key)
                continue;
            }

            let index = parts[2]; // The third part of the key name is the calibration point index
            start_index = start_indexes[index];            

            //console.log("The key is now " + key + " so the index would be " + index +  " and the start would be " + start_index);

//            for (let i = 0; i < datapoints[key].length; i++){
//                let x = datapoints[key][i]["x"]*vmKx;
//                let y = datapoints[key][i]["y"]*vmKy;
//                ctx.beginPath();
//                ctx.fillStyle = "#00ff00"
//                ctx.strokeStyle = "#00ff00"
//                ctx.roundedRect(x,y,2*R,2*R,R,R)
//                ctx.stroke();
//                ctx.fill();
//            }

            for (let i = start_index; i < datapoints[key].length; i++){
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
        renderScreenRepresentation(ctx,left);
        if (vm3DModeValidationScreen){
            renderColoredTargetsWithHitPercents(ctx,left);
        }
        else {
            renderTargets(ctx)
            renderDataPoints(ctx,left)
        }
    }

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        flowControl.hideRenderWindow();
        visible = true
    }

    function close(){
        vmIsLeftEyeValidated = false;
        vmIsRightEyeValidated = false;
        visible = false
        flowControl.showRenderWindow();
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
        text: loader.getStringForKey("viewevaluation_left_eye") + ":  "
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        x: leftEyeCanvas.x
    }

    Text {
        id: leftEyeResult
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontBaseSize
        anchors.verticalCenter: leftEyeTitle.verticalCenter
        anchors.left: leftEyeTitle.right
        onWidthChanged: {
            leftEyeTitle.x = leftEyeCanvas.x + (leftEyeCanvas.width - leftEyeResult.width - leftEyeTitle.width)/2
        }
    }

    Text {
        id: rightEyeTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewevaluation_right_eye") + ":  "
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        //anchors.horizontalCenter: rightEyeCanvas.horizontalCenter
        x: rightEyeCanvas.x
    }

    Text {
        id: rigthEyeResult
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 600
        anchors.verticalCenter: rightEyeTitle.verticalCenter
        anchors.left: rightEyeTitle.right
        onWidthChanged: {
            rightEyeTitle.x = rightEyeCanvas.x + (rightEyeCanvas.width - rigthEyeResult.width - rightEyeTitle.width)/2
        }

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
        vmEnabled: vmIsLeftEyeValidated || vmIsRightEyeValidated
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
            calibrationValidated()
        }
    }

    VMButton {
        id: slowCalibrationButton
        vmButtonType: vmTypeSecondary
        vmText: loader.getStringForKey("viewevaluation_slow_calib");
        visible: !vmIsLeftEyeValidated && !vmIsRightEyeValidated
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        anchors.horizontalCenter: parent.horizontalCenter
        onClickSignal: {
            requestReCalibration(true)
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
            requestReCalibration(false)
        }
    }



}
