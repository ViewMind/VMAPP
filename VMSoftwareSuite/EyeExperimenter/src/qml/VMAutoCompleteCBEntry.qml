import QtQuick 2.6

Rectangle {
//    property string vmText: ""
//    property string vmData: ""
//    property int vmIndex: -1

    signal clicked(string text, string data, int index);

    id: contentRect
    color: mouseDetect.containsMouse? "#cfcfcf" : "#ffffff"
    border.width: mainWindow.width*0.001
    border.color: "#cfcfcf"
    z: 10

    MouseArea {
        id: mouseDetect
        hoverEnabled: true
        anchors.fill: parent
        onClicked: contentRect.clicked(vmText,vmData,vmIndex);
    }

    Text {
        id: contentText
        text: vmText
        //color: (labelText.visible)? "#58595b" : "#cfcfcf"
        color: "#202020"
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        verticalAlignment: TextInput.AlignVCenter
        anchors.verticalCenter: contentRect.verticalCenter
        leftPadding: 5
    }

}
