import QtQuick
import "../"

Item {

    // When configuring the followign properties need to be set
    // vmHeight: The height of the element.
    // vmImage: The image source
    // vmText: The text to be displayed.
    id: labelItemContainer
    height: vmHeight

    readonly property double vmImageAlign: 0.2*width
    readonly property double vmMargin: VMGlobals.adjustWidth(20)

    Image {
        id: icon
        source: vmImage
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(40)
        anchors.verticalCenter: labelItemContainer.verticalCenter
        x: vmImageAlign - icon.width - vmMargin/2
    }

    Text {
        id: text
        text: vmText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: labelItemContainer.verticalCenter
        x: vmImageAlign + vmMargin/2;
    }

    Rectangle {
        id: divisor
        width: labelItemContainer.width
        anchors.horizontalCenter: labelItemContainer.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.bottom: labelItemContainer.bottom
        visible: !vmIsLast
    }

}
