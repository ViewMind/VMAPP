import QtQuick
import "../"
import "../components"

Rectangle {

    id: subScreenArchive

    readonly property int vmMAX_L_COMMMENT_LINES: 19
    readonly property int vmMAX_C_COMMENT_CHARS: 92

    property int vmIndexReasonOther: -1

    property var vmReasonStringCodeList: [];

    function reset(){
        archiveReason.setSelection(-1)
        otherBox.clear()
    }

    function archive(){
        if (archiveReason.vmCurrentIndex === -1){
            archiveReason.vmErrorMsg = loader.getStringForKey("viewqc_err_reason_missing")
            return;
        }

        if (archiveReason.vmCurrentIndex === vmIndexReasonOther){
            let ok = viewQC.verifyCommentLength(otherBox.getText())
            if (ok !== ""){
                otherBox.vmErrorMsg = ok;
                return
            }
        }
    }


    Text {
        id: title
        text: loader.getStringForKey("viewqc_archive_studies")
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
        id: warning
        text: loader.getStringForKey("viewqc_archive_explanation")
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        anchors.top: title.bottom
        anchors.left: title.left
        anchors.topMargin: VMGlobals.adjustHeight(10)
    }

    VMComboBox {
        id: archiveReason
        vmLabel: loader.getStringForKey("viewqc_reason")
        vmPlaceHolderText: loader.getStringForKey("viewqc_archive_reason_ph")
        width: VMGlobals.adjustWidth(460)
        anchors.left: parent.left
        anchors.top: warning.bottom
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(54)
        z: parent.z + 1
        Component.onCompleted: {

            var reasonMap = loader.getStringListForKey("viewqc_archive_reason");

            // In order to make reasons both function with a code and be future proof the string map will contain both the reason string code
            // (so that it's order in the string list is irrelevant) and the string itself (language dependant string to show). They are separed by a -
            vmReasonStringCodeList = [];
            var displayList = [];

            for (var i = 0; i < reasonMap.length; i++){
                var key_and_value = reasonMap[i].split("-");
                vmReasonStringCodeList.push(key_and_value[0])
                displayList.push(key_and_value[1])
            }

            // The "other" reason is always the last one.
            vmIndexReasonOther = displayList.length-1;

            setModelList(displayList);

        }
    }

    VMTextAreaInput {
        id: otherBox
        vmEnabled: ((archiveReason.vmCurrentIndex == vmIndexReasonOther) && (vmIndexReasonOther != -1))
        vmLabel: loader.getStringForKey("viewQC_specify_other")
        vmPlaceHolderText: loader.getStringForKey("viewqc_comment_ph")
        vmNumberOfLines: 10
        width: archiveReason.width
        anchors.top: archiveReason.bottom
        anchors.left: archiveReason.left
        anchors.topMargin: VMGlobals.adjustHeight(54)
        onVmEnabledChanged: {
            if (!vmEnabled) clear()
        }
    }

}
