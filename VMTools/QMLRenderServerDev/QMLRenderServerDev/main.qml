import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import com.qml 1.0

Window {

    width: 1920
    height: 1080
    visible: true
    title: qsTr("Render Server Development Helper App")

    Connections {
        target: control
        function onNewImageAvailable () {
            renderServerView.image = control.image;
        }
    }

    Column {

        id: buttonRow
        width: 0.1*parent.width
        anchors.left: parent.left
        anchors.top: parent.top

        Button {
            id: btnConnect
            hoverEnabled: false;
            width: parent.width
            text: "CONNECT"
            onClicked: {
                control.connectToRenderServer();
            }
        }

//        Button {
//            id: btnLoadImage
//            hoverEnabled: false;
//            width: parent.width
//            text: "LOAD IMAGE"
//            onClicked: {
//                control.loadImageFromFile("C:/Users/ViewMind/Documents/UnityProjects/CommunicationDev/screenshot_0.png");
//            }
//        }

//        Button {
//            id: btnSendTestData
//            hoverEnabled: false;
//            width: parent.width
//            text: "SEND TEST DATA"
//            onClicked: {
//                control.sendTestData();
//            }
//        }


    }

    Rectangle {
        id: frame
        width: renderServerView.width*1.01
        height: renderServerView.height*1.01
        border.color: "#000000";
        border.width: 5;
        anchors.centerIn: renderServerView
    }


    // Where all images will be displayed.
    QImageDisplay {
        id: renderServerView
        width: parent.width*0.7;
        height: parent.height*0.8;
        anchors.centerIn: parent
        Component.onCompleted: {
            control.setTargetInDisplayResolution(width,height);
        }
    }

}
