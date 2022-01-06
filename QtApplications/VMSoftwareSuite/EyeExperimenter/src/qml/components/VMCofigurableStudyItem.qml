import QtQuick
import "../"

Rectangle {

    // Variables when setting as a list view delegate
    //    vmIndex           The study code.
    //    vmIsLastSelected: True if it's a selected study that his last on the list. So that we know where to draw the divisor.
    //    vmStudyName       String for the study name
    //    vmOptions         Object. Each key contains a string list. The key is the option name. The String list are the radio button text

    id: configurableStudyItem
    height: (internal.vmIsExpanded && studyCheckBox.vmIsOn) ? internal.vmHEIGHT_EXPANDED : internal.vmHEIGHT
    color: studyCheckBox.vmIsOn ? VMGlobals.vmBlueBackgroundHighlight : VMGlobals.vmWhite
    border.color: VMGlobals.vmGrayUnselectedBorder
    border.width: VMGlobals.adjustHeight(1)

    signal selectionChanged(int index, bool isSelected)
    signal updateSelectedOptions(int vmIndex, var options)

    function getOptionsSelected(){
        var optionsSelected = {}
        for (var i = 0; i < optionSetRepeater.count; i++ ){
            var keyname = internal.vmKeyByIndex[i]
            var keyvalue = optionSetRepeater.itemAt(i).vmSelection
            optionsSelected[keyname] = keyvalue
        }
        return optionsSelected
    }

    function sendUpdatedOptions(){
        updateSelectedOptions(vmIndex,getOptionsSelected())
    }

    function toggleExpansion(){
        internal.vmIsExpanded = !internal.vmIsExpanded
        expandIndicator.requestPaint()
    }

    function setExpansionState(expanded){
        internal.vmIsExpanded = expanded
        expandIndicator.requestPaint();
    }


    QtObject {
        id: internal
        readonly property int vmHEIGHT : VMGlobals.adjustHeight(52)
        readonly property int vmHEIGHT_EXPANDED : VMGlobals.adjustHeight(52+64)

        readonly property double vmOPTION_H_SPACING: VMGlobals.adjustWidth(30)

        property bool vmCanBeExpanded: false

        property bool vmIsExpanded: false;

        property var vmKeyByIndex: [];

    }

    VMCheckBox {
        id: studyCheckBox
        vmText: vmStudyName
        //vmIsOn: vmIsSelected
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin:  VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(15)
        onVmIsOnChanged: {
            //console.log(vmStudyName  + " check state change to " + vmIsOn)
            if (vmIsOn && internal.vmCanBeExpanded) setExpansionState(true)
            //else setExpansionState(false)
            selectionChanged(vmIndex,vmIsOn)
        }
    }

    MouseArea {

        id: maExpandIndicator
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: VMGlobals.adjustWidth(19.38)
        anchors.topMargin: VMGlobals.adjustHeight(22.25)
        height: VMGlobals.adjustHeight(7)
        width: height*12/7
        enabled: internal.vmCanBeExpanded && studyCheckBox.vmIsOn
        onClicked: {
            toggleExpansion()
        }

        Canvas {
            id: expandIndicator
            anchors.fill: parent
            contextType: "2d"
            visible: maExpandIndicator.enabled
            onPaint: {

                // Using round line caps extends the line past the point of finish by 1 measure of line with.
                // Hence we need to compute the midpoints.
                var lw = VMGlobals.adjustHeight(1.5)
                var midw = (width-2*lw)/2 +lw
                var midh = (height-2*lw)/2 +lw
                var fw   = (width-2*lw) + lw
                var fh   = (height-2*lw) + lw

                var ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = VMGlobals.vmBlackText
                ctx.lineWidth = lw
                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                if (internal.vmIsExpanded){
                    ctx.moveTo(lw,fh)
                    ctx.lineTo(midw,lw)
                    ctx.lineTo(fw,fh)
                }
                else {
                    ctx.moveTo(lw,lw)
                    ctx.lineTo(midw,fh)
                    ctx.lineTo(fw,lw)
                }
                ctx.stroke();

            }
        }

    }

    ListModel {
        id: optionSetList
    }

    Rectangle {
        id: lastDivisor
        width: parent.width
        anchors.bottom: parent.bottom
        color: VMGlobals.vmGrayStudyDivisor
        height: VMGlobals.adjustHeight(1)
        visible: studyCheckBox.vmIsOn && vmIsLastSelected
    }

    Row {
        id: optionRow
        spacing: internal.vmOPTION_H_SPACING
        visible: internal.vmIsExpanded && studyCheckBox.vmIsOn
        anchors.top: studyCheckBox.bottom
        anchors.topMargin: VMGlobals.adjustHeight(17)
        anchors.left: studyCheckBox.left
        anchors.leftMargin: VMGlobals.adjustWidth(35)

        Repeater {
            id: optionSetRepeater
            model: optionSetList

            Item {
                id: optionSetItem

                property string vmSelection: ""

                Text {
                    id: name
                    text: optionSetList.get(index).text
                    font.weight: 400
                    font.pixelSize: VMGlobals.vmFontBaseSize
                    height: VMGlobals.adjustHeight(18)
                    verticalAlignment: Text.AlignVCenter
                }
                VMSingleChoiceMultiRadioButton{
                    id: optionSelection
                    anchors.left: name.left
                    anchors.top: name.bottom
                    anchors.topMargin: VMGlobals.adjustHeight(10.5)
                    Component.onCompleted: {
                        //console.log(JSON.stringify(vmOptions[index]))
                        let keyname = internal.vmKeyByIndex[index]
                        let options = vmOptions[keyname][VMGlobals.vmSCO_OPTION_VAlUES]
                        // WARNING: Notice that the default option here is loaded as an index, but when saved, it will always be saved as text.
                        let default_option = vmOptions[keyname][VMGlobals.vmSCO_OPTION_SELECTED]
                        optionSelection.setOptions(options)
                        optionSelection.setSelection(default_option)
                    }
                    onCurrentSelectionChanged: {
                        // Send an update to keep track of the options.
                        vmSelection = optionSelection.getSelectedText()
                        sendUpdatedOptions()
                    }
                }
            }
        }

    }


    Component.onCompleted: {
        //console.log("Options have been set to " + JSON.stringify(vmOptions));
        internal.vmCanBeExpanded = (vmOptions !== undefined)
        //console.log("Is vmOptions NOT undefined " + internal.vmCanBeExpanded)
        if (!internal.vmCanBeExpanded) return;
        //console.log(JSON.stringify(vmOptions))

        optionSetList.clear();
        internal.vmKeyByIndex = [];
        for (var keyname in vmOptions){
            let name = vmOptions[keyname][VMGlobals.vmSCO_OPTION_NAME]
            internal.vmKeyByIndex.push(keyname);
            optionSetList.append({text: name})
        }
    }

}
