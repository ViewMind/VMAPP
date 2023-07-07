import QtQuick
import "../"
import "../components"

Rectangle {

    id: settingsTechSupport
    anchors.fill: parent
    radius: VMGlobals.adjustWidth(10);
    color: VMGlobals.vmGrayToggleOff

    Connections {
        target: loader
        function onLogUploadFinished () {
            // There is nothing to do really. Just close the window.
            mainWindow.closeWait()
        }
    }




    // Rectangle that straightens the left side edge.
    Rectangle {
        id: straightDivisor
        height: parent.height
        anchors.left: parent.left
        width: parent.radius
        border.width: 0
        color: parent.color
    }

    // Title
    Text {
        id: title
        text: loader.getStringForKey("viewsettings_tech_support");
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(18);
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(27)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }

    // Title
    Text {
        id: supportTextExplanation
        text: loader.getStringForKey("viewsettings_support_text");
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        //height: VMGlobals.adjustHeight(18);
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: title.left
    }


    VMButton {
        id: btnUploadLogs
        anchors.top: supportTextExplanation.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(20)
        vmText: loader.getStringForKey("viewsettings_upload_logs")
        onClickSignal: {
            mainWindow.openWait(loader.getStringForKey("viewwait_synching"))
            loader.requestOperatingInfo(true);
        }
    }


}
