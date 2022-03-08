import QtQuick
import "../"
import "../components"

Rectangle {

    id: subScreenSendReport

    readonly property int vmFAIL_CODE_NONE:   0
    readonly property int vmFAIL_CODE_SERVER: 2

    function reset(){
        comment.clear()
    }

    Connections {
        target: loader
        function onFinishedRequest () {
            // Close the connection dialog and open the user selection dialog.
            mainWindow.closeWait()

            var failCode = loader.wasThereAnProcessingUploadError();

            // This check needs to be done ONLY when on this screen.
            if (( failCode !== VMGlobals.vmFAIL_CODE_NONE ) && (mainWindow.getCurrentSwipeIndex() === VMGlobals.vmSwipeIndexQCView)){

                popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewqc_err_server_error"));
                return;

            }

            if (loader.getLastAPIRequest() === VMGlobals.vmAPI_REPORT_REQUEST){
                popUpNotify(VMGlobals.vmNotificationGreen,loader.getStringForKey("viewqc_success_on_send"));
                swipeTo(VMGlobals.vmSwipeIndexMainScreen)
            }
        }
    }

    function sendToProcess(){
        let ok = viewQC.verifyCommentLength(comment.getText())
        if (ok !== ""){
            comment.vmErrorMsg = ok;
            return
        }

        mainWindow.openWait(loader.getStringForKey("viewqc_send_wait"))

        // Setting the discard reason to empty.
        loader.setDiscardReasonAndComment("",comment.getText())

        // Sending the study to be processed.
        loader.sendStudy();

    }

    Text {
        id: title
        text: loader.getStringForKey("view_qc_send_report")
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.leftMargin: VMGlobals.adjustWidth(30)
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewqc_evaluator_comments")
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        anchors.top: title.bottom
        anchors.left: title.left
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    VMTextAreaInput {
        id: comment
        vmLabel: loader.getStringForKey("viewqc_comment")
        vmPlaceHolderText: loader.getStringForKey("viewqc_comment_ph")
        vmNumberOfLines: 10
        width: subtitle.width
        anchors.top: subtitle.bottom
        anchors.left: subtitle.left
        anchors.topMargin: VMGlobals.adjustHeight(54)
    }

}
