import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

ApplicationWindow {

    id: mainWindow
    visible: true
    visibility: Window.Maximized
    title: qsTr("Hello World")
    //color: "#99000000" <- Way to make it translucent.
    color: "#7e7e7e"

    VMGlobals {
        id: vmGlobals
    }


    Component.onCompleted: {
        // This ensures that no resizing is possible.
        minimumHeight = height;
        maximumHeight = height;
        minimumWidth = width;
        maximumWidth = width;
        vmGlobals.mainHeight = height;
        vmGlobals.mainWidth = width;
    }

    Item {
        id: testItem
        width: 500
        height: 200

        anchors.left: testColumn.left
        anchors.top: testColumn.bottom
        anchors.topMargin: 20

        onActiveFocusChanged: {
            if (!activeFocus) example.focus = false;
        }

        property double vmFontSize: vmGlobals.vmFontBaseSize

        function changeFontSize(){
            example.font.pixelSize = 20;
        }

        ScrollView {
            id: test
            anchors.fill: parent
            TextArea {
                id: example
                anchors.fill: parent
//                anchors.top: parent.top
//                anchors.right: parent.right
//                enabled: vmEnabled
//                font.pixelSize: {

//                    let f = vmGlobals.vmFontBaseSize
//                    console.log("Returning font base as " + f)
//                    return f;
//                }
                font.pixelSize: 10
//                font.weight: 400
//                leftPadding: vmGlobals.adjustHeight(10)
//                topPadding: vmGlobals.adjustHeight(12)
//                rightPadding: leftPadding
//                bottomPadding: topPadding
//                wrapMode: Text.WordWrap
//                readOnly: false

            }
            Component.onCompleted: {
                //example.font.pixelSize = vmGlobals.vmFontBaseSize
            }

        }

    }

    Column {
        id: testColumn
        anchors.centerIn: parent
        spacing: 50

        VMTextAreaInput {
            id: testComboBox
            vmPlaceHolderText: "Select a value"
            vmLabel: "Some label to try out"
            width: 500
            //z: 10
            //vmErrorMsg: "This is an error message"
        }

        Button {
            id: testButton
            text: "Presss Me"
            onClicked: {
                testButton.forceActiveFocus()
                testItem.changeFontSize();
            }
        }



        //        VMSearchInput {
        //            id: testSearchInput
        //            vmPlaceHolderText: "Search for something here"
        //            width: vmGlobals.vmControlsHeight*7.68
        //            onTextChanged: {
        //                console.log("Search for " + vmCurrentText)
        //            }
        //        }

        //        VMTextInput {
        //            id: textInput
        //            vmPlaceHolderText: "Text input"
        //            vmLabel: "Label for text input"
        //            width: vmGlobals.vmControlsHeight*7.68
        //            onTextChanged: {
        //                console.log("Text is " + vmCurrentText)
        //                if (vmCurrentText == "juan"){
        //                    console.log("ERROR")
        //                    vmErrorMsg = "Value not acceptable";
        //                }
        //            }
        //        }

        //        VMPasswordInput {
        //            id: passwordInput
        //            vmPlaceHolderText: "Enter password"
        //            width: vmGlobals.vmControlsHeight*7.68
        //        }

        //        Row {

        //            id: rowPressableCotrols
        //            spacing: testColumn.spacing

        //            VMPrimaryButton {
        //                id: testPrimaryButton
        //                vmText: "START"
        //                onCustomClicked: {
        //                    console.log("Clicked")
        //                }
        //            }

        //            VMPrimaryButton {
        //                id: testPrimaryButton2
        //                vmText: "DISABLED"
        //                vmEnabled: false
        //            }

        //            VMToggle {
        //                id: testToggle
        //            }

        //            VMCheckBox {
        //                id: testCheckBox
        //            }

        //            VMRadioButton {
        //                id: testRadioButton
        //            }

        //        }

    }


}
