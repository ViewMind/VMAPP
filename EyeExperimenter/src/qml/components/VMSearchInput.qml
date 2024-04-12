import QtQuick
import QtQuick.Controls
import "../"

Item {

    id: vmSearchInput
    height: VMGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""
    property string vmCurrentText: ""

    readonly property double vmLeftMargin: VMGlobals.adjustWidth(10);
    readonly property double vmRightMargin: VMGlobals.adjustWidth(18);

    signal textChanged()

    Rectangle {

        id: mainRect
        anchors.fill: parent
        color: {
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
        radius: VMGlobals.adjustHeight(10);


        // The display Text.
        TextInput {
            id: inputText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
            text: vmPlaceHolderText
            color: {
                if (vmErrorMsg == ""){
                    return (vmCurrentText == "")? VMGlobals.vmGrayPlaceholderText : VMGlobals.vmBlackText
                }
                else{
                    return VMGlobals.vmBlackText
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
                    if (vmCurrentText == "") {
                        text = ""
                    }
                }
            }

            onActiveFocusChanged: {
                if (activeFocus){
                    mainWindow.requestApplicationActiveFocus();
                    if (vmCurrentText == "") {
                        text = ""
                    }
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
            source: "qrc:/images/20px_search.png"
        }

    }

}
