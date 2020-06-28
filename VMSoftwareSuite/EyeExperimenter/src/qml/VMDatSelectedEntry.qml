import QtQuick 2.0

Item {

    id: vmDatSelectedEntry

    Rectangle {
        id: dateRect
        color: "#ffffff"
        border.color: "#3096ef"
        border.width: mainWindow.width*0.0005
        anchors.fill: parent
        Text {
            font.family: viewHome.gothamR.name
            font.pixelSize: 12*viewHome.vmScale
            text: vmDatSelected
            color: "#000000"
            anchors.centerIn: parent
        }
    }

}
