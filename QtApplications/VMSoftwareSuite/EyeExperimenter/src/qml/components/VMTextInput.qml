import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmTextInput
    height: VMGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""
    property bool vmFocus: false
    property string vmClarification: ""

    readonly property double vmLeftMargin: VMGlobals.adjustWidth(10);
    readonly property double vmRightMargin: VMGlobals.adjustWidth(18);

    signal textChanged()

    function clear(){
        inputText.text = vmPlaceHolderText
        vmCurrentText = "";
        vmErrorMsg = ""
        vmFocus = false
    }

    function setText(text){
        if (text === undefined) return; // Used to avoid warnings
        inputText.text = text
        vmCurrentText = text
    }

    // Text input display.
    Rectangle {

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
        border.width: Math.ceil(vmTextInput.width/300);
        radius: Math.round(10*vmTextInput.height/44);


        // The display Text.
        TextInput {
            id: inputText
            focus: vmFocus
            height: parent.height
            width: parent.width - vmLeftMargin
            anchors.top: parent.top
            anchors.right: parent.right
            enabled: vmEnabled
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400

            text: vmPlaceHolderText

            color: {
                if (!vmEnabled) return VMGlobals.vmGrayPlaceholderText
                if (vmErrorMsg == ""){
                    return (vmCurrentText == "")? VMGlobals.vmGrayPlaceholderText : VMGlobals.vmBlackText
                }
                else{
                    return VMGlobals.vmBlackText
                }
            }

            verticalAlignment: Text.AlignVCenter

            onFocusChanged: {
                if (focus){
                    if (vmCurrentText == "") {
                        inputText.text = ""
                    }
                }
            }

            onTextEdited: {
                vmErrorMsg = ""
                vmCurrentText = inputText.text
                vmTextInput.textChanged();
            }

            onEditingFinished: {
                if (inputText.text == "") {
                    //console.log("Setting the placehoder of " + vmPlaceHolderText);
                    inputText.text = vmPlaceHolderText
                    vmCurrentText = "";
                }
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
