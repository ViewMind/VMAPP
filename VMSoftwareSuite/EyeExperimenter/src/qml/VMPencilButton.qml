import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmPlusButton
    height: 40
    width:  4*height/3;

    scale: vmPlusButton.pressed? 0.8:1

    property double vmStickWidth: width*0.08
    property double vmStickLength: height*0.5

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    background: Rectangle {
        id: rectArea
        radius: 6
        border.color: enabled? (vmPlusButton.pressed ? "#4984b3" : "#297FCA") : ("#bcbec0")
        color: enabled? (vmPlusButton.pressed ? "#4984b3" : "#297FCA") : "#bcbec0"
        anchors.fill: parent
    }
    contentItem: Item{
        id: plus
        anchors.centerIn: parent
        Image {
            id: headDesign
            source: "qrc:/images/edit_icon.png"
            anchors.centerIn: parent
        }
    }
}
