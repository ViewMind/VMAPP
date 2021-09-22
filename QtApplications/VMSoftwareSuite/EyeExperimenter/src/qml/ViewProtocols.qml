import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMDialogBase {

    id: viewProtocols    
    width: mainWindow.width*0.48
    height: mainWindow.height*0.522

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    onOpened: {
        var list =  loader.getProtocolList();
        list.unshift(loader.getStringForKey("viewprotocol_optionSelectProtocol"))
        //console.log(list);
        cbProtocolToDelete.setModelList(list);
        labelNewProtocol.vmErrorMsg = "";
        cbProtocolToDelete.vmErrorMsg = "";
        labelNewProtocol.clear()
    }


    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey("viewprotocol_diagTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.101
    }

    Text {
        id: viewSubTitle
        font.family: viewHome.robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.019
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey("viewprotocol_diagSubTitle");
    }

    Row {
        id: rowAddProtocols
        width: parent.width*0.6
        spacing: mainWindow.width*0.013
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter

        VMTextDataInput{
            id: labelNewProtocol
            width: parent.width*0.7
            vmPlaceHolder: loader.getStringForKey("viewprotocol_labelNewProtocol");
            anchors.bottom: parent.bottom
        }

        VMButton{
            id: btnAdd
            height: labelNewProtocol.height
            vmFont: viewHome.gothamM.name
            width: parent.width - labelNewProtocol.width - rowAddProtocols.spacing
            vmText: loader.getStringForKey("viewprotocol_btnAdd");
            anchors.bottom: parent.bottom
            onClicked: {
                if (loader.addProtocol(labelNewProtocol.vmEnteredText)){
                    viewProtocols.close();
                }
                else labelNewProtocol.vmErrorMsg =  loader.getStringForKey("viewprotocol_msgExists");
            }
        }
    }

    Row {
        id: rowDeleteProtocols
        width: parent.width*0.6
        spacing: mainWindow.width*0.013
        anchors.top: rowAddProtocols.bottom
        anchors.topMargin: mainWindow.height*0.043
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
            vmText: loader.getStringForKey("viewprotocol_btnDelete");
            anchors.bottom: parent.bottom
            onClicked: {
                if (cbProtocolToDelete.vmCurrentIndex > 0){
                    if (cbProtocolToDelete.vmErrorMsg !== ""){
                        loader.deleteProtocol(cbProtocolToDelete.vmCurrentText)
                        viewProtocols.close();
                    }
                    else{
                        cbProtocolToDelete.vmErrorMsg =  loader.getStringForKey("viewprotocol_msgPressAgainToDelete");
                    }
                }
            }
        }
    }

}
