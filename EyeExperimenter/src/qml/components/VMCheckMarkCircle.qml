import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    readonly property int vmCHECK_DONE:     0;
    readonly property int vmCHECK_NOT_DONE: 1;
    readonly property int vmCHECK_CURRENT:  2;

    property int vmCheckState: vmCHECK_NOT_DONE

    radius: width/2
    height: width
    border.width: (vmCheckState != vmCHECK_CURRENT)? 0.05*width : 0
    border.color: VMGlobals.vmGrayPlaceholderText
    color: (vmCheckState === vmCHECK_CURRENT)? VMGlobals.vmBlueSelected : "transparent";

    onVmCheckStateChanged: {
        canvas.requestPaint()
    }


    Canvas {

        id: canvas
        contextType: "2d"

        // Only the height should be set.
        anchors.fill: parent

        onPaint: {

            let color = VMGlobals.vmGrayToggleOff
            if (vmCheckState == vmCHECK_DONE){
                color = VMGlobals.vmGrayPlaceholderText
            }
            else if (vmCheckState == vmCHECK_NOT_DONE) return;

            var ctx = getContext("2d")
            ctx.reset();

            let xs = 0.3*width
            let xc = width/2
            let xe = 0.7*width

            let ys = height*0.6
            let yb = height*0.7
            let ye = height*0.3

            ctx.strokeStyle = color
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
