import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Rectangle {

    id: vmBaseView
    width: VMGlobals.mainWidth
    height: VMGlobals.mainHeight;

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(width,height)
        gradient: Gradient {
            GradientStop { position: 0.0; color: VMGlobals.vmWhite }
            GradientStop { position: 0.034; color: VMGlobals.vmWhite }
            GradientStop { position: 0.7792; color: VMGlobals.vmWhiteMainBackground }
            GradientStop { position: 1.0; color: VMGlobals.vmWhiteMainBackground }
        }
    }

    MouseArea {
        id: focusLooser
        anchors.fill: parent
        onClicked: {
            focusLooser.forceActiveFocus();
        }
    }

}
