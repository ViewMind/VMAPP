import QtQuick 2.0

Item {
    property string vmFont: "Mono"

    Canvas {
        id: lineEditCanvas
        contextType: "2d"
        anchors.fill: parent
        onPaint: {
            var ctx = lineEditCanvas.getContext("2d");
            ctx.reset();
            ctx.strokeStyle = "#297FCA";
            ctx.lineWidth = 2;
            ctx.lineCap = "round"
            //console.log("w and h are " + width + " and " + height);
            ctx.moveTo(0,height);
            ctx.lineTo(width,height);
            ctx.closePath();
            ctx.stroke();
        }
    }

    TextInput {
        id: lineEdit
        color: "#757575"
        font.family: vmFont
        font.pixelSize: 13
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
    }
}
