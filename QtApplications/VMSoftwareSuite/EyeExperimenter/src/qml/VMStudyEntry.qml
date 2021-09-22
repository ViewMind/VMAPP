import QtQuick

Item {
    id: vmStudyEntry

    signal itemSelected(int vmIndex);

    MouseArea {
        anchors.fill: parent
        onClicked: {
            vmStudyEntry.itemSelected(vmIndex);
        }
    }
    Rectangle {
        id: dateRect
        color: vmIsSelected? "#3096ef" : "#ffffff"
        border.color: vmIsSelected? "#144673" : "#3096ef"
        radius: mainWindow.width*0.005
        border.width: mainWindow.width*0.0005
        anchors.fill: parent
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: 12*viewHome.vmScale
            text: vmStudyName
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.centerIn: parent
        }
    }

}
