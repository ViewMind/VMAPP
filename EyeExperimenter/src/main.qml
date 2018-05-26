import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("EyeExperimenter")
    minimumHeight: 780
    maximumHeight: 780
    maximumWidth: 1280
    minimumWidth: 1280

    ViewHome{
        anchors.fill: parent
    }

}
