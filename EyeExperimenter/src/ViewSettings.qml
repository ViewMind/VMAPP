import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Dialog {

    property string keybase: "viewsettings_"

    id: viewSettings
    modal: true
    width: 614
    height: 632
    closePolicy: Popup.CloseOnEscape

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: base.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"diagTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 50
    }

    // The settings explanation
//    Text {
//        id: diagSubTitle
//        font.pixelSize: 11
//        font.family: base.robotoR.name
//        color: "#757575"
//        text: loader.getStringForKey(keybase+"diagSubTitle");
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.top: diagTitle.bottom
//        anchors.topMargin: 11
//    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewSettings.close()
        }
    }

    // Doctor's label and input field
    Text {
        id: diagLabelDoctor
        text: loader.getStringForKey(keybase+"diagLabelDoctor");
        font.family: base.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTitle.bottom
        anchors.topMargin: 45
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMLineTextInput{
        id: diagTIDoctor
        vmFont: base.robotoR.name
        width: 314
        height: 20
        anchors.bottom: diagLabelDoctor.bottom
        anchors.bottomMargin: -35
        anchors.left: parent.left
        anchors.leftMargin: 150
    }


    // Doctor's label and input field
    Text {
        id: diagLabelEmail
        text: loader.getStringForKey(keybase+"diagLabelEmail");
        font.family: base.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIDoctor.bottom
        anchors.topMargin: 35
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMLineTextInput{
        id: diagTIEmail
        vmFont: base.robotoR.name
        width: 314
        height: 20
        anchors.top: diagLabelEmail.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    // Combo box for language selection and label
    Text {
        id: diagLabelLang
        text: loader.getStringForKey(keybase+"diagLabelLang");
        font.family: base.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagTIEmail.bottom
        anchors.topMargin: 38
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMComboBox{
        id: diagCBLang
        vmModel: ["English", "Spanish"]
        font.family: base.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelLang.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    // Combo box for eyetracker selection and label
    Text {
        id: diagLabelET
        text: loader.getStringForKey(keybase+"diagLabelET");
        font.family: base.robotoB.name
        font.pixelSize: 13
        font.bold: true
        anchors.top: diagCBLang.bottom
        anchors.topMargin: 38
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

    VMComboBox{
        id: diagCBET
        vmModel: ["REDm", "Mouse"]
        font.family: base.robotoR.name
        font.pixelSize: 13
        anchors.top: diagLabelET.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

//    VMButton{
//        id: diagBtnAdvance
//        enabled: false
//        vmFont: gothamMedium.name
//        vmSize: [120, 49]
//        vmText: loader.getStringForKey(keybase+"diagBtnAdvance");
//        anchors.top: diagTIEmail.bottom
//        anchors.topMargin: 36
//        anchors.left: parent.left
//        anchors.leftMargin: 150
//    }

    VMButton{
        id: diagBtnOK
        vmFont: base.gothamM.name
        vmSize: [120, 49]
        vmText: loader.getStringForKey(keybase+"diagBtnOK");
        anchors.top: diagCBET.bottom
        anchors.topMargin: 36
        anchors.left: parent.left
        anchors.leftMargin: 150
    }

//    VMCheckBox {
//        id: diagCBoxRemember
//        font.family: base.robotoR.name
//        font.pixelSize: 13
//        text: loader.getStringForKey(keybase+"diagCBoxRemember");
//        anchors.left: parent.left
//        anchors.leftMargin: 150
//        anchors.top: diagBtnAdvance.bottom
//        anchors.topMargin: 40
//    }

}
