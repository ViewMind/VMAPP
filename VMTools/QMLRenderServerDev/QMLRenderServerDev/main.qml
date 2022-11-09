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
            text: "HIDE"
            onClicked: {
                control.hideRenderWindow();
            }
        }

        Button {
            id: btnShow
            hoverEnabled: false;
            width: parent.width
            text: "SHOW"
            onClicked: {
                control.showRenderWindow();
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

    }

    Rectangle {
        id: frame
        width: parent.width*0.7;
        height: parent.height*0.8;
        border.color: "#000000";
        border.width: 5;
        anchors.centerIn: parent
        color: "transparent"
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

    onWindowStateChanged: {
    }

}
