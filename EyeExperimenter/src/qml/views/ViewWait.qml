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
    property bool showProgress: false;

    function show(){        
        visible = true;
        showProgress = false;
    }

    function showWithProgress(){
        visible = true;
        showProgress = true;
    }

    function hide(){
        visible = false;
    }

    function updateProgress(p,text){
        // This way we can just not add the parameter to keep what was there before.
        if (text !== undefined) progressBar.vmMessage = text;
        progressBar.vmPercent = p;
    }

    AnimatedImage {
        id: slideAnimation
        source: "qrc:/images/loader.gif"
        height: VMGlobals.adjustHeight(120)
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(351)
        visible: !showProgress
    }

    Text {
        id: waitText
        text: vmText
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 600
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: {
            if (!showProgress) return slideAnimation.bottom
            else return parent.top
        }
        anchors.topMargin: {
            if (!showProgress) VMGlobals.adjustHeight(20)
            else parent.height*0.4;
        }
    }

    VMProgressBar {
        id: progressBar
        vmPercent: 0
        vmMessage: ""
        width: parent.width*0.6
        vmPBarHeight: parent.height*0.02
        anchors.top: waitText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(40);
        anchors.horizontalCenter: parent.horizontalCenter
        visible: showProgress
    }

}
