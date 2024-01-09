import QtQuick
import QtQuick.Controls

Window {

    id: dialogWindowForAnotherApplicationRunning
    width: message.width + 50
    height: message.height + langSelect.height + 70 + message.anchors.topMargin + whereIsTheTaskBar.height + whereIsTheTaskBar.anchors.topMargin
    visible: true
    title: "Another Instance of ViewMind Atlas Is Running"

    property var vmTitles: []
    property var vmMessages: []

    Text {
        id: message
        text: "You can either close the existing application and restart it\nor click on the taskbar icon"
        font.pixelSize: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30
    }

    Image {
        id: whereIsTheTaskBar
        source: "qrc:/images/where_is_taskbar.png"
        width: parent.width - 20
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: message.bottom
        anchors.topMargin: 20
    }

    ComboBox {
        id: langSelect
        editable: false
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onCurrentTextChanged: {
            //console.log("Current Text is: " + currentText)
            message.text = vmMessages[currentText]
            dialogWindowForAnotherApplicationRunning.title = vmTitles[currentText];
        }
    }

    Component.onCompleted : {

        // We need to load the texts of the application.

        vmTitles = loader.getTitleMap();
        vmMessages = loader.getMessageMap();

        langSelect.model = Object.keys(vmTitles)

    }
}
