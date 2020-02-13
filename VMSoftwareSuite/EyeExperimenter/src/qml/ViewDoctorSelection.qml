import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewdrsel_"

    id: viewDoctorSelection
    modal: true
    width: mainWindow.width*0.511
    height: mainWindow.height*0.942
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
        loader.loadDoctorSelectionInformation();
        var ans = loader.getDoctorNameList();
        ans.unshift(loader.getStringForKey(keybase+"labelDrProfile"));
        labelDrProfile.setModelList(ans)
        labelDrProfile.vmCurrentIndex = 0;
    }

    function setCurrentDoctor(){
        if ((labelDrProfile.vmCurrentIndex === 0) || (labelDrProfile.vmListSize === 0)){
            labelDrProfile.vmErrorMsg = loader.getStringForKey(keybase+"labelNoDrError");
            return false;
        }
        else{            
            var name = labelDrProfile.vmCurrentText;
            var uid = loader.getDoctorUIDByIndex(labelDrProfile.vmCurrentIndex-1)

            // Cleaning the name
            var parts = name.split("(");
            name = parts[0];

            //console.log("Setting current doctor to " + name + " and " + uid);

            //console.log("Setting the DOCTOR UID to: " + uid);
            loader.setValueForConfiguration(vmDefines.vmCONFIG_DOCTOR_UID,uid);
            loader.setValueForConfiguration(vmDefines.vmCONFIG_DOCTOR_NAME,name);
            return true;
        }
    }

    function logInAttempt(){
        if (setCurrentDoctor()){
            if (loader.isDoctorPasswordCorrect(drPassword.getText())){
                // Updating the text of the doctor menu.
                viewHome.updateDrMenuText();
                viewDoctorSelection.close();
                viewPatList.vmShowAll = false;
                swiperControl.currentIndex = swiperControl.vmIndexPatientList
                drPassword.vmErrorMsg = "";
            }
            else{
                drPassword.vmErrorMsg = loader.getStringForKey(keybase+"drwrongpass");;
            }
        }
    }


    onOpened: {
        instPassword.clear();
        drPassword.clear();
        //instPassword.setText("3n5YPEfz");
        updateDrProfile();
    }

    VMDefines{
        id: vmDefines
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.032
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.02
        onClicked: {
            viewDoctorSelection.close();
        }
    }

    Image {
        id: drPic
        source: "qrc:/images/dr_icon.png"
        width: mainWindow.width*0.084
        height: mainWindow.height*0.157
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.101
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 40*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"viewTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: drPic.bottom
        anchors.topMargin: mainWindow.height*0.022
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: viewHome.gothamR.name
        color: "#969696"
        text: loader.getStringForKey(keybase+"viewSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.014
    }

    Row {

        id: rowProfileAndAdd
        spacing: mainWindow.width*0.008
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.057
        anchors.horizontalCenter: parent.horizontalCenter
        z: 1

        // The selection box and the add button
        VMComboBox2{
            id: labelDrProfile
            width: mainWindow.width*0.273
            onVmCurrentIndexChanged: {
                if (vmCurrentIndex > 0){
                    if (!loader.isDoctorValidated(vmCurrentIndex-1)){
                        vmErrorMsg = loader.getStringForKey(keybase+"novalid_msg");
                        instPassRow.visible = true;
                    }
                    else {
                        instPassRow.visible = false;
                        instPassword.vmErrorMsg = "";
                        vmErrorMsg = "";
                    }


                    drPassword.setText("");
                    if (loader.isDoctorPasswordEmpty(vmCurrentIndex-1)){
                        //console.log("Setting the error message: " + loader.getStringForKey(keybase+"drnopass"));
                        drPassword.vmErrorMsg = loader.getStringForKey(keybase+"drnopass");
                    }
                    else{
                        drPassword.vmErrorMsg = "";
                    }

                }
                else {
                    vmErrorMsg = "";
                    instPassRow.visible = false;
                    drPassword.vmErrorMsg = "";
                }                
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

    }

    VMPasswordField{
        id: drPassword
        vmLabelText: loader.getStringForKey(keybase+"drpassword");
        width: labelDrProfile.width
        anchors.left: rowProfileAndAdd.left
        anchors.top: rowProfileAndAdd.bottom
        anchors.topMargin: mainWindow.height*0.072
        onEnterPressed: logInAttempt();
    }

    Row {
        id: instPassRow
        anchors.left: rowProfileAndAdd.left
        anchors.top: drPassword.bottom
        anchors.topMargin: mainWindow.height*0.087
        spacing: mainWindow.width*0.008

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
            width: mainWindow.width*0.062
            anchors.bottom: instPassword.bottom
            onClicked: {
                if (!loader.requestDrValidation(instPassword.getText(),labelDrProfile.vmCurrentIndex-1)){
                    instPassword.vmErrorMsg = loader.getStringForKey(keybase+"instpassword_wrong");
                }
                else{
                    instPassRow.visible = false;
                    instPassword.vmErrorMsg = "";
                    viewDoctorSelection.close();
                }
            }
        }
    }

    VMButton{
        id: btnOk
        height: mainWindow.height*0.072
        vmText: loader.getStringForKey(keybase+"btnOk");
        vmFont: viewHome.gothamM.name
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.043
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            logInAttempt();
        }
    }

}
