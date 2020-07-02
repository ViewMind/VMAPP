import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmArrowButton
    scale: vmArrowButton.pressed? 0.8:1

    property bool vmRotate: false

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    background: Rectangle {
        id: rectArea
        radius: width*0.15
        border.color: "#ffffff"
        color: enabled?( pressed ? "#4984b3" : "#297fca") : "#eaeaea"
        anchors.fill: parent
    }
    contentItem: Image{
        anchors.centerIn: parent
        source: "qrc:/images/white_arrow.png"
        scale: 0.6
        transformOrigin: Item.Center
        rotation: vmRotate? 0 : 180
    }
}
