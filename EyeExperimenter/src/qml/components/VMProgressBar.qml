import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: progressBar

    property double vmPercent: 25;
    property string vmMessage: "";
    property double vmPBarHeight: VMGlobals.adjustHeight(50)
    property int vmFontSize: VMGlobals.vmFontLarger

    property double margin: 0 //canvas.height/2

    height: percentText.height + vmPBarHeight

    Text {
        id: percentText
        text: Math.round(vmPercent) + " %"
        font.pixelSize: vmFontSize
        font.weight: 400
        color: VMGlobals.vmBlackText
        anchors.bottom: canvas.top
        anchors.right: parent.right
        anchors.rightMargin: margin
        verticalAlignment: Text.AlignVCenter
    }

    Text {
        id: messageText
        text: vmMessage
        font.pixelSize: vmFontSize
        font.weight: 400
        color: VMGlobals.vmBlackText
        anchors.bottom: canvas.top
        anchors.left: parent.left
        anchors.leftMargin: margin
        verticalAlignment: Text.AlignVCenter
    }

    onVmPercentChanged: {
        canvas.requestPaint();
    }

    Canvas {

        id: canvas
        contextType: "2d"

        // The canvas is the actual progress bar which should be a the botton of the container.
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        //height: parent.height - percentText.height
        height: vmPBarHeight

        onPaint: {

            var ctx = getContext("2d")

            let containerColor = VMGlobals.vmGrayAccented;
            let fillColor = VMGlobals.vmBlueSelected
            let lw = 0
            let R = height/2

            ctx.reset();

            // Draws the container for the progress bar.
            ctx.beginPath()
            ctx.fillStyle = containerColor;
            ctx.lineWidth = lw
            ctx.roundedRect(0,0,width,height,R,R)
            ctx.fill();
            ctx.closePath()

            // Draws the actual progress.
            let w = width*vmPercent/100
            if (w > width) w = width

            if (w <= 2*R){
                let a = w/2
                semiCircleIndicator(ctx,a,R,0,0,fillColor,false)
                semiCircleIndicator(ctx,a,R,-2*R+2*a,0,fillColor,true)
            }
            else {
                ctx.beginPath()
                ctx.fillStyle = fillColor;
                ctx.lineWidth = lw
                ctx.roundedRect(0,0,w,height,R,R)
                ctx.fill();
                ctx.closePath()
            }
        }
    }

    function semiCircleIndicator(ctx, a, r, varstart_x, varstart_y, fillColor, isRight){

        // We now compute the parameters fro drawing the proper arc of the circle given how much it's covered (a). This is for the left side.
        let c = r - a;
        let d = Math.sqrt(r*r - c*c);
        let alpha = Math.atan(d/c);
        let beta = Math.PI/2 - alpha;
        ctx.beginPath();
        ctx.strokeStyle = fillColor;
        ctx.fillStyle = fillColor;

        if (isRight){
            // Now we do the same for the right hand side.
            let x = varstart_x + 2*r - a
            let y = varstart_y + (r - a)
            ctx.moveTo(x,y);
            ctx.arc(varstart_x+r,varstart_y+r,r,Math.PI/2 - beta,3*Math.PI/2 + beta,true);
        }
        else {
            // Left hand plug.
            let x = varstart_x + a;
            let y = varstart_y + r + d;
            ctx.moveTo(x,y);
            ctx.arc(varstart_x+r,varstart_y+r,r,Math.PI/2 + beta,3*Math.PI/2 - beta,false);
        }

        ctx.fill();
        ctx.closePath();

        if (!isRight){
            // we add a line to close the gap. when joined with whatever is on the left hand side.
            ctx.lineWidth = 1;
            ctx.strokeStyle = fillColor;
            ctx.beginPath();
            ctx.moveTo(varstart_x+a,varstart_y+r-d)
            ctx.lineTo(varstart_x+a,varstart_y+r+d)
            ctx.stroke();
            ctx.closePath();
        }

    }


}

