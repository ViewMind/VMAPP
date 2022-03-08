import QtQuick
import "./"
import "../"

Rectangle {

    id: singleChoiceMultipleRadioButton
    color: "transparent"

    height: {
        if (repeater.count > 0){
            return repeater.itemAt(0).height
        }
        else return 0
    }
    width: {
        var w = 0;
        for (var i = 0; i < repeater.count; i++){
            w = w + repeater.itemAt(i).width
        }
        w = w + vmSpacing*(repeater.count-1);
        return w;
    }

    signal currentSelectionChanged()

    property double vmSpacing: VMGlobals.adjustWidth(30)

    ListModel {
        id: options
    }

    function settingChanged(index){
        for (var i = 0; i < repeater.count; i++){
            if (i !== index) repeater.itemAt(i).vmIsSelected = false
        }
        currentSelectionChanged()
    }

    function setSelection(index){
        for (var i = 0; i < repeater.count; i++){
            if (i !== index) repeater.itemAt(i).vmIsSelected = false
            else repeater.itemAt(i).vmIsSelected = true
        }
    }

    function getSelectedIndex(){
        for (var i = 0; i < repeater.count; i++){
            if (repeater.itemAt(i).vmIsSelected) return i;
        }
        return -1;
    }

    function getSelectedText(){
        let i = getSelectedIndex();
        if (i === -1) return ""
        else return options.get(i).text
    }

    function setOptions(stringList){
        options.clear()
        for (var i = 0; i < stringList.length; i++){
            options.append({ text: stringList[i] })
        }
    }

    Row {

        spacing: vmSpacing

        Repeater {

            id: repeater
            model: options

            Rectangle {

                height: radioButton.height
                width: radioButton.width
                color: "transparent"

                property alias vmIsSelected: radioButton.vmIsOn

                VMRadioButton {
                    id: radioButton
                    vmText: options.get(index).text
                    anchors.top: parent.top
                    anchors.left: parent.left
                    onVmIsOnChanged: {
                        if (vmIsOn) settingChanged(index)
                    }
                }

            }

        }

    }

}
