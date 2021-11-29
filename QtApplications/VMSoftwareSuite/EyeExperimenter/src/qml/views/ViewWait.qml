import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Rectangle {

    id: vmWaitScreen
    z: 100;
    visible: false
    width: VMGlobals.mainWidth
    height: VMGlobals.mainHeight;
    color: VMGlobals.vmWhite

    property string vmText: ""

    function show(){
        visible = true;
    }

    function hide(){
        visible = false;
    }

    AnimatedImage {
        id: slideAnimation
        source: "qrc:/images/loader.gif"
        height: VMGlobals.adjustHeight(120)
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(351)
    }

    Text {
        id: waitText
        text: vmText
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 600
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: slideAnimation.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

}
