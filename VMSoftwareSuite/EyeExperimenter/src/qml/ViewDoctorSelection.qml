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


    function logInAttempt(){
//        if (loader.evaluatorLogIn(labelDrProfile.vmCurrentText,drPassword.getText())){
        if (loader.evaluatorLogIn("aarelovich@gmail.com","1234")){

            // Updating the text of the doctor menu.
            viewHome.updateDrMenuText();
            viewDoctorSelection.close();
            drPassword.vmErrorMsg = "";

            /*********************************************
             * FOR DEBUGGING
             *********************************************/

            loader.setSelectedSubject("1_0_20210606090711350");
            //viewStudyStart.vmSelectedMedic = "1";
            //viewStudyStart.vmSelectedProtocol = "";
            //swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            //viewFinishedStudies.loadEvaluatorStudies();
            //swiperControl.currentIndex = swiperControl.vmIndexFinishedStudies
            swiperControl.currentIndex = swiperControl.vmIndexViewQC;
            return;

            /*************************************************/



            swiperControl.currentIndex = swiperControl.vmIndexPatientList;

        }
        else{
            drPassword.vmErrorMsg = loader.getStringForKey(keybase+"drwrongpass");;
        }
    }

    function updateDrProfile(){
        var ans = loader.getLoginEmails();
        //console.log(ans);
        ans.unshift(loader.getStringForKey(keybase+"labelDrProfile"));
        labelDrProfile.setModelList(ans)
        labelDrProfile.vmCurrentIndex = 0;
    }


    onOpened: {
        //labelDrProfile.clear();
        drPassword.clear();
        updateDrProfile();
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

        //        VMTextDataInput {
        //            id: labelDrProfile
        //            width: mainWindow.width*0.273
        //            height: mainWindow.height*0.043
        //            vmFont: viewHome.robotoR.name
        //            vmPlaceHolder: "Email"
        //        }

        // The selection box and the add button
        VMComboBox2{
            id: labelDrProfile
            width: mainWindow.width*0.273
            onVmCurrentIndexChanged: {
                vmErrorMsg = "";
                drPassword.vmErrorMsg = "";
            }
        }

        VMPlusButton{
            id: btnAddProfile
            height: labelDrProfile.height
            anchors.bottom: labelDrProfile.bottom
            onClicked: {
                viewDoctorSelection.close();
                viewDrInfo.clearAllFields();
                loader.logOut();
                viewDrInfo.newEvaluator();
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
