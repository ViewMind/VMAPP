import QtQuick 2.6
import QtQuick.Controls 2.3

Button{

    id: vmFolderButton
    height: 40
    width:  4*height/3;

    scale: vmFolderButton.pressed? 0.8:1

    Behavior on scale{
        NumberAnimation {
            duration: 25
        }
    }

    background: Rectangle {
        id: rectArea
        radius: 6
        border.color: enabled? (vmFolderButton.pressed ? "#4984b3" : "#297FCA") : ("#bcbec0")
        color: enabled? (vmFolderButton.pressed ? "#4984b3" : "#297FCA") : "#bcbec0"
        anchors.fill: parent
    }
    contentItem: Item{
        id: folder
        anchors.centerIn: parent
        Rectangle{
            id: lowerRect
            height: vmFolderButton.height*0.44
            width: vmFolderButton.width*0.6
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: rectArea.color
            border.width: 1
            border.color: "#ffffff"
            radius: 1
        }
        Rectangle{
            id: horizontalStick
            height: vmFolderButton.height*0.22
            width: vmFolderButton.width*0.55
            anchors.bottom: lowerRect.top
            anchors.bottomMargin: -1
            anchors.left: lowerRect.left
            color: rectArea.color
            border.width: 1
            border.color: "#ffffff"
            radius: 1
        }
    }

}
