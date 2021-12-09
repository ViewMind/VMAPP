import QtQuick
import QtQuick.Controls

Item {

    id: vmTextAreaInput
    height: inputText.font.pixelSize*vmNumberOfLines + (vmNumberOfLines-1)*vmSpaceBetweenLines + inputText.bottomPadding + inputText.topPadding

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""
    property bool vmFocus: false
    property string vmClarification: ""
    property int vmNumberOfLines: 5

    readonly property double vmLeftMargin: vmGlobals.adjustWidth(10);
    readonly property double vmRightMargin: vmGlobals.adjustWidth(18);

    readonly property double vmSpaceBetweenLines: inputText.font.pixelSize/2

    signal textChanged()

    function clear(){
        vmCurrentText = "";
        vmErrorMsg = ""
        vmFocus = false
    }

    function setText(text){
        if (text === undefined) return; // Used to avoid warnings
        inputText.text = text
        vmCurrentText = text
    }


    TextArea {
        id: inputText
        focus: vmFocus
        anchors.fill: parent
        anchors.top: parent.top
        anchors.right: parent.right
        enabled: vmEnabled
        font.pixelSize: vmGlobals.vmFontBaseSize
        font.weight: 400
        leftPadding: vmGlobals.adjustHeight(10)
        topPadding: vmGlobals.adjustHeight(12)
        rightPadding: leftPadding
        bottomPadding: topPadding
        wrapMode: Text.WordWrap
        readOnly: false

        onPressed: {
            console.log("Got pressed")
        }

        topInset: 0
        leftInset: 0
        rightInset: 0
        bottomInset: 0

        background: Rectangle {
            id: mainRect
            anchors.fill: parent
            color: {
                if (!vmEnabled) return vmGlobals.vmGrayToggleOff
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
            border.width: vmGlobals.adjustHeight(1);
            radius: vmGlobals.adjustHeight(8);
        }

        placeholderText: vmPlaceHolderText
        placeholderTextColor: vmGlobals.vmGrayPlaceholderText

        color: {
            if (!vmEnabled) return vmGlobals.vmGrayPlaceholderText
            if (vmErrorMsg == ""){
                return (vmCurrentText == "")? vmGlobals.vmGrayPlaceholderText : vmGlobals.vmBlackText
            }
            else{
                return vmGlobals.vmBlackText
            }
        }

        //verticalAlignment: Text.AlignVCenter

        onTextChanged: {
            vmErrorMsg = ""
            vmCurrentText = inputText.text
            vmTextAreaInput.textChanged();
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

    //////////////////// THE LABEL (if present) + THE CLARIFICATION (if present).
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

    Text {
        id: clarification
        text: vmClarification
        color: vmGlobals.vmGrayPlaceholderText
        font.pixelSize: vmGlobals.vmFontLarge
        font.weight: 400
        anchors.bottom: label.bottom
        anchors.left: label.right
        anchors.leftMargin: vmGlobals.adjustWidth(5)
        visible: (vmClarification !== "") && (label.visible)
    }

}
