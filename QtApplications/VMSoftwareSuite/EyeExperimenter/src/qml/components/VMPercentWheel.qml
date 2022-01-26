import QtQuick
import "../"

Rectangle {

    id: rectHolder
    color: "transparent"
    //color: "#ff0000"

    property color vmOuterColor: "#000000"
    property color vmInnerColor: "#999999"
    property int vmPercent: 75


    function setDesiredCircleHeight(value){
        height = value
        width  = value
        canvas.height = value;
        canvas.width = value;
        //console.log("Rect Holder szie " + width + "x" + height + ". Canvas size: " + canvas.width + "x" + canvas.height)
    }

    onVmPercentChanged: {
        canvas.requestPaint()
    }

    onVmOuterColorChanged: {
        canvas.requestPaint()
    }

    onVmInnerColorChanged: {
        canvas.requestPaint()
    }

    Canvas {

        id: canvas
        contextType: "2d"

        // Only the height should be set.
        anchors.centerIn: parent

        function computeXY(R,angle){
            let x = R*Math.cos(angle) + R
            let y = R*Math.sin(angle) - R
            return [x,y]
        }

        onPaint: {

            var ctx = getContext("2d")

            ctx.reset();
            //ctx.fillStyle = "#ff0000"
            //ctx.fillRect(0,0,width,height)

            // This angle formula is because 0 is Pi/2 and 100 % is -3pi/2
            //console.log("Set percent is " + vmPercent);
            let angle = 2*Math.PI*vmPercent/100 - Math.PI/2

            let offset = 2;
            let R = width/2 - offset
            let cx = R + offset
            let cy = R + offset
            let r = 0.8*R

            //let pcenter = [R,R]
            let startAngle = -Math.PI/2
            let fullEndAngle = 3*Math.PI/2

            ctx.beginPath();
            ctx.fillStyle = vmOuterColor

            // Drawing the outer filled, incomplete circle
            ctx.arc(cx,cy,R,startAngle,angle,false)
            ctx.lineTo(cx,cy)
            ctx.closePath();
            ctx.fill();

            // Drawing the inner filler full circle
            ctx.beginPath();
            ctx.strokeStyle = vmOuterColor
            ctx.fillStyle = vmInnerColor
            ctx.arc(cx,cy,r,startAngle,fullEndAngle,false)
            ctx.closePath();
            ctx.stroke();
            ctx.fill();

            // Drawing the outer line circle.
            ctx.beginPath();
            ctx.strokeStyle = vmOuterColor
            ctx.arc(cx,cy,R,startAngle,fullEndAngle,false)
            //ctx.roundedRect(offset,offset,2*R,R*R,R,R)
            //ctx.closePath();
            ctx.stroke();

            // Drawing the actual percent
            ctx.beginPath()
            //mainWindow.vmSegoeBold.name
            let fontString = '700 %1px "%2"'.arg(VMGlobals.vmFontVeryLarge).arg(mainWindow.vmSegoeBold.name);
            //console.log("Using font " + fontString)
            ctx.font = fontString
            ctx.textBaseline = "middle"
            ctx.textAlign  = "center"
            ctx.fillStyle = vmOuterColor
            ctx.strokeStyle = vmOuterColor
            //console.log("Drawing text " + vmPercent);
            ctx.fillText(vmPercent,cx,cy)

        }

    }

}
