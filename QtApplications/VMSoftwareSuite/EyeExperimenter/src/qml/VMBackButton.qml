import QtQuick 2.0
import QtQuick.Controls 2.3

Button {
    width: mainWindow.width*0.021
    height: mainWindow.height*0.029
    background: Rectangle {
        id: btnBackRect
        color: "#88B2D0"
        anchors.fill: parent
    }
    contentItem: Canvas {
        id: btnBackCanvas
        contextType: "2d"
        anchors.fill: parent
        onPaint: {
            var ctx = btnBackCanvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = "#ffffff";
            ctx.lineWidth = 2;
            ctx.lineCap = "round"
            ctx.moveTo(11,0);
            ctx.lineTo(1,10);
            ctx.lineTo(11,20);
            ctx.moveTo(0,10);
            ctx.lineTo(27,10);
            ctx.closePath();
            ctx.stroke();
        }
    }
}
