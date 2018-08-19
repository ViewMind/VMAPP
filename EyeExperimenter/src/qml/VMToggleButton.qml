import QtQuick 2.6
import QtQuick.Controls 2.3

Rectangle {

    property string vmFont: "Mono"
    property bool vmOn: false
    property string vmText: ""

    id: toggleButton
    height: 26
    width: 120
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
        font.pixelSize: 13
        color: vmOn ? "#ffffff" : "58595b"
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
