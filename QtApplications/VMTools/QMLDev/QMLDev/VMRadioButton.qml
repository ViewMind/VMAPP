import QtQuick
import QtQuick.Controls

Rectangle {

    property bool vmIsOn: false

    id: vmRadioButton
    height: Math.round(20*vmGlobals.vmControlsHeight/44)
    width: height
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
        id: circle
        color: vmGlobals.vmWhite
        border.width: 0
        height: Math.round(12*vmRadioButton.height/20)
        width: height
        radius: height/2
        visible: vmIsOn
        anchors.centerIn: parent
    }

}
