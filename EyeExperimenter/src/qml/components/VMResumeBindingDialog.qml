import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"

Item {
    id: bindingResumeDialog;
    anchors.fill: parent
    visible: false
    clip: true
    z: 10

    signal removeFile(filepath: string);
    signal resumeBinding(filepath: string);

    property var vmBindingList: []

    ////////////////////////// BASE DIALOG FUNCTIONS ////////////////////////
    function open(binding_studies){

        visible = true
        vmBindingList = binding_studies;

        let displayTexts = [];

        for (let i in vmBindingList){

            let length = loader.getStringForKey("viewevaluation_binding_normal");
            if (vmBindingList[i]["number_of_trials"] != -1) length = loader.getStringForKey("viewevaluation_binding_short");

            let row = [
                vmBindingList[i]["date"],
                vmBindingList[i]["number_targets"],
                length
            ]
            displayTexts.push(row);            
        }

        bindingStudiesTable.setList(displayTexts)

    }

    function close(){
    }

    MouseArea {
        id: mouseCatcher
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: overlay
        color: VMGlobals.vmGrayDialogOverlay
        anchors.fill: parent
    }

    Rectangle {
        id: dialog
        width: VMGlobals.adjustWidth(800)
        height: VMGlobals.adjustHeight(600)
        anchors.centerIn: parent
        radius: VMGlobals.adjustWidth(10);
        color: "#ffffff"
    }

    /////////////////////////////////////////////////////////////////////

    Text {
        id: title
        text: loader.getStringForKey("viewevaluation_resume_binding_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(43)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: dialog.top
        anchors.topMargin: VMGlobals.adjustHeight(20)
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewevaluation_resume_binding_subtitle")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        height: VMGlobals.adjustHeight(18)
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(15)
    }



    VMSortableTable {

        id: bindingStudiesTable
        width: dialog.width*0.86
        height: VMGlobals.adjustHeight(400)
        anchors.horizontalCenter: dialog.horizontalCenter
        anchors.top: subtitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 0.3;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewevaluation_resume_binding_study_date")] = element;

            element = {}
            element["width"]    = 0.15;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewevaluation_resume_binding_target_num")] = element;

            element = {}
            element["width"]    = 0.15;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewevaluation_resume_binding_length")] = element;

            // console.log("Settign up sortable table with: " + JSON.stringify(nameWidthMap));

            bindingStudiesTable.configureTable(nameWidthMap,"")

            // Defining the enabled actions (A custom button and the Edit action) by simply setting the language texts.
            var actions = ["","","",""];
            actions[bindingStudiesTable.vmActionDelete] = " "//loader.getStringForKey("viewevaluation_remove_binding")
            actions[bindingStudiesTable.vmActionButton] = loader.getStringForKey("viewevaluation_resume_binding_continue")
            bindingStudiesTable.vmActionEnabledTexts = actions;
        }


        onDeleteClicked: function (vmIndex){
            console.log("Clicked delete for " + vmIndex)
        }

        onCustomButtonClicked: function(vmIndex) {
            console.log("Clicked custom action for " + vmIndex)
        }

    }

    VMButton {
        id: btnNewBinding
        vmText: loader.getStringForKey("viewevaluation_resume_binding_continue")
        anchors.bottom: dialog.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(10)
        anchors.right: dialog.right
        anchors.rightMargin: VMGlobals.adjustWidth(30)
        onClickSignal: {
        }
    }

}
