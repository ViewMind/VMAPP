import QtQuick
import QtQuick.Controls
import "../"
import "../components"

Rectangle {

    id: evaluationFinished

    Text {
        id: congratsText
        text: loader.getStringForKey("viewevaluation_congrats")
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: trophy.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    Text {
        id: subCongratsText
        text: loader.getStringForKey("viewevaluation_congrats_sub")
        color: VMGlobals.vmGrayPlaceholderText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: congratsText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)

    }

    Image {
        id: trophy
        source: "qrc:/images/congrats.png"
        height: VMGlobals.adjustHeight(150)
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(171)
    }

}
