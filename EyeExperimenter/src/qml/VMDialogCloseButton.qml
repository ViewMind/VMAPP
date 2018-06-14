import QtQuick 2.0
import QtQuick.Controls 2.3

Button {
    id: vmCloseButton
    width: 20
    height: 20
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
            ctx.lineTo(20,20);
            ctx.moveTo(20,0);
            ctx.lineTo(0,20);
            ctx.closePath();
            ctx.stroke();
        }
    }
}
