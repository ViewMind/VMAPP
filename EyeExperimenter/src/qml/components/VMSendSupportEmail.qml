import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

Item {

    id: sendSupportEmailDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(){
        subject.setSelection(-1);
        issues.clear()
        evaluator.setModelList(loader.getLoginEmails(true));
        visible = true
    }

    function close(){
        visible = false
    }

    property var vmReasonCodes: []

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {

        }
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(821)
        height: VMGlobals.adjustHeight(650)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }
    /////////////////////////////////////////////////////////////////////

    function setCurrentEvaluator(mail){
       if (mail === "") {
           evaluator.setSelection(-1);
           evaluator.vmEnabled = true;
           return;
       }
       let emails = loader.getLoginEmails()
       for (let i = 0; i < emails.length; i++){
           if (emails[i].includes(mail)){
               evaluator.setSelection(i);
               evaluator.vmEnabled = false;
               break;
           }
       }
    }

    Connections {
        target: loader

        function onSendSupportEmailDone(isok) {
            mainWindow.closeWait();
            if (isok){
                mainWindow.popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewsendsupport_email_succes"),false)
            }
            else {
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewsendsupport_email_fail"),false)
            }
        }
    }



    Text {
        id: dialogTitle
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontExtraLarge
        text: loader.getStringForKey("viewsendsupport_email_title")
        height: VMGlobals.adjustHeight(37)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(34)
        anchors.horizontalCenter: dialog.horizontalCenter    
    }

    // The close button for the dialog
    VMCloseButton {
        id: closeDialog
        anchors.right: dialog.right
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(29.75)
        anchors.rightMargin: VMGlobals.adjustWidth(29.75)
        onCloseSignal: {
            sendSupportEmailDialog.close();
        }
    }


    Column {

        id: mainColumn
        width: dialog.width*0.86
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(140)
        anchors.horizontalCenter: dialog.horizontalCenter
        spacing: VMGlobals.adjustHeight(50)

        VMComboBox {
            id: subject
            vmLabel: loader.getStringForKey("viewsendsupport_email_subject")
            vmPlaceHolderText: loader.getStringForKey("viewsendsupport_email_subject_ph")
            width: mainColumn.width
            z: 20
            Component.onCompleted: {
                var reasons = loader.getStringListForKey("viewsendsupport_email_reasons")
                vmReasonCodes = [];
                let stringCodes = [];
                for (let i = 0; i < reasons.length; i++){
                    let parts = reasons[i].split("-");
                    vmReasonCodes.push(parts[0]);
                    stringCodes.push(parts[1]);
                }
                setModelList(stringCodes)
            }
        }

        VMComboBox {
            id: evaluator
            vmLabel: loader.getStringForKey("viewsendsupport_email_sender")
            vmPlaceHolderText: loader.getStringForKey("viewsendsupport_email_sender_ph")
            width: mainColumn.width
            z: 10
        }

        VMTextAreaInput {
            id: issues
            width: mainColumn.width
            vmLabel: loader.getStringForKey("viewsendsupport_email_issue")
            vmPlaceHolderText: loader.getStringForKey("viewsendsupport_email_issue_ph")
            vmNumberOfLines: 10
        }

    }

    VMButton {
        id: btnSend
        vmText: loader.getStringForKey("viewsendsupport_email_send")
        vmCustomWidth: VMGlobals.adjustHeight(150)
        anchors.right: mainColumn.right
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(20)
        onClickSignal: {

            if (issues.getText() === ""){
                issues.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error");
                return
            }

            // We need to check that no fields are empty.
            if (subject.vmCurrentIndex == -1){
                subject.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error");
                return
            }

            if (evaluator.vmCurrentIndex == -1){
                evaluator.vmErrorMsg = loader.getStringForKey("viewsendsupport_email_error");
                return
            }

            let fullEvalText = evaluator.vmCurrentText;
            let parts = fullEvalText.split("(");
            let eval_name = parts[0].trim();
            let eval_email = parts[1].replace(")","");
            let subject_code = vmReasonCodes[subject.vmCurrentIndex]
            eval_email = eval_email.trim();

            mainWindow.openWait(loader.getStringForKey("viewwait_synching"))

            close();

            loader.sendSupportEmail(subject_code,
                                    issues.getText(),
                                    eval_name,
                                    eval_email);

        }
    }




}

