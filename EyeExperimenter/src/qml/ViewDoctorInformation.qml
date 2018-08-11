import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewdrinfo_"

    signal updateMenus();

    id: viewDoctorInformation
    modal: true
    width: 654
    height: 686
    closePolicy: Popup.CloseOnEscape

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    VMDefines{
        id: vmDefines
    }

    // The Doctor Information Title.
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 50
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            //viewDoctorInformation.close()
        }
    }

    VMComboBox{
        id: diagCBLang
        vmModel: ["English", "Spanish"]
//        currentIndex: {
//            var lang = loader.getConfigurationString(vmDefines.vmCONFIG_REPORT_LANGUAGE)
//            for (var i = 0; i < vmModel.length; i++){
//                if (vmModel[i] === lang){
//                    vmLoadLanguage = lang;
//                    return i;
//                }
//            }
//            return 0;
//        }
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagTitle.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMComboBox{
        id: diagCBET
        vmModel: ["REDm", "Mouse", "GP3HD"]
//        currentIndex: {
//            var sel = loader.getConfigurationString(vmDefines.vmCONFIG_SELECTED_ET)
//            for (var i = 0; i < vmModel.length; i++){
//                if (vmModel[i] === sel) {
//                    vmLoadET = vmModel[i];
//                    return i;
//                }
//            }
//            return 0;
//        }
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: diagTitle.bottom
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 150
    }

}
