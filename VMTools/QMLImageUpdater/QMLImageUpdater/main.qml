import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import com.qml 1.0

Window {
    visible: true
    width: 1280
    height: 720
    title: qsTr("QML Image Updater")

    Connections{
        target: control
        onNewImageAvailable: {
            imageDisplay.image = control.image;
        }
    }

    Column {

        id: layout
        anchors.fill: parent
        spacing: parent.height*0.05

        property alias image: imageDisplay;

        QImageDisplay{
            id: imageDisplay
            width: parent.width*0.9
            height: parent.height*0.8
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button{
            id: testButton
            text: "Test stuff"
            width: parent.width
            height: parent.height*0.1
            onClicked: {
                control.goToNextImage();
            }
        }

    }


}
