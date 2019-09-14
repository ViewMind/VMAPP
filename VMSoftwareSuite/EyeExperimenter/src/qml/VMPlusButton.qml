import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmPlusButton
    height: 40
    width:  4*height/3;

    scale: vmPlusButton.pressed? 0.8:1

    property double vmStickWidth: width*0.08
    property double vmStickLength: height*0.5
    property bool vmMakeDeleteButton: false

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    background: Rectangle {
        id: rectArea
        radius: 6
        border.color: vmPlusButton.pressed ? "#4984b3" : "#297FCA"
        color: vmPlusButton.pressed ? "#4984b3" : "#297FCA"
        anchors.fill: parent
    }
    contentItem: Item{
        id: plus
        anchors.centerIn: parent
        Rectangle{
            id: verticalStick
            height: vmStickLength
            width: vmStickWidth
            color: "#ffffff"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: horizontalStick.horizontalCenter
            border.width: 0
            rotation: vmMakeDeleteButton? 45: 0
        }
        Rectangle{
            id: horizontalStick
            height: vmStickWidth
            width: vmStickLength
            color: "#ffffff"
            anchors.centerIn: parent
            border.width: 0
            rotation: vmMakeDeleteButton? 45: 0
        }
    }
}
