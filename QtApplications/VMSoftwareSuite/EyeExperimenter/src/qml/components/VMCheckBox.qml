import QtQuick
import QtQuick.Controls

Rectangle {

    property bool vmIsChecked: false

    id: vmCheckBox
    height: Math.round(20*vmGlobals.vmControlsHeight/44)
    width: height
    radius:Math.round(3*height/20)
    border.color:  vmIsChecked ? vmGlobals.vmBlueSelected : vmGlobals.vmGrayAccented
    border.width: Math.round(height/40)
    color: vmIsChecked ? vmGlobals.vmBlueSelected : vmGlobals.vmGrayToggleOff

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: vmIsChecked = !vmIsChecked
    }

    Canvas {
        id: checkMark
        contextType: "2d"
        width: vmCheckBox.width/2
        height: Math.round(7*vmCheckBox.height/20)
        visible: vmIsChecked
        anchors.centerIn: parent

        onPaint: {
            var ctx = checkMark.getContext("2d");
            ctx.beginPath();
            ctx.strokeStyle = vmGlobals.vmWhite;
            ctx.lineWidth = 2;
            ctx.moveTo(0, height*0.57);
            ctx.lineTo(width*0.4, height);
            ctx.lineTo(width, 0);
            ctx.stroke()
        }


    }

}
