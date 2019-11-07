import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewprotocol_"
    id: viewProtocols
    modal: true
    width: 614
    height: 360
    closePolicy: Popup.NoAutoClose

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    onOpened: {
        var list =  loader.getProtocolList(false);
        list.unshift(loader.getStringForKey(keybase+"optionSelectProtocol"))
        console.log(list);
        cbProtocolToDelete.setModelList(list);
        labelNewProtocol.vmErrorMsg = "";
        cbProtocolToDelete.vmErrorMsg = "";
        labelNewProtocol.clear()
    }


    VMDefines{
        id: vmDefines
    }

    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"diagTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 70
    }

    Text {
        id: viewSubTitle
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  diagTitle.bottom
        anchors.topMargin: 13
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keybase+"diagSubTitle");
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewProtocols.close();
        }
    }


    Row {
        id: rowAddProtocols
        width: parent.width*0.6
        spacing: mainWindow.width*0.013
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter

        VMTextDataInput{
            id: labelNewProtocol
            width: parent.width*0.7
            vmPlaceHolder: loader.getStringForKey(keybase+"labelNewProtocol");
            anchors.bottom: parent.bottom
        }

        VMButton{
            id: btnAdd
            height: labelNewProtocol.height
            vmFont: viewHome.gothamM.name
            width: parent.width - labelNewProtocol.width - rowAddProtocols.spacing
            vmText: loader.getStringForKey(keybase+"btnAdd");
            anchors.bottom: parent.bottom
            onClicked: {
                if (loader.addProtocol(labelNewProtocol.vmEnteredText)){
                    viewProtocols.close();
                }
                else labelNewProtocol.vmErrorMsg =  loader.getStringForKey(keybase+"msgExists");
            }
        }
    }

    Row {
        id: rowDeleteProtocols
        width: parent.width*0.6
        spacing: mainWindow.width*0.013
        anchors.top: rowAddProtocols.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter

        VMComboBox2{
            id: cbProtocolToDelete
            width: labelNewProtocol.width
            anchors.bottom: parent.bottom
            vmMaxDisplayItems: 3
            onVmCurrentIndexChanged: {
                cbProtocolToDelete.vmErrorMsg = "";
            }
        }

        VMButton{
            id: btnDelete
            vmFont: viewHome.gothamM.name
            width: btnAdd.width
            height: btnAdd.height
            vmText: loader.getStringForKey(keybase+"btnDelete");
            anchors.bottom: parent.bottom
            onClicked: {
                if (cbProtocolToDelete.vmCurrentIndex > 0){
                    if (cbProtocolToDelete.vmErrorMsg !== ""){
                        loader.deleteProtocol(cbProtocolToDelete.vmCurrentText)
                        viewProtocols.close();
                    }
                    else{
                        cbProtocolToDelete.vmErrorMsg =  loader.getStringForKey(keybase+"msgPressAgainToDelete");
                    }
                }
            }
        }
    }

}
