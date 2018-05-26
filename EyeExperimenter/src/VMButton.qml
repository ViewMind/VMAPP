import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    property string vmText: "VM Button"
    property string vmFont: "Mono"
    property variant vmSize: [131,50]

    id: vmButton
    background: Rectangle {
        id: rectArea
        radius: 3
        color: enabled? "#297fca" : "#A1D1F9"
        width: vmSize[0]
        height: vmSize[1]
    }
    contentItem: Text{
        anchors.centerIn: rectArea
        font.family: vmFont
        font.pixelSize: 13
        text: vmText
        color: "#ffffff"
    }
}
