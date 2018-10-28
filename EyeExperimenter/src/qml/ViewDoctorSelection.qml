import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewdrsel_"

    id: viewDoctorSelection
    modal: true
    width: 654
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

    function updateDrProfile(){
        var ans = loader.getDoctorList();
        ans.unshift(loader.getStringForKey(keybase+"labelDrProfile"));
        labelDrProfile.vmModel = ans;
        labelDrProfile.currentIndex = 0;
    }

    function setCurrentDoctor(){
        if ((labelDrProfile.currentIndex == 0) || (labelDrProfile.count == 0)){
            labelDrProfile.vmErrorMsg = loader.getStringForKey(keybase+"labelNoDrError");
            return false;
        }
        else{            
            var name = labelDrProfile.currentText;
            var uid = loader.getDoctorUIDByIndex(labelDrProfile.currentIndex-1)

            // Cleaning the name
            var parts = name.split("(");
            name = parts[0];

            //console.log("Setting the DOCTOR UID to: " + uid);
            loader.setValueForConfiguration(vmDefines.vmCONFIG_DOCTOR_UID,uid);
            loader.setValueForConfiguration(vmDefines.vmCONFIG_DOCTOR_NAME,name);
            return true;
        }

    }

    function changeView(wait){
        if (wait){
           btnClose.visible = false;
           rowProfileAndAdd.visible = false;
           drPic.visible = false;
           diagTitle.visible = false;
           diagSubTitle.visible = false;
           instPassRow.visible = false;
           btnOk.visible = false;
           diagVerifWaitTitle.visible = true;
           slideAnimation.visible = true;
        }
        else{
            btnClose.visible = true;
            rowProfileAndAdd.visible = true;
            drPic.visible = true;
            diagTitle.visible = true;
            diagSubTitle.visible = true;
            btnOk.visible = true;
            diagVerifWaitTitle.visible = false;
            slideAnimation.visible = false;
        }

    }

    Connections{
        target: loader
        onInstPasswordVerifyResults:{
            changeView(false);
            instPassword.clear();
            if (verifyMsg !== ""){
                instPassword.vmErrorMsg = verifyMsg;
                instPassRow.visible = true;
            }
            else{
                labelDrProfile.vmErrorMsg = "";
                instPassword.vmErrorMsg = "";
                instPassRow.visible = false;
            }
        }
    }

    onOpened: {
        instPassword.clear();
        instPassword.setText("3n5YPEfz");
        updateDrProfile();
    }

    //****************************************************************************************
    //******************* SECOND VIEW FOR AWAITING PASSWORD VERIFICATION *********************
    //****************************************************************************************

    // The instruction text
    Text {
        id: diagVerifWaitTitle
        font.family: viewHome.gothamB.name
        font.pixelSize: 43
        anchors.top: parent.top
        anchors.topMargin: 88
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: loader.getStringForKey(keybase+"verif_title");
        visible: false;
    }

    AnimatedImage {
        id: slideAnimation
        source: "qrc:/images/LOADING.gif"
        anchors.top: diagVerifWaitTitle.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }

    //****************************************************************************************
    //****************************************************************************************
    //****************************************************************************************

    VMDefines{
        id: vmDefines
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewDoctorSelection.close();
        }
    }

    Image {
        id: drPic
        source: "qrc:/images/dr_icon.png"
        width: 108
        height: 108
        anchors.top: parent.top
        anchors.topMargin: 70
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 40
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"viewTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: drPic.bottom
        anchors.topMargin: 15
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11
        font.family: viewHome.gothamR.name
        color: "#969696"
        text: loader.getStringForKey(keybase+"viewSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: 10
    }

    Row {

        id: rowProfileAndAdd
        spacing: 10
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: 39
        anchors.horizontalCenter: parent.horizontalCenter

        // The selection box and the add button
        VMComboBox{
            id: labelDrProfile
            width: 350
            vmModel: []
            font.family: viewHome.robotoR.name
            onCurrentIndexChanged: {
                if (currentIndex > 0){
                    if (!loader.isDoctorValidated(currentIndex-1)){
                        vmErrorMsg = loader.getStringForKey(keybase+"novalid_msg");
                        instPassRow.visible = true;
                    }
                    else {
                        instPassRow.visible = false;
                        vmErrorMsg = "";
                    }
                }
                else {
                    vmErrorMsg = "";
                    instPassRow.visible = false;
                }
                instPassword.vmErrorMsg = "";
            }
        }

        VMPlusButton{
            id: btnAddProfile
            height: labelDrProfile.height
            anchors.bottom: labelDrProfile.bottom
            onClicked: {
                viewDoctorSelection.close();
                viewDrInfo.clearAllFields();
                swiperControl.currentIndex = swiperControl.vmIndexDrProfile;
            }
        }

        VMPencilButton {
            id: btnEditDrInfo
            height: labelDrProfile.height
            anchors.bottom: labelDrProfile.bottom
            onClicked: {
                if (setCurrentDoctor()){
                    viewDoctorSelection.close();
                    viewDrInfo.clearAllFields();
                    viewDrInfo.loadDoctorInformation();
                    swiperControl.currentIndex = swiperControl.vmIndexDrProfile;
                }
            }
        }
    }

    Row {
        id: instPassRow
        anchors.left: rowProfileAndAdd.left
        anchors.top: rowProfileAndAdd.bottom
        anchors.topMargin: 50
        spacing: 10

        VMPasswordField{
            id: instPassword
            vmLabelText: loader.getStringForKey(keybase+"labelInstPassword");
            width: labelDrProfile.width
        }

        VMButton{
            id: btnValidate
            height: labelDrProfile.height
            vmText: loader.getStringForKey(keybase+"btnValidate");
            vmFont: viewHome.gothamM.name
            width: rowProfileAndAdd.width - spacing - instPassword.width
            anchors.bottom: instPassword.bottom
            onClicked: {
                loader.requestDrValidation(instPassword.getText(),labelDrProfile.currentIndex-1);
                changeView(true);
            }
        }

    }

    VMButton{
        id: btnOk
        height: 50
        vmText: loader.getStringForKey(keybase+"btnOk");
        vmFont: viewHome.gothamM.name
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            if (setCurrentDoctor()){
                // Updating the text of the doctor menu.
                viewHome.updateDrMenuText();

                viewDoctorSelection.close();
                swiperControl.currentIndex = swiperControl.vmIndexPatientList
            }
        }
    }

}
