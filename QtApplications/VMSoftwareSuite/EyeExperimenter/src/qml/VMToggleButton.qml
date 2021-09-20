import QtQuick
import QtQuick.Controls

Rectangle {

    property string vmFont: "Mono"
    property bool vmOn: false
    property string vmText: ""

    id: toggleButton
    height: mainWindow.height*0.038
    width: mainWindow.width*0.117
    radius: 3
    color: vmOn ? "#3a8bd1" : "#ffffff"
    border.color: "#3a8bd1"
    border.width: 2;

    MouseArea {
        id: marea
        anchors.fill: parent
        onClicked: vmOn = !vmOn
    }

    Text {
        id: text
        text: vmText
        font.family: vmFont
        font.pixelSize: 13*viewHome.vmScale
        color: vmOn ? "#ffffff" : "58595b"
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
