import QtQuick
import QtQuick.Controls

CheckBox {

    property string vmTextColor: "#525252"

    id: control
    checked: false
    leftPadding: 0

    indicator: Rectangle {
        width: mainWindow.width*0.016
        height: mainWindow.height*0.029
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 4
        border.width: mainWindow.width*0.001
        border.color: "#297fca" //control.down ? "#17a81a" : "#21be2b"

        Rectangle {
            width: parent.width*0.8
            height: parent.height*0.8
            anchors.centerIn: parent
            radius: 2
            color: "#297fca" //control.down ? "#17a81a" : "#21be2b"
            visible: control.checked
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: vmTextColor //"#757575" //control.down ? "#757575" : "#21be2b"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
