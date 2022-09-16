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

    function breakCommentsIntoLines(comment){

        let lines = [];
        let initial_lines = comment.split('\n');

        for (let i = 0; i < initial_lines.length; i++){

            let words = initial_lines[i].split(' ');
            let line_length = 0;
            let line = []

            for (let j = 0; j < words.length; j++){

                line_length = words[j].length + line_length + 1 // The plus one is because of the space

                if (line_length > vmMAX_C_COMMENT_CHARS){

                    if (line.length > 0){
                        // The word need to go in a new line, so the previous one is finished.
                        lines.push(line.join(" "));
                        line.push(words[j]);
                    }
                    else {
                        // A single word larger than the max ammount of chars. Nothing that I can do. This should basically never happen.
                        lines.push(words[j]);
                    }

                    line = [];
                    line_length = 0;

                }

                else  {

                    line.push(words[j]);
                }

            }

            // We need to add whaever words remained.
            lines.push(line.join(" "));

        }

        // Finally we check if the the total number of lines is less than the maximum allowed.
        if (lines.length <= vmMAX_L_COMMMENT_LINES) return lines.join('\n');
        else return false;


    }

    function sendToProcess(){

        let formatted_text = breakCommentsIntoLines(comment.getText())

        if (formatted_text === false){
            comment.vmErrorMsg = loader.getStringForKey("viewQC_commentSizeError");
            return
        }

        mainWindow.openWait(loader.getStringForKey("viewqc_send_wait"))

        // Setting the discard reason to empty.
        loader.setDiscardReasonAndComment("",formatted_text)

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
        wrapMode: Text.Wrap
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
