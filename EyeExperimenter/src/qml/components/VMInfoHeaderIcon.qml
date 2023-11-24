import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property string vmToolTip: "Let's try a couple of lines\nof text so that we can see\nwhat that looks like";

    id: infoIcon
    width: height
    height: 20
    color: "transparent"

    signal triggeredSignal()

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
        }
        onContainsMouseChanged: {
            if (containsMouse){
                console.log("Tootip time!!!");
            }
        }
    }

    Rectangle {
        id: tooltip
        color: VMGlobals.vmGrayTooltipBackground
        border.width: VMGlobals.adjustHeight(1)
        border.color: VMGlobals.vmGrayAccented
        radius: VMGlobals.adjustHeight(3)
        height: text.height*1.06
        width: text.width*1.06
        anchors.bottom: infoIconCanvas.top
        anchors.right: infoIconCanvas.right
        z: 100
        visible: mouseArea.containsMouse

        Text {
            id: text
            text: vmToolTip
            font.pixelSize: VMGlobals.vmFontSmall
            font.weight: 400
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.centerIn: parent
        }

    }

    Canvas {
        id: infoIconCanvas
        anchors.fill: parent
        contextType: "2d"
        onPaint: {

            var lw = 0.1*width
            var space = 0.1*height
            var D = 0.16*width;
            var R = D/2
            var ctx = infoIconCanvas.getContext("2d");

            let color = VMGlobals.vmBlueSelected;
            //color = "#ff0000"

            ctx.reset();
            ctx.strokeStyle = color
            ctx.fillStyle   = color
            ctx.lineWidth   = lw;
            ctx.lineCap     = "round"

            // First we draw the cirle of the icon
            ctx.beginPath()
            ctx.ellipse(lw/2,lw/2,width-lw,height-lw);
            ctx.stroke();

            // I beam of the letter i.
            ctx.beginPath()
            let xcenter = width/2;
            let ybottom = height - 2*lw;
            let barh    = height*0.33;
            let ytop    = ybottom - barh;

            ctx.moveTo(xcenter,ytop);
            ctx.lineTo(xcenter,ybottom);
            ctx.stroke();

            // The Dot of the letter y.
            let ycenter = ytop - R - space
            let x = xcenter - R;
            let y = ycenter - R;
            ctx.beginPath()
            ctx.ellipse(x,y,D,D);
            ctx.fill();



        }
    }
}
