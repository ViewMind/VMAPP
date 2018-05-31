import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmButton
    property string vmText: "VM Button"
    property string vmFont: "Mono"
    property variant vmSize: [131,50]
    property bool vmInvertColors: false

    width: vmSize[0]
    height: vmSize[1]

    background: Rectangle {
        id: rectArea
        radius: 3
        color: enabled? (vmInvertColors? "#ffffff" : "#297fca") : "#A1D1F9"
        border.color: "#A1D1F9"
        anchors.fill: parent
    }
    contentItem: Text{
        anchors.centerIn: parent
        font.family: vmFont
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: vmText
        color: vmInvertColors? "#297fca" : "#ffffff"
    }
}
