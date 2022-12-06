import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Qt5Compat.GraphicalEffects

ApplicationWindow {

    id: root
    width: 1000
    height: 900
    visible: true
    title: qsTr("Render Server Development Helper App")

    property int clear_x: 0
    property int clear_y: 0
    property int clear_w: 0
    property int clear_h: 0

    Connections {
        target: control
        function onRequestWindowGeometry () {
            onMove();
        }
    }

    function onMove(){
        //console.log("QML WINDOW: (" + x + "," + y +") " + width + "x" + height);
        control.setRenderWindowGeometry(frame.x,frame.y,frame.width,frame.height);
    }

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(width,height)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#ff0000" }
            GradientStop { position: 0.034; color: "#ff0000" }
            GradientStop { position: 0.7792; color: "#0000ff" }
            GradientStop { position: 1.0; color: "#0000ff" }
        }
    }


    Column {

        id: buttonRow
        width: 0.1*parent.width
        anchors.left: parent.left
        anchors.top: parent.top

        Button {
            id: btnHide
            hoverEnabled: false;
            width: parent.width
            text: "Start Study Explanation"
            onClicked: {
                control.startRenderingStudy();
            }
        }


        Button {
            id: btnNextExplanation
            hoverEnabled: false;
            width: parent.width
            text: "Next Study Explanation";
            onClicked: {
                control.nextStudyExplanation();
            }
        }

        Button {
            id: btnEanblePacketLog
            hoverEnabled: false;
            width: parent.width
            text: "Enable Packet Log";
            onClicked: {
                control.enablePacketLog(true);
            }
        }

        Button {
            id: btnDisablePacketLog
            hoverEnabled: false;
            width: parent.width
            text: "Disable Packet Log";
            onClicked: {
                control.enablePacketLog(false);
            }
        }

        Button {
            id: btnPacketBurst
            hoverEnabled: false;
            width: parent.width
            text: "PACKET BURST";
            onClicked: {
                control.packetBurst();
            }
        }

        Button {
            id: btnCloseServer
            hoverEnabled: false;
            width: parent.width
            text: "STATUS:";
            onClicked: {
                //control.appClose();
                if (!control.checkRenderServerStatus()){
                    btnCloseServer.text = "STATUS: NOT WORKING";
                }
                else {
                    btnCloseServer.text = "STATUS: WORKING";
                }
            }
        }


        Button {
            id: btnForceConnect
            hoverEnabled: false;
            width: parent.width
            text: "Force Connect";
            onClicked: {
                control.forceConnect();
            }
        }

    }

    Column {

        id: studyControlColumn
        width: 0.2*parent.width
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 30;

        ComboBox {
            id: packetSelect
            model: ["Request Packet Description","Start Hand Calibration","Explantion - Next","Explanation - Previous","Examples - Start","Examples - Next","Study - Start"];
            width: parent.width
        }


        Button {
            id: goTest;
            text: "GO";
            width: parent.width
            onClicked: {
                // Need to send the messages.
            }
        }


        Rectangle {
            width: parent.width
            height: 20;
            color: "#ffffff"
            TextInput {
                id: value;
                anchors.fill: parent
            }
        }


    }

    Rectangle {
        id: frame
        width: parent.width*0.7;
        height: parent.height*0.7;
        border.color: "#000000";
        border.width: 5;
        anchors.centerIn: parent
        color: "transparent"
    }

    Rectangle {
        id: messageBox
        width: parent.width
        height: parent.height*0.08;
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        Text {

            id: textMsg
            text: "Received packet types will appear here"
            width: parent.width;
            horizontalAlignment: Text.AlignHCenter;

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
