import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewdrsel_"

    id: viewDoctorSelection
    modal: true
    width: 654
    height: 520
    closePolicy: Popup.CloseOnEscape

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    Connections{
        target: loader
        onNewDoctorAdded:{
            loader.getUserDoctorInfoFromDB();
        }
        onUpdatedDoctorList:{
            updateDrProfile()
        }
    }

    function updateDrProfile(){
        var ans = loader.getDoctorList();
        ans.unshift(loader.getStringForKey(keybase+"labelDrProfile"));
        labelDrProfile.vmModel = ans;
    }

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

    VMButton{
        id: btnOk
        height: 50
        vmText: loader.getStringForKey(keybase+"btnOk");
        vmFont: viewHome.gothamM.name
        anchors.top: rowProfileAndAdd.bottom
        anchors.topMargin: 106
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            if ((labelDrProfile.currentIndex == 0) || (labelDrProfile.count == 0)){
                labelDrProfile.vmErrorMsg = loader.getStringForKey(keybase+"labelNoDrError");
            }
            else{
                // Since the format is FirstName LastName - (UID), this knowledge is used to extract the UID.
                var text = labelDrProfile.currentText;
                var parts = text.split("(");
                var uid = parts[1];
                // The last character is a ) so it is removed.
                uid = uid.slice(0,-1);

                //console.log("Setting the UID to: " + uid);
                loader.setConfigurationString(vmDefines.vmCONFIG_DOCTOR_UID,uid);
                loader.setConfigurationString(vmDefines.vmCONFIG_DOCTOR_NAME,parts[0].slice(0,-3));

                // Updating the text of the doctor menu.
                viewHome.updateDrMenuText()

                // Updating the patient list.
                loader.getPatientListFromDB();

                viewDoctorSelection.close();
                swiperControl.currentIndex = swiperControl.vmIndexStudyStart
            }
        }
    }

}