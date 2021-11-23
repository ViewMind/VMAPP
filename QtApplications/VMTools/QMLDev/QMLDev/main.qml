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
    color: "#ffffff"

    VMGlobals {
        id: vmGlobals
    }

    Component.onCompleted: {
        var list = [];
        for (var i = 0; i < 100; i++){
            list.push("Value " + i);
        }
        testComboBox.setModelList(list);
    }

    onHeightChanged: {
        //console.log("New height is " + mainWindow.height)
        vmGlobals.vmControlsHeight = mainWindow.height*0.06;
    }

    Column {
        id: testColumn
        anchors.centerIn: parent
        spacing: 50

        VMComboBox {
            id: testComboBox
            vmPlaceHolderText: "Select a value"
            vmLabel: "Some label to try out"
            width: vmGlobals.vmControlsHeight*7.68
            z: 10
            onVmCurrentIndexChanged: {
                if (vmCurrentIndex === 5){
                    vmErrorMsg = "This is the wrong component to choose"
                }
            }
        }

        VMSearchInput {
            id: testSearchInput
            vmPlaceHolderText: "Search for something here"
            width: vmGlobals.vmControlsHeight*7.68
            onTextChanged: {
                console.log("Search for " + vmCurrentText)
            }
        }

        VMTextInput {
            id: textInput
            vmPlaceHolderText: "Text input"
            vmLabel: "Label for text input"
            width: vmGlobals.vmControlsHeight*7.68
            onTextChanged: {
                console.log("Text is " + vmCurrentText)
                if (vmCurrentText == "juan"){
                    console.log("ERROR")
                    vmErrorMsg = "Value not acceptable";
                }
            }
        }

        VMPasswordInput {
            id: passwordInput
            vmPlaceHolderText: "Enter password"
            width: vmGlobals.vmControlsHeight*7.68
        }

        Row {

            id: rowPressableCotrols
            spacing: testColumn.spacing

            VMPrimaryButton {
                id: testPrimaryButton
                vmText: "START"
            }

            VMPrimaryButton {
                id: testPrimaryButton2
                vmText: "DISABLED"
                vmEnabled: false
            }

            VMToggle {
                id: testToggle
            }

            VMCheckBox {
                id: testCheckBox
            }

            VMRadioButton {
                id: testRadioButton
            }

        }

    }


}
