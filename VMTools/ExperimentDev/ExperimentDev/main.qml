import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import com.qml 1.0

Window {
    visible: true
    title: qsTr("QML-VRTester & Experiment Graphical Designer")
    visibility: Window.Maximized
    id: mainWindow

    Rectangle {
        id: background
        color: "#aaaaaa"
        anchors.fill: parent
    }

    Connections{
        target: control
        function onNewImageAvailable() {
            hmdView.image = control.image;
        }
    }

    QImageDisplay {
        id: hmdView
        width: parent.width*0.9;
        height: parent.height*0.8;
        anchors.top: parent.top
        anchors.topMargin: parent.height*0.01
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Column {
        id: rowsOfOptions
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: hmdView.bottom
        anchors.topMargin: parent.height*0.01
        spacing: parent.height*0.01
        width: parent.width
        height: parent.height*0.2

        Row {
            id: row1
            spacing: parent.width*0.01;
            width: parent.width
            height: mainWindow.height*0.05

            Button {
                id: startEyeTrackingTest
                width: parent.width*0.1;
                height: parent.height
                text: "EyeTrackingTest"
                hoverEnabled: false
                onClicked: {
                    control.setRenderArea(hmdView.width,hmdView.height);
                    control.setEyeTracker(eyeTrackingSelection.currentIndex)
                    control.testEyeTracking();
                    // This is to make all options dissppear to force a restart. This is just a test application. Fast. Not Right.
                    // rowsOfOptions.z = -1;
                }
            }

            ComboBox {
                id: eyeTrackingSelection
                width: parent.width*0.1;
                height: parent.height
                model: [ "Mouse", "HP Headset", "HTC Vive"]
            }

            Button {
                id: startCalibration
                width: parent.width*0.1;
                height: parent.height
                text: "Calibrate"
                hoverEnabled: false
                onClicked: {
                    control.setEyeTracker(eyeTrackingSelection.currentIndex)
                    control.startCalibration();
                }
            }

            Button {
                id: startStopEyeTracking
                width: parent.width*0.1;
                height: parent.height
                text: "Start/Stop"
                hoverEnabled: false
                onClicked: {
                    control.switchEyeTrackerEnableState();
                }
            }

        }

    }


}
