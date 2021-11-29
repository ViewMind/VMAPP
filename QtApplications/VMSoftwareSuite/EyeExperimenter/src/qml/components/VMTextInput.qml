import QtQuick
import QtQuick.Controls

Item {

    id: vmTextInput
    height: vmGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""

    readonly property double vmLeftMargin: Math.ceil(width*10/338);
    readonly property double vmRightMargin: Math.ceil(width*18/338);

    signal textChanged()

    // Text input display.
    Rectangle {

        id: mainRect
        anchors.fill: parent
        color: {
            if (vmErrorMsg === "") return  vmGlobals.vmWhite
            else vmGlobals.vmRedErrorBackground
        }
        border.color: {
            if (vmErrorMsg === ""){
                return vmGlobals.vmGrayUnselectedBorder;
            }
            else {
                return vmGlobals.vmRedError;
            }
        }
        border.width: Math.ceil(vmTextInput.width/300);
        radius: Math.round(10*vmTextInput.height/44);


        // The display Text.
        TextInput {
            id: inputText

            height: parent.height
            width: parent.width - vmLeftMargin
            anchors.top: parent.top
            anchors.right: parent.right

            font.pixelSize: vmGlobals.vmFontBaseSize
            font.weight: 400

            text: vmPlaceHolderText

            color: {
                if (vmErrorMsg == ""){
                    return (vmCurrentText == "")? vmGlobals.vmGrayPlaceholderText : vmGlobals.vmBlackText
                }
                else{
                    return vmGlobals.vmBlackText
                }
            }

            verticalAlignment: Text.AlignVCenter

            onFocusChanged: {
                if (focus){
                    if (vmCurrentText == "") text = ""
                }
            }
            onTextEdited: {
                vmErrorMsg = ""
                vmCurrentText = inputText.text
                vmTextInput.textChanged();
            }
            onEditingFinished: {
                if (inputText.text == "") {
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
        color:  vmGlobals.vmRedError
        font.pixelSize: vmGlobals.vmFontBaseSize
        font.weight: 400
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 10
        visible: (vmErrorMsg !== "")
    }

    //////////////////// THE LABEL (if present).
    Text {
        id: label
        text: vmLabel
        color: vmGlobals.vmBlackText
        font.pixelSize: vmGlobals.vmFontLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 10;
        visible: (vmLabel !== "")
    }

}
