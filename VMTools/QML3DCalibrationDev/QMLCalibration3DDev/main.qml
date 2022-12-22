import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Qt5Compat.GraphicalEffects

ApplicationWindow {

    width: 1280
    height: 900
    visible: true
    title: qsTr("3DCalibrationDev")

    Connections {
        target: control
        function onRequestWindowGeometry () {
            onMove();
        }
        function onEyeTrackerConnected(){
            goBtn.enabled = true;
        }
    }

    function onMove(){
        //console.log("QML WINDOW: (" + x + "," + y +") " + width + "x" + height);
        control.setRenderWindowGeometry(frame.x,frame.y,frame.width,frame.height);
    }

    Rectangle {
        id: frame
        width: parent.width*0.8;
        height: parent.height*0.8;
        border.color: "#000000";
        border.width: 5;
        anchors.centerIn: parent
        color: "transparent"
    }

    Button {
        id: goBtn
        text: "TEST STUFF"
        hoverEnabled: false
        width: 0.5*parent.width
        height: 0.1*parent.height
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: false
        onClicked: {
            //console.log("Ready To Calibrate");
            control.startCalibration();
        }
    }


    onWidthChanged: {
        onMove()
    }

    onYChanged: {
        onMove()
    }

    onXChanged: {
        onMove()
    }

    onHeightChanged: {
        onMove()
    }

    onClosing: {
        control.appClose();
    }


}
