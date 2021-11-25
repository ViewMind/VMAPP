import QtQuick
import QtQuick.Controls

Rectangle {

    property bool vmIsOn: false

    id: vmToggle
    height: Math.round(20*vmGlobals.vmControlsHeight/44)
    width: 2*height
    radius: height/2
    border.color:  vmIsOn ? vmGlobals.vmBlueSelected : vmGlobals.vmGrayAccented
    border.width: Math.round(height/40)
    color: vmIsOn ? vmGlobals.vmBlueSelected : vmGlobals.vmGrayToggleOff

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: vmIsOn = !vmIsOn
    }

    Rectangle {
        id: knob
        radius: height/2
        height: Math.round(12*vmToggle.height/20)
        width: height
        border.width: 0
        color: vmIsOn ? vmGlobals.vmWhite : vmGlobals.vmGrayPlaceholderText
        anchors.verticalCenter: parent.verticalCenter
        x: vmIsOn ? (vmToggle.width - height - (vmToggle.height-height)/2) : (vmToggle.height-height)/2
    }

}
