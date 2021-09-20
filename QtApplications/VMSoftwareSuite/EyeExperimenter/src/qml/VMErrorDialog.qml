import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Dialog {

    // The possible errors
    property string vmErrorTitle: "Error Ocurred"
    property string vmErrorMessage: "An error has ocurred"
    property string vmErrorButtonMsg: "OK";
    property int vmErrorCode: 0

    readonly property int vmErrorCodeNotClose: 0
    readonly property int vmErrorCodeClose: 1

    id: errorDiag;
    modal: true
    width: mainWindow.width*0.495
    height: mainWindow.height*0.87
    closePolicy: Popup.NoAutoClose

    onWidthChanged: repositionImage();
    onHeightChanged: repositionImage();

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.032
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.02
        onClicked: {
            close();
            if (vmErrorCode == vmErrorCodeClose) Qt.quit()
        }
    }

    // The instruction text
    Text {
        id: diagTitle
        font.family: viewHome.gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.128
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: vmErrorTitle
    }

    // The instruction text
    Text {
        id: diagMessage
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.038
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: vmErrorMessage
    }

    Image{
        id: diagImage
        source: "qrc:/images/ERROR_ILUS.png"
        scale: viewHome.vmScale
        transformOrigin: Item.TopLeft
        onScaleChanged: {
            repositionImage();
        }
    }

    // The Ok.
    VMButton{
        id: btnOk
        vmSize: [mainWindow.width*0.141, mainWindow.height*0.072]
        vmText: vmErrorButtonMsg
        vmFont: gothamM.name
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.06
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            close();
            if (vmErrorCode == vmErrorCodeClose) Qt.quit()
        }
    }

    function repositionImage(){
        diagImage.x = (errorDiag.width - diagImage.width*viewHome.vmScale)/2;
        diagImage.y = (errorDiag.height - diagImage.height*viewHome.vmScale)/2;
    }


}
