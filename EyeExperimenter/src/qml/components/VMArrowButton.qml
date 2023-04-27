import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property bool vmArrowPointsForward: true

    id: vmArrowButton

    height: VMGlobals.adjustHeight(22)
    width: height
    radius: VMGlobals.adjustHeight(2)
    border.width: VMGlobals.adjustHeight(1)
    border.color: VMGlobals.vmGrayUnselectedBorder

    color: {
        if (mouseArea.containsMouse){
            if (mouseArea.pressed) return VMGlobals.vmGrayButtonPressed
            else return VMGlobals.vmGrayHoverButton
        }
        else return "#ffffff"
    }

    signal arrowPressed()

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            arrowPressed()
        }
        onContainsMouseChanged: {
            btnArrowButton.requestPaint();
        }
    }

    scale: mouseArea.pressed? 0.8 : 1;

    Behavior on scale{
        NumberAnimation {
            duration: 10
        }
    }

    Canvas {
        id: btnArrowButton
        anchors.fill: parent
        contextType: "2d"
        onPaint: {

            let hmargin = width*0.2;   // 5% of the width is the air betwen the border of the button
            let vmargin = height*0.2;
            let xstart = hmargin;
            let xend   = width - hmargin
            let ystart = vmargin;
            let yend   = height - vmargin
            let ym     = height/2;

            var ctx = btnArrowButton.getContext("2d");
            ctx.reset();
            ctx.fillStyle = (mouseArea.containsMouse) ? VMGlobals.vmBlueButtonHighlight : VMGlobals.vmBlueSelected;
            ctx.lineCap = "round"

            if (vmArrowPointsForward){
                ctx.moveTo(xstart,ystart);
                ctx.lineTo(xend,ym);
                ctx.lineTo(xstart,yend);
            }
            else {
                ctx.moveTo(xend,ystart);
                ctx.lineTo(xstart,ym);
                ctx.lineTo(xend,yend);
            }

            ctx.closePath();
            ctx.fill();

        }
    }
}
