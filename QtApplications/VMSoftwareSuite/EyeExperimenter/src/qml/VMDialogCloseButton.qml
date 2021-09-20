import QtQuick
import QtQuick.Controls

Button {
    id: vmCloseButton
    width: mainWindow.width*0.009
    height: mainWindow.height*0.018
    hoverEnabled: false
    background: Rectangle {
        id: btnCloseRect
        color: "#ffffff"
        anchors.fill: parent
    }

    onPressed:{
        btnCloseCanvas.requestPaint();
    }

    scale: pressed? 0.8:1;

    Behavior on scale{
        NumberAnimation {
            duration: 10
        }
    }

    contentItem: Canvas {
        id: btnCloseCanvas
        contextType: "2d"
        anchors.fill: parent
        onPaint: {
            var ctx = btnCloseCanvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = pressed? "#58595b": "#757575";
            ctx.lineWidth = 2;
            ctx.lineCap = "round"
            ctx.moveTo(0,0);
            ctx.lineTo(vmCloseButton.width,vmCloseButton.height);
            ctx.moveTo(vmCloseButton.width,0);
            ctx.lineTo(0,vmCloseButton.height);
            ctx.closePath();
            ctx.stroke();
        }
    }
}
