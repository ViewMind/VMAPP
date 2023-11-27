import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

ViewBase {

    id: viewAddEvaluator

    function clear(){
        fname.clear()
        lname.clear()
        password.clear()
        verifyPassword.clear()
        email.clear()
        acceptTerms.vmIsOn = false;
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

        if (email.vmCurrentText === ""){
            email.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty");
            return;
        }

        if (email.vmCurrentText !== verifyEmail.vmCurrentText){
            verifyEmail.vmErrorMsg = loader.getStringForKey("viewaddeval_badverifemail");
            return;
        }

        // Ensuring tha the password is not empty and the email does not already exist.
        if (password.vmCurrentText === ""){
            password.vmErrorMsg = loader.getStringForKey("viewaddeval_err_empty");
            return;
        }
        if (loader.checkIfEvaluatorEmailExists(email.vmCurrentText)){
            labelMail.vmErrorMsg = loader.getStringForKey("viewaddeval_exmail_exists");
            return;
        }

        loader.addOrModifyEvaluator(email.vmCurrentText,
                                    "",
                                    password.vmCurrentText,
                                    fname.vmCurrentText,
                                    lname.vmCurrentText);

        var message = loader.getStringForKey("viewaddeval_added")
        message = message.replace("<b></b>","<b>" + fname.vmCurrentText + " " + lname.vmCurrentText + "</b>")
        mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,message)
        mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome);
    }




    Text {
        id: title
        text: loader.getStringForKey("viewaddeval_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(43)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(645)
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewaddeval_subtitle")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        verticalAlignment: Text.AlignVCenter
        height: VMGlobals.adjustHeight(18)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    Row {

        id: form
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subtitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(70)
        spacing: VMGlobals.adjustWidth(20)

        Column {

            id: inputColumnLeft
            width: VMGlobals.adjustWidth(320)
            spacing: VMGlobals.adjustHeight(60)


            VMTextInput {
                id: fname
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_fname_ph")
                vmLabel: loader.getStringForKey("viewaddeval_fname")
                width: parent.width
                vmAlignErrorLeft: false
                Keys.onTabPressed: lname.vmFocus = true;
            }

            VMTextInput {
                id: email
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_email_ph")
                vmLabel: loader.getStringForKey("viewaddeval_email")
                vmAlignErrorLeft: false
                Keys.onTabPressed: verifyEmail.vmFocus = true;
            }

            VMPasswordInput {
                id: password
                width: parent.width
                vmLabel: loader.getStringForKey("viewlogin_label_pass")
                vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
                vmShowText: loader.getStringForKey("viewlogin_show")
                vmHideText: loader.getStringForKey("viewlogin_hide")
                vmAlignErrorLeft: false
                Keys.onTabPressed: verifyPassword.vmFocus = true;
            }

        }


        Column {

            id: inputColumnRight
            width: VMGlobals.adjustWidth(320)
            spacing: VMGlobals.adjustHeight(60)

            VMTextInput {
                id: lname
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_lname_ph")
                vmLabel: loader.getStringForKey("viewaddeval_lname")
                vmAlignErrorLeft: false
                Keys.onTabPressed: email.vmFocus = true;
            }

            VMTextInput {
                id: verifyEmail
                width: parent.width
                vmPlaceHolderText: loader.getStringForKey("viewaddeval_verifemail")
                vmLabel: loader.getStringForKey("viewaddeval_email")
                vmAlignErrorLeft: false
                Keys.onTabPressed: password.vmFocus = true;
            }

            VMPasswordInput {
                id: verifyPassword
                width: parent.width
                vmLabel: loader.getStringForKey("viewaddeval_verifpass")
                vmPlaceHolderText: loader.getStringForKey("viewlogin_placeholder_password")
                vmShowText: loader.getStringForKey("viewlogin_show")
                vmHideText: loader.getStringForKey("viewlogin_hide")
                vmAlignErrorLeft: false
                Keys.onTabPressed: fname.vmFocus = true;
            }

        }

    }

    VMCheckBox {
        id: acceptTerms
        anchors.top: form.bottom
        anchors.left: form.left
        anchors.topMargin: VMGlobals.adjustHeight(35)
        vmText: loader.getStringForKey("viewaddeval_accept")
        Keys.onTabPressed: fname.vmFocus = true;
        onLinkClicked: function (url){
            loader.openURLInBrowser(url);
        }
    }

    VMButton {
        id: addEvaluButton
        vmText: loader.getStringForKey("viewaddeval_addeval")
        //vmCustomWidth: inputColumn.width
        anchors.top: acceptTerms.bottom
        anchors.topMargin: VMGlobals.adjustHeight(40)
        anchors.horizontalCenter: parent.horizontalCenter
        vmEnabled: acceptTerms.vmIsOn
        onClickSignal: {
            checkAndSave();
        }
    }

    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexHome)
        }
    }


}
