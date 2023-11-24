import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"


// The user manual hyperlink
MouseArea {
    id: userManualClickableArea
    hoverEnabled: true;
    cursorShape: userManualClickableArea.containsMouse? Qt.PointingHandCursor : Qt.ArrowCursor
    height: VMGlobals.adjustHeight(18)
    width: metrics.width + icon.width + icon.anchors.leftMargin

    onClicked: {
        loader.openUserManual()
    }

    Text {
        id: userManualText
        font: metrics.font
        text: metrics.text
        color: {
            if (userManualClickableArea.containsMouse){
                if (userManualClickableArea.pressed) return VMGlobals.vmBlueButtonHighlight
                else return VMGlobals.vmBlueButtonPressed
            }
            return VMGlobals.vmBlueSelected
        }
        verticalAlignment: Text.AlignVCenter
        height: parent.height
    }

    Image {
        id: icon
        source: "qrc:/images/external-link.png"
        height: VMGlobals.adjustHeight(18)
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: userManualText.right
        anchors.leftMargin: VMGlobals.adjustWidth(5)
    }

    TextMetrics {
        id: metrics
        text: loader.getStringForKey("viewsettings_umanual")
        font.pixelSize: VMGlobals.vmFontLarger
        font.underline: userManualClickableArea.containsMouse
        font.weight: 600
    }

}
