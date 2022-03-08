import QtQuick
import QtQuick.Controls
import "../"

Rectangle {

    property var vmItems: [];
    property int vmCurrentIndex: 0;
    property bool vmLikeButton: false;

    readonly property double vmSpacing: VMGlobals.adjustHeight(5);

    function setItems(list){
        elementList.clear();
        for (let i = 0; i < list.length; i++){
            var element = {};
            element["vmSelected"] = false;
            element["vmIndex"] = i;
            element["vmIconSelected"] = list[i]["icon_selected"];
            element["vmIconNormal"] = list[i]["icon_normal"];
            element["vmText"] = list[i]["text"];
            //console.log("SIDE NAVIGATOR: Appending: " + list[i]["text"])
            elementList.append(element)
        }

    }


    function setItemSelected(index){
        for (var i = 0; i < elementList.count; i++){
            if (i !== index) elementList.setProperty(i,"vmSelected",false);
            else elementList.setProperty(i,"vmSelected",true);
        }
        vmCurrentIndex = index
    }


    id: sideNavPanel

    color: "transparent"
    border.width: 0
    height: elementList.count*VMGlobals.vmControlsHeight

    ListModel {
        id: elementList
    }

    ListView {
        id: list
        anchors.fill: parent
        model: elementList
        delegate: Rectangle {

            id: listItem
            height: VMGlobals.vmControlsHeight
            width: parent.width
            color: vmSelected ? VMGlobals.vmBluePanelBKGSelected : "transparent"
            radius: VMGlobals.adjustWidth(10)
            border.width: 0

            readonly property double fontSize: VMGlobals.vmFontBaseSize

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true;
                onClicked: {
                    setItemSelected(vmIndex)
                }
            }

            Text {
                id: text
                text: vmText
                font.pixelSize: vmLikeButton? VMGlobals.vmFontLarge : VMGlobals.vmFontBaseSize
                font.weight: 600
                color: vmSelected ? VMGlobals.vmBlueSelected : VMGlobals.vmBlackText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: icon.right
                anchors.leftMargin: VMGlobals.adjustWidth(10)
            }

            Image {
                id: icon
                height: vmLikeButton ? VMGlobals.adjustHeight(16) : VMGlobals.adjustHeight(18)
                fillMode: Image.PreserveAspectFit
                source: vmSelected ? vmIconSelected : vmIconNormal
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin:  vmLikeButton? VMGlobals.adjustWidth(20) : VMGlobals.adjustWidth(13);
            }

        }


    }

}
