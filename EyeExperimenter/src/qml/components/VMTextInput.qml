import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmTextInput
    height: VMGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmAlignErrorLeft: true
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""
    property bool vmFocus: false
    property string vmClarification: ""
    property int vmMaxLength: 0

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

        TextMetrics {
            id: textMeasure
            font: inputText.font
            text: vmCurrentText
            //text: "a"
            //text: inputText.text
        }


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
            autoScroll: true

            text: vmPlaceHolderText

//            onWidthChanged: {
//                // We estimate the numbe maximum number of characters that the input can hold by assuming they are all "a" usually the widest character.
//                let nchars = width/textMeasure.advanceWidth;
//                maximumLength = Math.floor(nchars)
//                if (vmCurrentText == ""){
//                    clear()
//                }
//            }

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
                //console.log("Focus Changed: vmCurrentText '" + vmCurrentText + "'")
                //console.log("Focus Changed: inputText.text '" + inputText.text + "'")
                if (focus){
                    if (vmCurrentText == "") {
                        inputText.text = ""
                    }
                }
            }

            // The active focus changes is required for when clicking out of the applications instead of just out of the editing box.
            // I'm not 100% sure between the differnce between active focus and the other focus
            // But if you click into the box, both the active focus and the focus will change
            // If you then click out of the application the active focus will change and focus will remaion on.
            // Clicking back in the box will AGAIN change the active focus to true and focus will still be true.
            onActiveFocusChanged: {

                if (activeFocus){
                    mainWindow.requestApplicationActiveFocus();
                }

                if (focus){
                    if (vmCurrentText == "") {
                        inputText.text = ""
                    }
                }
                if (!activeFocus){
                    if (inputText.text == "") {
                        //console.log("Setting the placehoder of " + vmPlaceHolderText);
                        inputText.text = vmPlaceHolderText
                        vmCurrentText = "";
                    }
                }
            }

            onTextEdited: {
                //console.log("Text Edited: vmCurrentText '" + vmCurrentText + "'")
                //console.log("Text Edited: inputText.text '" + inputText.text + "'")
                vmErrorMsg = ""
                vmCurrentText = inputText.text
                //console.log("Measured text: " + textMeasure.advanceWidth)
                if (vmMaxLength == 0){
                    if (textMeasure.advanceWidth >= (width - vmLeftMargin*2)){
                        maximumLength = inputText.text.length;
                    }
                    else {
                        maximumLength = 32767 // This is the default value.
                    }
                }
                else {
                    maximumLength = vmMaxLength
                }
                //console.log("Maximum length is now: " + maximumLength)
                vmTextInput.textChanged();

            }

            onEditingFinished: {

                //console.log("Editing Finished: vmCurrentText '" + vmCurrentText + "'")
                //console.log("Editing Finished: inputText.text '" + inputText.text + "'")

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
        //anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 10
        x: {
            if (vmAlignErrorLeft) return 0
            else {
                return (vmTextInput.width - width)
            }
        }
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
