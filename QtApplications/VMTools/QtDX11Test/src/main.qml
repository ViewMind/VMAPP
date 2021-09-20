import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0

Window {
    visible: true
    width: 1000
    height: 600
    title: qsTr("Qt-QML-DX11 Tester")

    Column{

        width: 100
        spacing: 10
        anchors.centerIn: parent

        Button {
            id: startTest
            width: parent.width
            height: 50
            text: "Start Test"
            onClicked: {
                control.startTest();
            }
        }

        Button {
            id: stopTest
            width: parent.width
            height: 50
            text: "Stop Test"
            onClicked: {
                control.stopTest();
            }
        }

    }

}


