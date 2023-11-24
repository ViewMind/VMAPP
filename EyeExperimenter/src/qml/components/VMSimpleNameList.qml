import QtQuick
import QtQuick.Controls
import "."
import "../"

Rectangle {

    id: simpleNameList

    ListModel {
        id: model
    }


    function setList(list){
        model.clear()
        for (var i = 0; i < list.length; i++){
            var element = list[i];
            element["vmIndex"] = i
            model.append(element)
        }
    }

    ListView {
        id: listNameView
        clip: true
        anchors.fill: parent
        model: model

        ScrollBar.vertical: ScrollBar {
            active: true
        }

        delegate: Rectangle {
            id: nameItem
            width: listNameView.width
            height: VMGlobals.adjustHeight(50)
            color: {
                if (itemMouseArea.containsMouse) return VMGlobals.vmWhiteButtonHiglight
                //else return (vmIndex % 2) ? VMGlobals.vmWhite : VMGlobals.vmGrayToggleOff
                else VMGlobals.vmWhite
            }
            border.width: VMGlobals.adjustWidth(1)
            border.color:  VMGlobals.vmGrayUnselectedBorder
            radius: VMGlobals.adjustHeight(2)

            MouseArea {
                id: itemMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onContainsMouseChanged: {

                }
                onClicked: {
                    console.log("Clicked on item index: " + vmIndex + " And ID: " + vmSubjectID);
                }
            }

            Text {
                id: patientName
                text: (vmPersonalID === "") ?  "<b>" + vmName + '</b>' :  "<b>" + vmName + '</b> - (<i>' + vmPersonalID + '</i>)'
                color: VMGlobals.vmBlackText
                font.pixelSize: VMGlobals.vmFontLarge
                font.weight: 400
                verticalAlignment: Text.AlignVCenter
                height: parent.height*0.5
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: VMGlobals.adjustWidth(10);
            }

            Text {
                id: patientDateOfBirth
                text: vmBirthDate
                font.pixelSize: VMGlobals.vmFontBaseSize
                font.weight: 400
                font.italic: true;
                color: VMGlobals.vmGrayAccented
                verticalAlignment: Text.AlignVCenter
                height: patientName.height
                anchors.left: patientName.left
                anchors.top: patientName.bottom
            }

        }

        onCurrentIndexChanged: {
        }
    }

}
