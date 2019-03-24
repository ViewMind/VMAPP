import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Dialog {

    // The possible errors
    readonly property int vmERROR_LOAD_CONFIG: 1
    readonly property int vmERROR_CREATING_PDIR: 2
    readonly property int vmERROR_CONNECT_ET: 3
    readonly property int vmERROR_PROG_ERROR: 4
    readonly property int vmERROR_EXP_END_ERROR: 5
    readonly property int vmERROR_SERVER_COMM: 6
    readonly property int vmERROR_NO_SSL: 7
    readonly property int vmERROR_PROC_ACK: 8
    readonly property int vmERROR_FREQ_ERR: 9
    readonly property int vmERROR_LAUNCHER_UPDATE: 10

    property string vmErrorTitle: "Error Ocurred"
    property string vmErrorMessage: "An error has ocurred"
    property string vmErrorButtonMsg: "OK";
    property int vmErrorCode: 0

    id: errorDiag;
    modal: true
    width: 634
    height: 600
    closePolicy: Popup.NoAutoClose

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
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            errorHandling()
        }
    }

    // The instruction text
    Text {
        id: diagTitle
        font.family: viewHome.gothamB.name
        font.pixelSize: 43
        anchors.top: parent.top
        anchors.topMargin: 88
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: vmErrorTitle
    }

    // The instruction text
    Text {
        id: diagMessage
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top:  diagTitle.bottom
        anchors.topMargin: 26
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: vmErrorMessage
    }

    Image{
        id: diagImage
        source: "qrc:/images/ERROR_ILUS.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:  diagMessage.bottom
        anchors.topMargin: 35
    }

    // The Ok.
    VMButton{
        id: btnOk
        vmSize: [180, 50]
        vmText: vmErrorButtonMsg
        vmFont: gothamM.name
        anchors.top: diagImage.bottom
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            errorHandling()
        }
    }

    function errorHandling(){
        switch(vmErrorCode){
        case vmERROR_CONNECT_ET:
            // Should just close this dialog.
            close();
            break;
        case vmERROR_EXP_END_ERROR:
            close()
            swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            break;
        case vmERROR_SERVER_COMM:
            close();
            break;
        case vmERROR_NO_SSL:
            close();
            break;
        case vmERROR_PROC_ACK:
            close();
            break;
        case vmERROR_FREQ_ERR:
            close();
            break;
        default:
            Qt.quit();
            break;
        }
    }

}
