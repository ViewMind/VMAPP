import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property bool vmCheckIsVisble: true

    radius: width/2
    height: width
    border.width: vmCheckIsVisble? 0 : 0.05*width
    border.color: VMGlobals.vmGrayPlaceholderText
    color: vmCheckIsVisble? VMGlobals.vmBlueSelected : "transparent";

    onVmCheckIsVisbleChanged: {
        canvas.requestPaint()
    }


    Canvas {

        id: canvas
        contextType: "2d"

        // Only the height should be set.
        anchors.fill: parent

        onPaint: {

            if (!vmCheckIsVisble) return;

            var ctx = getContext("2d")
            ctx.reset();

            let xs = 0.3*width
            let xc = width/2
            let xe = 0.7*width

            let ys = height*0.6
            let yb = height*0.7
            let ye = height*0.3

            ctx.strokeStyle = VMGlobals.vmGrayToggleOff
            ctx.lineWidth = width*0.08
            ctx.lineCap = "round"

            // This draws the checkmark.
            //ctx.beginPath()
            ctx.moveTo(xs,ys)
            ctx.lineTo(xc,yb)
            ctx.lineTo(xe,ye)
            ctx.stroke();
            //ctx.closePath()



        }
    }
}
