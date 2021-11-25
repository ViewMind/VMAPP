import QtQuick
import QtQuick.Controls

Rectangle {

    property string vmText: ""
    property bool vmEnabled: true

    id: vmPrimaryButton
    height: vmGlobals.vmControlsHeight

    signal clicked();

    width: Math.round(metrics.tightBoundingRect.width*88/42)
    color: mouseArea.containsMouse? vmGlobals.vmBlueButtonHighlight : vmGlobals.vmBlueTextHighlight
    border.width: 0
    radius: Math.round(vmPrimaryButton.height*8/44)

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: clicked()
        enabled: vmEnabled
    }

    scale: mouseArea.pressed ? 0.8 : 1

    Text {
        id:     buttonText
        text:   vmText
        font.pixelSize: vmGlobals.vmFontLarge
        font.weight: 600
        color: vmEnabled? vmGlobals.vmWhite : vmGlobals.vmWhite50Translucent
        anchors.verticalCenter: parent.verticalCenter
        x: (vmPrimaryButton.width - width)/2
    }

    TextMetrics {
        id:     metrics
        font.pixelSize: vmGlobals.vmFontLarge
        font.weight: 600
        text:   vmText
    }

}
