import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0

Window {
    visible: true
    width: 1000
    height: 600
    title: qsTr("Qt-QML-VR Tester")

    Column{

        id: mainColumn
        width: 100
        spacing: 10
        anchors.centerIn: parent

        property bool hasStarted: false;

        Button {
            id: startTest
            width: parent.width
            height: 50
            text: "Start Test"
            onClicked: {
                if (!mainColumn.hasStarted) {
                    text = "Stop Test";
                    mainColumn.hasStarted = true;
                    control.startTest();
                }
                else {                    
                    text = "Start Test";
                    control.stopTest();
                    mainColumn.hasStarted = false;
                }
            }
        }

//        Button {
//            id: tempButton
//            width: parent.width
//            height: 50
//            text: "Test"
//            onClicked: {
//               control.stopTest();
//            }
//        }

    }

}
