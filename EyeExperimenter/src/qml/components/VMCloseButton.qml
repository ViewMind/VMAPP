import QtQuick
import QtQuick.Controls
import "../"

Rectangle {
    id: vmCloseButton
    width: height
    height: VMGlobals.adjustHeight(32)
    border.width: 0
    color: {
        if (mouseArea.containsMouse){
            if (mouseArea.pressed) return VMGlobals.vmGrayButtonPressed
            else return VMGlobals.vmGrayHoverButton
        }
        else return "transparent"
    }

    signal closeSignal()

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            closeSignal()
        }
    }

    scale: mouseArea.pressed? 0.8 : 1;

    Behavior on scale{
        NumberAnimation {
            duration: 10
        }
    }

    Canvas {
        id: btnCloseCanvas
        width: height
        height: VMGlobals.adjustHeight(12.5)
        anchors.centerIn: parent
        contextType: "2d"
        onPaint: {

            var lw = VMGlobals.adjustHeight(1);


            var ctx = btnCloseCanvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = VMGlobals.vmBlackText
            ctx.lineWidth = lw
            ctx.lineCap = "round"

            ctx.moveTo(lw,lw);
            ctx.lineTo(width-lw,height-lw)
            ctx.moveTo(width-lw,lw)
            ctx.lineTo(lw,height-lw)

            ctx.stroke()
        }
    }
}
