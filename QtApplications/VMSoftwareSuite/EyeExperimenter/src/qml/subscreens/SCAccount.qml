import QtQuick
import "../"
import "../components"

Rectangle {

    id: subScreenAccount
    anchors.fill: parent

    function loadAccountInfo(){

        var evaluator = loader.getCurrentEvaluatorInfo()
        //console.log(JSON.stringify(evaluator));
        fname.setText(evaluator.name)
        lname.setText(evaluator.lastname)
        email.setText(evaluator.email)
        password.clear()
        verifyPassword.clear()

    }

    function checkAndSave(){
        if (password.vmCurrentText !== verifyPassword.vmCurrentText){
            verifyPassword.vmErrorMsg = loader.getStringForKey("viewaccount_badpassverify");
            return;
        }

        if (fname.vmCurrentText === ""){
            fname.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty");
            return;
        }

        if (lname.vmCurrentText === ""){
            lname.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty");
            return;
        }

        loader.addOrModifyEvaluator(email.vmCurrentText,
                                    email.vmCurrentText,
                                    password.vmCurrentText,
                                    fname.vmCurrentText,
                                    lname.vmCurrentText);

        var message = loader.getStringForKey("viewaccount_mod_success")
        message = message.replace("<b></b>","<b>" + fname.vmCurrentText + " " + lname.vmCurrentText + "</b>")
        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,message)
        viewMainSetup.goBackToPatientList()
    }


    Text {
        id: title
        text: loader.getStringForKey("viewpatlist_account")
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(31)
    }


    Rectangle {
        id: initials
        width: VMGlobals.adjustWidth(62)
        height: width
        radius: width/2
        color: VMGlobals.vmBluePanelBKGSelected
        anchors.left: title.left
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)

        Text {
            id: initialsText
            font.pixelSize: VMGlobals.vmFontExtraLarge
            color: VMGlobals.vmBlueSelected
            anchors.fill: parent
            text: viewMainSetup.vmInitials
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

        }
    }

    VMTextInput {
        id: fname
        vmPlaceHolderText: loader.getStringForKey("viewaddeval_fname_ph")
        vmLabel: loader.getStringForKey("viewaddeval_fname")
        width: VMGlobals.adjustWidth(320)
        Keys.onTabPressed: lname.vmFocus = true;
        anchors.left: title.left
        anchors.top: initials.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
    }

    VMTextInput {
        id: lname
        width: fname.width
        vmPlaceHolderText: loader.getStringForKey("viewaddeval_lname_ph")
        vmLabel: loader.getStringForKey("viewaddeval_lname")
        Keys.onTabPressed: password.vmFocus = true;
        anchors.top: fname.top
        anchors.left: fname.right
        anchors.leftMargin: VMGlobals.adjustWidth(20)
    }

    VMTextInput {
        id: email
        width: fname.width
        vmPlaceHolderText: loader.getStringForKey("viewaddeval_email_ph")
        vmLabel: loader.getStringForKey("viewaddeval_email")
        Keys.onTabPressed: password.vmFocus = true;
        anchors.left: title.left
        anchors.top: fname.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
        vmEnabled: false
    }

    VMPasswordInput {
        id: password
        width: fname.width
        vmLabel: loader.getStringForKey("viewlogin_label_pass")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
        vmShowText: loader.getStringForKey("viewlogin_show")
        vmHideText: loader.getStringForKey("viewlogin_hide")
        Keys.onTabPressed: verifyPassword.vmFocus = true;
        anchors.left: title.left
        anchors.top: email.bottom
        anchors.topMargin: VMGlobals.adjustHeight(54)
    }

    VMPasswordInput {
        id: verifyPassword
        width: fname.width
        vmLabel: loader.getStringForKey("viewaddeval_verifpass")
        vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
        vmShowText: loader.getStringForKey("viewlogin_show")
        vmHideText: loader.getStringForKey("viewlogin_hide")
        Keys.onTabPressed: fname.vmFocus = true;
        anchors.verticalCenter: password.verticalCenter
        anchors.left: fname.right
        anchors.leftMargin: VMGlobals.adjustWidth(20)

    }

    Rectangle {
        id: horizontalDivider
        height: VMGlobals.adjustHeight(1)
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(84)
        width: parent.width
        border.width: 0
        color: VMGlobals.vmGrayUnselectedBorder
    }


    VMButton {
        id: saveButton
        vmText: loader.getStringForKey("viewaccount_save")
        vmCustomWidth: VMGlobals.adjustWidth(120)
        anchors.top: horizontalDivider.top
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(29)
        onClickSignal: {
            checkAndSave()
        }
    }

    VMButton {
        id: cancelButton
        vmText: loader.getStringForKey("viewsettings_cancel")
        vmButtonType: cancelButton.vmTypeSecondary
        vmCustomWidth: VMGlobals.adjustWidth(120)
        anchors.top: saveButton.top
        anchors.right: saveButton.left
        anchors.rightMargin: VMGlobals.adjustWidth(20)
        onClickSignal: {
            viewMainSetup.goBackToPatientList()
        }
    }



}
