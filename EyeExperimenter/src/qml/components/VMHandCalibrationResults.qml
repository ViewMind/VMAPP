import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: handCalibrationResultsDialgo;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    signal calibrationValidated();
    signal requestReCalibration();

    // Horizontal space between the border of the dialog and the start of the hand area.
    // Also between the hand area and the middle of the dialog
    property double vmHorizotalMarginToHandArea: 0.08*dialog.width
    // The width of the hand area
    property double vmHandAreaWidth: dialog.width/2 - 2*vmHorizotalMarginToHandArea
    // The height of the hand area
    property double vmHandAreaHeight: vmHandAreaWidth*0.9
    // The distance from the vertical border of the hand area nearest the center to the vertical line representing
    // the horizontal limit
    property double vmHLimitDimension: vmHandAreaWidth*0.75;
    // The distance from the horizontal bottom of the hand area to the horizontal line representing
    // the vertical limit
    property double vmVLimitDimension: vmHandAreaHeight*0.75;

    // The horizontal space between the start of the person image and the border of the hand area nearest to the divider.
    property double vmHPersonOffset: vmHandAreaWidth*0.2
    // The vertical space between the bottom of the person image and the bottom of the hand area.
    property double vmVPersonOffset: vmHandAreaHeight*0.2;
    // The width of the hand graph
    property double vmHandGraphWidth: vmHandAreaWidth*0.1

    // Right and Left Hand Vertical and Horizontal passes.
    property bool vmRHH: false
    property bool vmRHV: false
    property bool vmLHH: false
    property bool vmLHV: false

    property bool vmUseLeftHand: false
    property bool vmUseRightHand: false


    ////////////////////////// CONVERSION FUNCTIONS: 2D HMD Data to Plot Data ////////////////////////

    function configureResults(horizontal, vertical, h_limit, v_limit, handsToUse){

        // First we need to define what hands are in use.
        if (handsToUse === 0){
            vmUseLeftHand = true;
            vmUseRightHand = false;
        }
        else if (handsToUse === 1){
            vmUseLeftHand = false;
            vmUseRightHand = true;
        }
        else {
            vmUseLeftHand = true;
            vmUseRightHand = true;
        }

        // Then we check if each hand passed.
        if (vmUseLeftHand) {
            vmLHH = horizontal[0] < h_limit;
            vmLHV = vertical[0] < v_limit;
        }
        else {
            vmLHH = true;
            vmLHV = true;
        }

        if (vmUseRightHand){
            vmRHH = horizontal[1] < h_limit;
            vmRHV = vertical[1] < v_limit;
        }
        else {
            vmRHV = true;
            vmRHH = true;
        }

        let vmRightHandOK = vmRHH && vmRHV
        let vmLeftHandOK = vmLHH && vmLHV

        // Setting the proper text.
        setEyeResultTextElement(leftHandResult,vmLeftHandOK)
        setEyeResultTextElement(rightHandResult,vmRightHandOK)

        leftHandCanvas.setHHandCalibValue(horizontal[1],h_limit);
    }

    function setEyeResultTextElement(textElement, validated){
        textElement.text = validated ? loader.getStringForKey("viewevaluation_success") : loader.getStringForKey("viewevaluation_failed");
        textElement.color = validated ? VMGlobals.vmBlackText : VMGlobals.vmRedError
    }


    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        flowControl.hideRenderWindow();
        visible = true
    }

    function close(){
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
        text: loader.getStringForKey("viewevaluation_hand_res")
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.horizontalCenter: dialog.horizontalCenter
    }

    Text {
        id: leftHandTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewevaluation_left_hand") + ":  "
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        x: leftHandCanvas.x
    }

    Text {
        id: leftHandResult
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontBaseSize
        anchors.verticalCenter: leftHandTitle.verticalCenter
        anchors.left: leftHandTitle.right
        visible: vmUseLeftHand
        onWidthChanged: {
            leftHandTitle.x = leftHandCanvas.x + (leftHandCanvas.width - leftHandResult.width - leftHandTitle.width)/2
        }
    }

    Text {
        id: rightHandTitle
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        text: loader.getStringForKey("viewevaluation_right_hand") + ":  "
        anchors.top: dialogTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        x: rightHandCanvas.x
    }

    Text {
        id: rightHandResult
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 600
        anchors.verticalCenter: rightHandTitle.verticalCenter
        anchors.left: rightHandTitle.right
        visible: vmUseLeftHand
        onWidthChanged: {
            rightHandTitle.x = rightHandCanvas.x + (rightHandCanvas.width - rightHandResult.width - rightHandTitle.width)/2
        }

    }

    /////////////////////////////////////////////////////////////////////

    Rectangle {
        id: halfScreenDivider
        width: VMGlobals.adjustWidth(1)
        radius: VMGlobals.adjustWidth(1)
        height: leftHandCanvas.height*0.9
        anchors.verticalCenter: leftHandCanvas.verticalCenter
        anchors.horizontalCenter: dialog.horizontalCenter
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    ///////////////////////////////////  LEFT HAND ELEMENTS //////////////////////////////////////

    Rectangle {
        id: leftHandCanvas
        width: vmHandAreaWidth
        height: vmHandAreaHeight
        radius: Math.max(0.02*vmHandAreaHeight,0.02*vmHandAreaWidth)
        border.width: 0
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.left: dialog.left;
        anchors.leftMargin: vmHorizotalMarginToHandArea
        anchors.top: leftHandTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)

        function setHHandCalibValue(value, hlimit){
            let zero = person.width + vmHPersonOffset
            let limit = vmHLimitDimension - horizontalHand.width
            let m = Math.abs(limit - zero)/hlimit; // We do absolute values because we don't care about sign.
            let b = zero;
            let hoffset = m*value + b;
            console.log("BEFORE: " + horizontalHand.x + " Should transfor to " + (vmHandAreaWidth - zero - hoffset))
            horizontalHand.x = vmHandAreaWidth - zero - hoffset
            console.log("AFTER: " + horizontalHand.x)
        }

        Canvas{

            id: horizontalLimit
            x: leftHandCanvas.width - vmHLimitDimension
            y: leftHandCanvas.height - height
            width:  0.01*leftHandCanvas.width
            height: vmVLimitDimension

            onPaint:{
                var ctx = getContext("2d");
                // I want N dashes and equal spacing. So we compute the value based on that.
                // let nDahses    = 10;
                // let dashLength = height/(nDahses*2)
                // console.log("height: " + height + " dash length " + dashLength)
                let dashLength = 3 // WARNING: Can't really explain the 3. But it looks ok. The math that I tried does not work and I don't understand why.
                ctx.setLineDash([dashLength, dashLength]);
                ctx.lineWidth = width
                ctx.beginPath();
                ctx.moveTo(0,0);
                ctx.lineTo(0,height);
                ctx.stroke();
            }
        }

        Canvas{

            id: verticalLineLimit
            x: leftHandCanvas.width - vmHLimitDimension
            y: leftHandCanvas.height - vmVLimitDimension
            width:  vmHLimitDimension
            height: horizontalLimit.width

            onPaint:{
                var ctx = getContext("2d");
                let dashLength = 3
                ctx.setLineDash([dashLength, dashLength]);
                ctx.lineWidth = height
                ctx.beginPath();
                ctx.moveTo(0,0);
                ctx.lineTo(width,0);
                ctx.stroke();
            }
        }

        Image {
            id: person
            width: vmHandAreaWidth*0.2
            fillMode: Image.PreserveAspectFit
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: vmHPersonOffset
            anchors.bottomMargin: vmVPersonOffset
            source: "qrc:/images/hand_calib_user.png"
        }

        Image {
            id: horizontalHand
            visible: vmUseLeftHand
            width: vmHandGraphWidth
            fillMode: Image.PreserveAspectFit
            //anchors.right: person.left
            anchors.verticalCenter: person.verticalCenter
            source: {
                if (vmLHH) return "qrc:/images/hand_left_green.png"
                else "qrc:/images/hand_left_red.png"
            }
        }

        Image {
            id: verticalHand
            visible: vmUseLeftHand
            width: vmHandGraphWidth
            fillMode: Image.PreserveAspectFit
            anchors.bottom: person.top
            anchors.horizontalCenter: person.horizontalCenter
            source: {
                if (vmLHV) return "qrc:/images/hand_left_green.png"
                else "qrc:/images/hand_left_red.png"
            }
        }


    }

    Rectangle {
        id: rightHandCanvas
        width: vmHandAreaWidth
        height: vmHandAreaHeight
        radius: Math.max(0.02*vmHandAreaHeight,0.02*vmHandAreaWidth)
        border.width: 0
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.right: dialog.right;
        anchors.rightMargin: vmHorizotalMarginToHandArea
        anchors.top: leftHandTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
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
        vmEnabled: vmRHH && vmRHV && vmLHV && vmLHH
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
            requestReCalibration(false)
        }
    }



}
