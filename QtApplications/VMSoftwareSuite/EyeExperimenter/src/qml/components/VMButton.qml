import QtQuick
import QtQuick.Controls
import "../"

/**
  *  VM Button can be configured in the following maner
  *  1) With or withouth an icon. The Icon can be left or right. This is set with the icon source.
  *     However if the icon source is "plus" or "arrow" a canvas drawn plus or arrow is used instead.
  *  2) Fixed width or computed width.
  *  3) Four button types: Primary, Secondary, Tertiary and Warning.
  *  4) With or withouth text.
  *  5) Can be disabled.
  *  6) 3  Types of predefined icons or select image as icon.
  *  7) Icons can be configured to appear to the right or to the left of the text in the button.
  **/

Rectangle {

    readonly property int vmTypePrimary:   0
    readonly property int vmTypeSecondary: 1
    readonly property int vmTypeTertiary:  2
    readonly property int vmTypeWarning:   3

    readonly property double vmSpacing: VMGlobals.adjustWidth(10);
    readonly property double vmSideMargins: VMGlobals.adjustWidth(20)

    // Configuration option
    property string vmText: ""
    property bool vmEnabled: true
    property double vmCustomWidth: 0
    property int vmButtonType: vmTypePrimary
    property string vmIconSource: ""
    property bool vmIconToTheRight: false; // If icon source is provided this tells it to go the right or left of the text.
    property bool vmThinButton: false

    // Should NOT be set by the instation.
    property bool vmShowIcon: false
    property bool vmShowBackArrow: false;
    property bool vmShowPlus: false;
    property bool vmShowChevronSide: false


    id: vmPrimaryButton
    height: vmThinButton ? VMGlobals.adjustHeight(36) : VMGlobals.vmControlsHeight

    property color vmHoverColor: VMGlobals.vmBlueButtonHighlight
    property color vmNormalColor: VMGlobals.vmBlueSelected
    property color vmPressedColor: VMGlobals.vmBlueButtonPressed
    property color vmDisabledColor: VMGlobals.vmGrayUnselectedBorder
    property color vmTextColor: VMGlobals.vmWhite
    property color vmTextColorDisabled: VMGlobals.vmWhite
    property color vmBorderColor: VMGlobals.vmBlueBackgroundHighlight
    property color vmBorderColorDisabled: VMGlobals.vmGrayUnselectedBorder
    property color vmBorderColorHover: VMGlobals.vmBlueBackgroundHighlight

    /////////// SETUP FUNCTIONS

    onVmButtonTypeChanged: {
        vmHoverColor = VMGlobals.vmBlueButtonHighlight
        vmPressedColor = VMGlobals.vmBlueButtonPressed
        vmTextColor = VMGlobals.vmWhite
        vmTextColorDisabled = VMGlobals.vmWhite
        vmNormalColor = VMGlobals.vmBlueSelected
        vmBorderColor = vmNormalColor
        vmDisabledColor = VMGlobals.vmGrayUnselectedBorder
        vmBorderColorDisabled = vmDisabledColor
        vmBorderColorHover = vmHoverColor

        if (vmButtonType == vmTypeSecondary){
            vmHoverColor = VMGlobals.vmWhiteButtonHiglight
            vmNormalColor = VMGlobals.vmWhite
            vmPressedColor = VMGlobals.vmGrayUnselectedBorder
            vmTextColor = VMGlobals.vmBlueSelected
            vmTextColorDisabled = VMGlobals.vmGrayTextButtonDisabled
            vmDisabledColor = VMGlobals.vmWhite
            vmBorderColor =  VMGlobals.vmGrayUnselectedBorder
            vmBorderColorDisabled = VMGlobals.vmGrayButtonPressed
            vmBorderColorHover = vmBorderColor
        }
        else if (vmButtonType == vmTypeTertiary){
            vmHoverColor = VMGlobals.vmGrayHoverButton
            vmNormalColor = "transparent"
            vmPressedColor = VMGlobals.vmGrayButtonPressed
            vmTextColor = VMGlobals.vmBlackText
            vmTextColorDisabled = VMGlobals.vmBlackText
            vmBorderColor = "transparent"
            vmBorderColorDisabled = vmBorderColor
            vmDisabledColor = vmNormalColor
            vmBorderColorHover = vmHoverColor
        }
        else if (vmButtonType == vmTypeWarning){
            vmNormalColor = VMGlobals.vmRedError
            vmHoverColor = Qt.lighter(vmNormalColor,1.5)            
            vmPressedColor = Qt.darker(vmNormalColor,1.5)
            vmTextColor = VMGlobals.vmWhite
            vmTextColorDisabled = VMGlobals.vmWhite
            vmBorderColor = vmNormalColor
            vmBorderColorDisabled = vmBorderColor
            vmDisabledColor = vmNormalColor
            vmBorderColorHover = vmHoverColor
        }
    }

    onVmIconSourceChanged: {
        vmShowBackArrow = false;
        vmShowPlus = false;
        vmShowIcon = false;
        vmShowChevronSide = false;
        if (vmIconSource === "plus"){
            vmShowPlus = true;
        }
        else if (vmIconSource === "arrow"){
            vmShowBackArrow = true;
        }
        else if (vmIconSource == "next"){
            vmShowChevronSide = true;
        }
        else if (vmIconSource !== ""){
            vmShowIcon = true;
        }
    }

    function imagePositioning(w){
        if (vmText == ""){
            return (vmPrimaryButton.width - w)/2
        }
        else {
            if (vmIconToTheRight){
                return buttonText.x + buttonText.width + vmSpacing
            }
            else{
                //console.log()
                return buttonText.x - vmSpacing - w
            }
        }
    }

    signal clickSignal();

    ////////////// BASIC SETUP
    //width: (vmCustomWidth == 0) ?  (vmSideMargins*2 + metrics.width) : vmCustomWidth
    width: {
        // console.log("Computing the width for " + vmText)
        if (vmCustomWidth != 0) return vmCustomWidth
        var w = vmSideMargins*2;
        var sumSpacing = 0;
        if (vmText != ""){
            w = w + metrics.width;
            sumSpacing++
        }
        if (vmShowIcon){
            w = w + icon.width
            sumSpacing++
        }
        else if (vmShowPlus || vmShowBackArrow || vmShowChevronSide){
            w = w + canvas.width
            sumSpacing++
        }
        if (sumSpacing >= 2) w = w + vmSpacing
        return w
    }
    color: {
        if (vmEnabled){
            if (mouseArea.containsMouse){
                if (mouseArea.pressed) return vmPressedColor;
                else return vmHoverColor
            }
            else return vmNormalColor
        }
        else return vmDisabledColor
    }
    border.width: VMGlobals.adjustHeight(1)
    border.color: {
        if (vmEnabled){
            if (mouseArea.containsMouse) return vmBorderColorHover
            else return vmBorderColor
        }
        else return vmBorderColorDisabled
    }
    radius: Math.round(vmPrimaryButton.height*8/44)

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: clickSignal()
        enabled: vmEnabled
        onPressed: {
            vmPrimaryButton.forceActiveFocus()
        }
    }



    scale: mouseArea.pressed ? 0.8 : 1

    Behavior on scale{
        NumberAnimation {
            duration: 10
        }
    }

    Text {
        id:     buttonText
        text:   vmText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        color: vmEnabled ? vmTextColor : vmTextColorDisabled
        anchors.verticalCenter: parent.verticalCenter
        height: VMGlobals.adjustHeight(18)
        verticalAlignment: Text.AlignVCenter
        visible: (vmText != "")
        x : {
            if (vmShowBackArrow || vmShowIcon || vmShowPlus || vmShowChevronSide){
                if (vmIconToTheRight){
                    return vmSideMargins
                }
                else{
                    vmPrimaryButton.width - vmSideMargins - buttonText.width
                }
            }
            else {
                return (vmPrimaryButton.width - buttonText.width)/2
            }
        }
    }

    Image {
        id: icon
        source: {
            if ((vmIconSource == "plus") || (vmIconSource == "arrow") || (vmIconSource == "next")) return ""
            else return vmIconSource
        }
        height: VMGlobals.adjustHeight(16)
        fillMode: Image.PreserveAspectFit
        visible: vmShowIcon
        anchors.verticalCenter: parent.verticalCenter
        x : imagePositioning(icon.width)
    }

    Canvas {
        id: canvas
        contextType: "2d"
        height: VMGlobals.adjustHeight(16)
        width: {
            if (vmShowPlus) return height
            else if (vmShowBackArrow) height*12/11;
            else height*5.62/10.12
        }
        anchors.verticalCenter: parent.verticalCenter
        x : imagePositioning(canvas.width)
        visible: vmShowPlus || vmShowBackArrow || vmShowChevronSide
        onPaint: {

            // Using round line caps extends the line past the point of finish by 1 measure of line with.
            // Hence we need to compute the midpoints.
            var lw = VMGlobals.adjustHeight(1.5)
            var midw = (width-2*lw)/2 +lw
            var midh = (height-2*lw)/2 +lw
            var fw   = (width-2*lw) + lw
            var fh   = (height-2*lw) + lw


            var ctx = canvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = buttonText.color
            ctx.lineWidth = lw
            ctx.lineCap = "round"
            ctx.lineJoin = "round"


            if (vmShowBackArrow){
                ctx.moveTo(midw,lw)
                ctx.lineTo(lw,midh)
                ctx.lineTo(midw,fh)
                ctx.moveTo(lw,midh)
                ctx.lineTo(fw,midh)
            }
            else if (vmShowPlus){
                ctx.moveTo(midw,lw)
                ctx.lineTo(midw,fh)                
                ctx.moveTo(lw,midh)
                ctx.lineTo(fw,midh)
            }
            else if (vmShowChevronSide){
                if (vmIconToTheRight){
                    ctx.moveTo(lw,lw)
                    ctx.lineTo(fw,midh)
                    ctx.lineTo(lw,fh)
                }
                else{
                    ctx.moveTo(fw,lw)
                    ctx.lineTo(lw,midh)
                    ctx.lineTo(fw,fh)
                }
            }
            ctx.stroke();

        }
    }


    TextMetrics {
        id:     metrics
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        text:   vmText
    }

}
