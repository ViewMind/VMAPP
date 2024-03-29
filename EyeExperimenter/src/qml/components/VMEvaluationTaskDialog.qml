import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: evaluationTasksDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    readonly property int vmRowH: evaluationTasksDialog.height*0.1;
    readonly property int vmMaxAllowListH: evaluationTasksDialog.height*0.6;
    readonly property int vmButtonH: vmRowH*0.8;
    readonly property int vmTaskColWidth: evaluationTasksDialog.width*0.20;
    readonly property int vmTaskQCIWidth: evaluationTasksDialog.width*0.1;
    readonly property int vmTaskButtonWidth: evaluationTasksDialog.width*0.1;
    readonly property real vmPercentWheelRowHeightRatio: 0.8
    readonly property real vmSideMargins: VMGlobals.adjustWidth(30)
    readonly property int vmHeightInUpLoadMode: evaluationTaskDialog.height*0.6;

    readonly property int vmMAX_L_COMMMENT_LINES: 11
    readonly property int vmMAX_C_COMMENT_CHARS: 92

    readonly property int vmSTATE_TASK_LIST:  0;
    readonly property int vmSTATE_REDO:       1;
    readonly property int vmSTATE_UPLOAD:     2;

    property bool vmReadyForUpload: false;

    signal requestReopen(var evalID)

    Item {
        id: own
        property string vmEvaluationID: ""
        property int vmState: vmSTATE_TASK_LIST
        property string vmTaskToRedo: ""
        property string vmTaskToRedoName: ""
        property int vmComputedHeight: 0;
        property var vmRedoReasonsCodes: []
        property string vmComment: "";

        onVmStateChanged: {

            comment.setText("")
            comment.vmErrorMsg = ""; // Ensuring there is no persisting error message

            if (vmState == vmSTATE_TASK_LIST) dialog.height = vmComputedHeight;
            else dialog.height = vmHeightInUpLoadMode;

            let text = "";
            if (vmState == vmSTATE_REDO){
                text = loader.getStringForKey("viewqc_reason");
                text = text.replace("<<T>>",vmTaskToRedoName)
            }
            else if (vmState == vmSTATE_UPLOAD){
                comment.setText(vmComment)
            }
            instructionText.text = text;

        }

    }

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    // The input should be the resutl fo the getDisplayEvaluationTask DB function.
    function open(dbtask_data){

        own.vmState = vmSTATE_TASK_LIST
        visible = true

        let taskList = dbtask_data.tasks;
        let name     = dbtask_data.name;
        let evalName = dbtask_data.eval;
        let dateTime = dbtask_data.date;
        own.vmComment  = dbtask_data.comment;

        let taskNameMap  = loader.getTaskCodeToNameMap();

        let desiredListH = taskList.length*vmRowH
        if (desiredListH > vmMaxAllowListH){
            desiredListH = vmMaxAllowListH;
        }

        dialog.height = desiredListH +
                patientName.height + patientName.anchors.topMargin +
                topDivider.height + topDivider.anchors.topMargin +
                bottomDivider.height + bottomDivider.anchors.topMargin +
                btnClose.height + btnClose.anchors.topMargin;
        dialog.height = dialog.height + VMGlobals.adjustHeight(25)
        own.vmComputedHeight = dialog.height;


        items.clear();
        vmReadyForUpload = true;
        for (let i = 0; i < taskList.length; i++){

            let name = taskList[i].name;
            if (name in taskNameMap){
                taskList[i].name = taskNameMap[name]
            }

            if (taskList[i].qci < 0) {
                vmReadyForUpload = false;
            }
            items.append(taskList[i]);
        }

        patientName.text = name;
        evaluationName.text = " - " + evalName;
        dateText.text = dateTime;

        // Security measuer.
        let effectiveW = (vmTaskColWidth + vmTaskQCIWidth + vmTaskButtonWidth)
        let textW = patientName.width + evaluationName.width + dateText.width;
        if (textW > effectiveW){
            patientName.visible = false;
            evaluationName.text = evalName;
        }
        else {
            patientName.visible = true
        }

        // We need to store the subject and evlauation id.
        own.vmEvaluationID = dbtask_data.eval_id;               
        comment.setText()
        comment.vmErrorMsg = ""; // Ensuring there is no persisting error message

        // Now we setup the reason codes and strings.
        let reasonMap = loader.getStringListForKey("viewqc_archive_reason");
        // console.log(JSON.stringify(reasonMap))

        // In order to make reasons both function with a code and be future proof the string map will contain both the reason string code
        // codes are in even positions and texts are in odd positions.
        own.vmRedoReasonsCodes = []; // We store the reason code so that we know.
        let redoReasonsText = []; // And the actual display string.

        let lastKey = "";
        for (let i = 0; i < reasonMap.length; i++){
            if ((i % 2) === 0){
                own.vmRedoReasonsCodes.push(reasonMap[i])
            }
            else {
                redoReasonsText.push(reasonMap[i])
            }
        }

        redoReason.setModelList(redoReasonsText)
        redoReason.setSelection(0);


    }

    function close(){
        visible = false        
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

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        width: (vmTaskColWidth + vmTaskQCIWidth + vmTaskButtonWidth) + 2*vmSideMargins
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    ListModel {
        id: items;
    }

    Text {
        id: patientName
        color: VMGlobals.vmBlackText
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(30)
        anchors.left: dialog.left
        anchors.leftMargin: vmSideMargins
    }

    Text {
        id: evaluationName
        color: VMGlobals.vmGrayLightGrayText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: patientName.verticalCenter
        anchors.left: {
            if (patientName.visible) return patientName.right
            else return dialog.left
        }
        anchors.leftMargin: {
            if (patientName.visible) return 0
            else return vmSideMargins
        }

    }
    Text {
        id: dateText
        color: VMGlobals.vmBlackText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: patientName.verticalCenter
        anchors.right: dialog.right
        anchors.rightMargin: patientName.anchors.leftMargin
    }


    Rectangle {
        id: topDivider
        width: dialog.width - 2*patientName.anchors.leftMargin
        height: VMGlobals.adjustHeight(1)
        anchors.top: patientName.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: dialog.horizontalCenter
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    ScrollView {
        id: contentScroll
        clip: true
        width: vmTaskButtonWidth + vmTaskColWidth + vmTaskQCIWidth;
        height: {
            let desiredH = vmRowH*items.count;
            if (desiredH > vmMaxAllowListH) return vmMaxAllowListH
            else return desiredH;
        }
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: topDivider.bottom

        visible: (own.vmState == vmSTATE_TASK_LIST)

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        Column {

            id: table
            spacing: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            width: parent.width
            height: vmRowH*items.count

            Repeater {

                id: repeater
                model: items
                anchors.fill: parent

                Item {
                    id: tableRow
                    height: vmRowH
                    width: vmTaskButtonWidth + vmTaskColWidth + vmTaskQCIWidth;

                    Text {
                        id: text
                        text: {
                            // We do this because when clearing the items before resetting them, it attemps to get an index which is not defined
                            // generating errors in the console.
                            if (items.get(index) === undefined) return ""
                            let display_text = items.get(index).name
                            if (items.get(index).discarded) display_text = "<i>" + display_text + "</i>"
                            return display_text;
                        }
                        color: VMGlobals.vmGrayPlaceholderText //check.vmCheckIsVisble ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayPlaceholderText
                        font.pixelSize: VMGlobals.vmFontLarge
                        font.weight: 400
                        verticalAlignment: Text.AlignVCenter
                        height: vmRowH
                        width: vmTaskColWidth
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    VMPercentWheel {
                        id: qci
                        y: vmRowH*(1-vmPercentWheelRowHeightRatio)/2
                        vmAnimationDuration: 800
                        vmPercent: {
                            if (items.get(index) === undefined) return 0;
                            if (items.get(index).qci < 0) return 0;
                            else return items.get(index).qci
                        }
                        onVmPercentChanged: {

                            setDesiredCircleHeight(vmRowH*vmPercentWheelRowHeightRatio);
                            x = vmTaskColWidth + (vmTaskQCIWidth - width)/2

                            if (items.get(index) === undefined) return;

                            if (items.get(index).qci < 0){
                                vmOuterColor = VMGlobals.vmBlackText
                                vmInnerColor = VMGlobals.vmGrayLightGrayText
                                vmHideNumber = true;
                                return
                            }

                            vmHideNumber = false;

                            if (items.get(index).qci_ok){
                                vmOuterColor = VMGlobals.vmGreenSolidQCIndicator
                                vmInnerColor = VMGlobals.vmGreenBKGStudyMessages
                            }
                            else {
                                vmOuterColor = VMGlobals.vmRedError
                                vmInnerColor = VMGlobals.vmRedBadIndexBackground
                            }

                        }
                    }

                    VMButton {
                        id: btnRedo
                        vmText: {
                            let default_text = loader.getStringForKey("viewongoing_redo");
                            if (items.get(index) === undefined) return default_text
                            if (items.get(index).discarded) return loader.getStringForKey("viewongoing_discarded");
                            if (items.get(index).qci < 0) return loader.getStringForKey("viewongoing_pending");
                            else return default_text
                        }
                        x: vmTaskColWidth + vmTaskQCIWidth
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        width: vmTaskButtonWidth*0.8
                        vmEnabled: {
                            if (!vmReadyForUpload) return false; // Can't redo an evaluation until done.
                            if (items.get(index) === undefined) return false;
                            if (items.get(index).discarded) return false;
                            if (items.get(index).qci_ok) {
                                // Can only redo tasks that have been done sucessfully, but only if RRS is working.
                                if (flowControl.isRenderServerWorking()) return true
                                return false;
                            }
                            else return false;
                        }
                        onClickSignal: {
                            own.vmTaskToRedo = items.get(index).id;
                            own.vmTaskToRedoName = items.get(index).name
                            own.vmState = vmSTATE_REDO
                        }
                    }

                    Rectangle {
                        id: rowDivider
                        width: parent.width
                        height: topDivider.height
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        color: VMGlobals.vmGrayLightGrayText
                    }

                }
            }

        }




    }

    Text {
        id: instructionText
        text: ""
        color: VMGlobals.vmBlackText
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontLarge
        height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.left: topDivider.left
        anchors.top: topDivider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        visible: (own.vmState == vmSTATE_REDO)
    }

    VMComboBox {
        id: redoReason
        width: topDivider.width
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: instructionText.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        z: parent.z + 1
        visible: (own.vmState == vmSTATE_REDO)
    }

    VMTextAreaInput {
        id: comment
        vmPlaceHolderText: (own.vmState == vmSTATE_UPLOAD) ? loader.getStringForKey("viewqc_comment_ph") : loader.getStringForKey("viewqc_archive_reason_ph")
        vmNumberOfLines: (own.vmState == vmSTATE_REDO) ? 7 : 10
        width: topDivider.width
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: (own.vmState === vmSTATE_REDO) ? redoReason.bottom : topDivider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: (own.vmState != vmSTATE_TASK_LIST)
    }

    Rectangle {
        id: bottomDivider
        width: topDivider.width
        height: topDivider.height
        anchors.left: topDivider.left
        anchors.top: (own.vmState == vmSTATE_TASK_LIST) ? contentScroll.bottom : comment.bottom
        anchors.topMargin: (own.vmState != vmSTATE_TASK_LIST) ? VMGlobals.adjustHeight(30) : topDivider.anchors.topMargin
        border.width: 0
        color: (own.vmState == vmSTATE_TASK_LIST) ? "transparent" : topDivider.color;
    }

    VMButton {
        id: btnClose
        vmButtonType: vmTypeSecondary
        vmText: {
            if (own.vmState == vmSTATE_TASK_LIST) return loader.getStringForKey("update_close");
            else return loader.getStringForKey("viewsettings_cancel");
        }
        anchors.top: bottomDivider.bottom
        anchors.topMargin: topDivider.anchors.topMargin
        anchors.left: dialog.left
        anchors.leftMargin: vmSideMargins
        onClickSignal: {
            if (own.vmState == vmSTATE_TASK_LIST) {
                close()
            }
            else {
                own.vmState = vmSTATE_TASK_LIST
            }
        }
    }

    VMButton {
        id: btnAction
        vmText: {
            if (own.vmState == vmSTATE_TASK_LIST){
                if (vmReadyForUpload) return loader.getStringForKey("viewongoing_upload")
                else return loader.getStringForKey("viewongoing_continue")
            }
            else if (own.vmState == vmSTATE_REDO){
                return loader.getStringForKey("viewongoing_redo_task")
            }
            else {
                // Ready for upload state.
                return loader.getStringForKey("viewongoing_upload")
            }
        }
        anchors.top: btnClose.top
        anchors.right: dialog.right
        anchors.rightMargin: btnClose.anchors.leftMargin
        vmEnabled: {
            // The button should should ONLY be enabled for upload if the RRS is not working.
            if (vmReadyForUpload) return true;
            if (flowControl == null) return false;
            if (flowControl.isRenderServerWorking()) return true;
            return false;
        }
        onClickSignal: {                        
            if (own.vmState == vmSTATE_TASK_LIST){
                if (!vmReadyForUpload){
                    close()
                    // We need to continue the selected evaluation.
                    viewEvaluations.continueEvaluation(own.vmEvaluationID);
                }
                else {
                    own.vmState = vmSTATE_UPLOAD
                }
            }
            else {

                // We need to make sure that the text is formatted properly:
                let formatted_text = breakCommentsIntoLines(comment.getText())

                if (formatted_text === false){
                    comment.vmErrorMsg = loader.getStringForKey("viewQC_commentSizeError");
                    return
                }

                if (own.vmState == vmSTATE_REDO){
                    close()
                    let reasonForRedoing = own.vmRedoReasonsCodes[redoReason.vmCurrentIndex];
                    console.log("Requesting redo of task " + own.vmEvaluationID + "." + own.vmTaskToRedo +  ". Reason for redoing "  + reasonForRedoing);
                    loader.redoTask(own.vmEvaluationID, own.vmTaskToRedo, reasonForRedoing, formatted_text)
                    requestReopen(own.vmEvaluationID);
                }
                else {
                    // START UPLOADING THE EVALUATIONS.
                    console.log("Requesting upload of evaluation " + own.vmEvaluationID);
                    mainWindow.openWaitAsProgress("");
                    loader.uploadEvaluation(own.vmEvaluationID, formatted_text);
                    close()
                }

            }
        }
    }



}
