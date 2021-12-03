import QtQuick
import "../"
import "../components"

Rectangle {

    id: subScreenNoPatients
    anchors.fill: parent

    Image {
        id: image
        source: "qrc:/images/docuemnts.png"
        fillMode: Image.PreserveAspectFit
        height: VMGlobals.adjustHeight(150)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(153)
    }

    Text {
        id: title
        text: loader.getStringForKey("viewpatlist_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: image.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewpatlist_nopats")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }


    VMButton {
        vmText: loader.getStringForKey("viewpatlist_addpatient")
        vmIconSource: "plus"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(200)
        onClickSignal: {
            viewAddPatient.clear()
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddPatient);
        }
    }

}
