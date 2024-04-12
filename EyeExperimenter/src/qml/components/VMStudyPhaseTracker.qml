import QtQuick
import QtQuick.Controls
import "../"


Rectangle {

    id: phaseTracker
    color: "transparent"

    property int vmCurrentIndex: 0
    property string vmEvalTitle: ""
    property string vmEvalNumber: ""

    ListModel {
        id: items;
    }

    // The setup only requires an item list.

    function setup(textList){
        items.clear();
        for (let i in textList){
            items.append({"text" : textList[i]})
        }
        vmCurrentIndex = 0;
    }

    function reset(){
        vmCurrentIndex = 0;
    }

    function indicateNext(){
        if (vmCurrentIndex < (items.count - 1))
            vmCurrentIndex++;
    }

    function getCurrentText(){
        if (items.count > 1){
            if (vmCurrentIndex < items.count){
                return items.get(vmCurrentIndex).text;
            }
        }
        return "";
    }


    Text {
        id: title
        text: vmEvalNumber  + ". " + vmEvalTitle
        wrapMode: Text.Wrap
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(32)
        width: parent.width
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.left: parent.left
    }

    Rectangle {
        id: emptyProgressBar
        width: parent.width
        height: VMGlobals.adjustHeight(6)
        anchors.left: title.left
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        color: VMGlobals.vmGrayPlaceholderText
    }

    Rectangle {
        id: fillProgress
        color: VMGlobals.vmBlueSelected
        height: emptyProgressBar.height
        anchors.top: emptyProgressBar.top
        anchors.left: emptyProgressBar.left
        width: {
            let total = items.count;
            if (total === 0) return 0;
            return (vmCurrentIndex + 1)*emptyProgressBar.width/total
        }
    }

    Column {

        spacing: emptyProgressBar.anchors.topMargin
        anchors.left: parent.left
        anchors.top: emptyProgressBar.bottom
        anchors.topMargin: emptyProgressBar.anchors.topMargin

        Repeater {

            id: repeater
            model: items
            anchors.fill: parent

            Item {
                id: textAndcheck
                height: text.height
                width: phaseTracker.width

                VMCheckMarkCircle {
                    id: check
                    width: text.height
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    vmCheckState: {
                        if (vmCurrentIndex === index) return check.vmCHECK_CURRENT
                        if (vmCurrentIndex < index) return check.vmCHECK_NOT_DONE
                        return check.vmCHECK_DONE
                    }
                }

                Text {
                    id: text
                    text: {
                        // We do this because when clearing the items before resetting them, it attemps to get an index which is not defined
                        // generating errors in the console.
                        if (items.get(index) === undefined) return ""
                        else items.get(index).text
                    }
                    color: (check.vmCheckState === check.vmCHECK_CURRENT) ? VMGlobals.vmBlueSelected : VMGlobals.vmGrayPlaceholderText
                    font.pixelSize: VMGlobals.vmFontBaseSize
                    font.weight: check.vmCheckIsVisble ? 600 : 400
                    verticalAlignment: Text.AlignVCenter
                    width: textAndcheck.width
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: check.right
                    anchors.leftMargin: VMGlobals.adjustWidth(6)
                }

            }
        }

    }



}
