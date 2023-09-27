import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmTextAreaInput
    height: inputText.font.pixelSize*vmNumberOfLines + (vmNumberOfLines-1)*vmSpaceBetweenLines + inputText.bottomPadding + inputText.topPadding

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property bool vmFocus: false
    property string vmClarification: ""
    property int vmNumberOfLines: 5

    readonly property double vmLeftMargin: VMGlobals.adjustWidth(10);
    readonly property double vmRightMargin: VMGlobals.adjustWidth(18);

    readonly property double vmSpaceBetweenLines: inputText.font.pixelSize/2

    signal textChanged()

    function clear(){
        vmErrorMsg = ""
        vmFocus = false
        inputText.text = ""
    }

    function getText(){
        return inputText.text
    }

    function setText(text){
        if (text === undefined) return; // Used to avoid warnings
        inputText.text = text
    }


    TextArea {
        id: inputText
        focus: vmFocus
        anchors.fill: parent
        anchors.top: parent.top
        anchors.right: parent.right
        enabled: vmEnabled
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 400
        leftPadding: VMGlobals.adjustHeight(10)
        topPadding: VMGlobals.adjustHeight(12)
        rightPadding: leftPadding
        bottomPadding: topPadding
        wrapMode: Text.WordWrap
        readOnly: false

        topInset: 0
        leftInset: 0
        rightInset: 0
        bottomInset: 0

        background: Rectangle {
            id: mainRect
            anchors.fill: parent
            color: {
                if (!vmEnabled) return VMGlobals.vmGrayToggleOff
                if (vmErrorMsg === "") return  VMGlobals.vmWhite
                else VMGlobals.vmRedErrorBackground
            }
            border.color: {
                if (vmErrorMsg === ""){
                    return VMGlobals.vmGrayUnselectedBorder;
                }
                else {
                    return VMGlobals.vmRedError;
                }
            }
            border.width: VMGlobals.adjustHeight(1);
            radius: VMGlobals.adjustHeight(8);
        }

        placeholderText: vmPlaceHolderText
        placeholderTextColor: VMGlobals.vmGrayPlaceholderText

        color: {
            if (!vmEnabled) return VMGlobals.vmGrayPlaceholderText
            else return VMGlobals.vmBlackText
        }

        //verticalAlignment: Text.AlignVCenter

        onTextChanged: {
            vmErrorMsg = ""
            vmTextAreaInput.textChanged();
        }

        onActiveFocusChanged: {
            if (activeFocus){
                mainWindow.requestApplicationActiveFocus();
            }
        }

    }



    //////////////////// THE ERROR MESSAGE
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  VMGlobals.vmRedError
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 400
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 10
        visible: (vmErrorMsg !== "")
    }

    //////////////////// THE LABEL (if present) + THE CLARIFICATION (if present).
    Text {
        id: label
        text: vmLabel
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 10;
        visible: (vmLabel !== "")
    }

    Text {
        id: clarification
        text: vmClarification
        color: VMGlobals.vmGrayPlaceholderText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        anchors.bottom: label.bottom
        anchors.left: label.right
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        visible: (vmClarification !== "") && (label.visible)
    }

}
