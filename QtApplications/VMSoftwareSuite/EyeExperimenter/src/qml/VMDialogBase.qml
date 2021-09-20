import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Dialog {

    property bool vmNoCloseButton: false

    id: vmDialogBase;
    modal: true
    y: (parent.height - height)/2
    x: (parent.width - width)/2
    closePolicy: Popup.NoAutoClose

    signal closeButtonPressed();

    Overlay.modal: Rectangle {
        color: "#aa333333"  // Use whatever color/opacity you like
    }

    contentItem: Rectangle {
        id: diagRect
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow {
            anchors.fill: diagRect
            radius: 10.0
        }
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnPartnerClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.032
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.019
        visible: !vmNoCloseButton
        onClicked: {
            closeButtonPressed();
            vmDialogBase.close()
        }
    }



}
