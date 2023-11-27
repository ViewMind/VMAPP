import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property bool vmArrowPointsForward: true
    readonly property real vmWidthAdjust: 1.5

    id: vmArrowButton

    height: VMGlobals.adjustHeight(22)
    width: height*vmWidthAdjust
    radius: VMGlobals.adjustHeight(2)
    border.width: VMGlobals.adjustHeight(1)
    border.color: VMGlobals.vmGrayUnselectedBorder

    color: {
        if (mouseArea.containsMouse){
            if (mouseArea.pressed) return VMGlobals.vmGrayButtonPressed
            else return VMGlobals.vmGrayHoverButton
        }
        else return "#ffffff"
    }

    signal arrowPressed()

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            arrowPressed()
        }
        onContainsMouseChanged: {
            btnArrowButton.requestPaint();
        }
    }

    scale: mouseArea.pressed? 0.8 : 1;

    Behavior on scale{
        NumberAnimation {
            duration: 10
        }
    }

    Canvas {
        id: btnArrowButton
        anchors.fill: parent
        contextType: "2d"
        onPaint: {

            let w = width/vmWidthAdjust;
            let hmargin = w*0.2;   // 5% of the width is the air betwen the border of the button
            let vmargin = height*0.2;            
            let xstart = hmargin;
            let arrowHeadWidth = w - 2*hmargin;
            let forwardXStart = width - hmargin - arrowHeadWidth;

            let xend   = w - hmargin
            let ystart = vmargin;
            let yend   = height - vmargin
            let ym     = height/2;

            if (vmArrowPointsForward){
                let offset = forwardXStart - xstart;
                xstart = forwardXStart;
                xend   = xend + offset;
            }

            var ctx = btnArrowButton.getContext("2d");
            ctx.reset();


            ctx.beginPath()
            ctx.fillStyle = (mouseArea.containsMouse) ? VMGlobals.vmBlueButtonHighlight : VMGlobals.vmBlueSelected;
            ctx.strokeStyle = ctx.fillStyle;
            ctx.lineCap = "round"

            if (vmArrowPointsForward){
                ctx.moveTo(xstart,ystart);
                ctx.lineTo(xend,ym);
                ctx.lineTo(xstart,yend);
            }
            else {
                ctx.moveTo(xend,ystart);
                ctx.lineTo(xstart,ym);
                ctx.lineTo(xend,yend);
            }

            ctx.closePath();
            ctx.stroke();
            ctx.fill();

            // Up until this point we've drawn the arrow "head". Now we add the arrow body.
            let bodyHeight = 0.4*height;
            let bodyWidth  = width - 2*hmargin - arrowHeadWidth;
            let ypos       = height/2 - bodyHeight/2;
            let xpos       = 0;
            if (vmArrowPointsForward){
                xpos = hmargin;
            }
            else {
                xpos = hmargin + arrowHeadWidth*0.9;
                //xpos = hmargin
            }
            ctx.rect(xpos,ypos,bodyWidth,bodyHeight);
            ctx.fill();

        }
    }
}
