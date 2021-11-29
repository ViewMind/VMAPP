import QtQuick
import QtQuick.Controls

Item {

    id: vmSearchInput
    height: vmGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""

    readonly property double vmLeftMargin: Math.ceil(width*10/338);
    readonly property double vmRightMargin: Math.ceil(width*18/338);

    signal textChanged()

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
        border.width: Math.ceil(vmSearchInput.width/300);
        radius: Math.round(10*vmSearchInput.height/44);


        // The display Text.
        TextInput {
            id: inputText
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

            height: parent.height
            width: parent.width - vmLeftMargin - searchIcon.width
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: searchIcon.right
            anchors.leftMargin: vmLeftMargin

            onFocusChanged: {
                if (focus){
                    if (vmCurrentText == "") text = ""
                }
            }
            onTextEdited: {
                vmCurrentText = inputText.text
                vmSearchInput.textChanged();
            }
            onEditingFinished: {
                if (inputText.text == "") {
                    inputText.text = vmPlaceHolderText
                    vmCurrentText = "";
                }
            }
        }

        // The triangle that functions as an indicator.
        Image {
            id: searchIcon
            height: Math.round(18*vmSearchInput.height/44)
            width: height
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: vmLeftMargin
            source: "qrc:/resources/pngs/20px_search.png"
        }

    }

}
