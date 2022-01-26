import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmRadioButton

    property string vmText: ""
    property bool vmIsOn: false

    readonly property double vmAirBeforeText: VMGlobals.adjustWidth(8)

    width: outerCircle.width + text.width + vmAirBeforeText
    height: VMGlobals.adjustHeight(20)

    Rectangle {

        id: outerCircle
        height: parent.height
        width: height
        radius: height/2
        border.color:  vmIsOn ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayAccented
        border.width: VMGlobals.adjustHeight(1)
        color: vmIsOn ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayToggleOff

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: vmIsOn = !vmIsOn
        }

        Rectangle {
            id: circle
            color: VMGlobals.vmWhite
            border.width: 0
            height: VMGlobals.adjustHeight(12)
            width: height
            radius: height/2
            visible: vmIsOn
            anchors.centerIn: parent
        }

    }

    Text {
        id: text
        text: vmText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        height: parent.height
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: outerCircle.verticalCenter
        anchors.left: outerCircle.right
        anchors.leftMargin: vmAirBeforeText
    }


}
