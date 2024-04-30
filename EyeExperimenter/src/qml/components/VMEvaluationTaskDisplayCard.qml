import QtQuick
import QtQuick.Controls
import "../"


Rectangle {

    color: (own.vmErrorModeText === "") ? VMGlobals.vmBlueVeryLightBkg : VMGlobals.vmNotificationRed;
    radius: VMGlobals.adjustWidth(10)

    Item {
        id: own
        property string vmTitle: ""
        property string vmErrorModeText: ""
        readonly property int vmSideMargins: VMGlobals.adjustWidth(20)
        readonly property int vmLineH: VMGlobals.adjustHeight(32)
    }

    ListModel {
        id: items;
    }

    function configure(title, list, errormode){

        if ((errormode === undefined) || (errormode === "")){
            own.vmErrorModeText = "";
        }
        else {
            own.vmErrorModeText = errormode;
        }

        own.vmTitle = title;
        items.clear()
        for (let i = 0; i < list.length; i++){
            items.append({"text" : list[i] })
        }

        if (own.vmErrorModeText !== "") {
            height = own.vmLineH*items.count +
                    displayTitle.height + displayTitle.anchors.topMargin +
                    divider.height + divider.anchors.topMargin +
                    errorModeText.anchors.topMargin + errorModeText.height
            height = height + VMGlobals.adjustHeight(10)
        }
        else {
            height = own.vmLineH*items.count +
                    displayTitle.height + displayTitle.anchors.topMargin +
                    divider.height + divider.anchors.topMargin +
                    bulletedList.anchors.topMargin
            height = height + VMGlobals.adjustHeight(20)
        }



    }

    Text {
        id: displayTitle
        color: (own.vmErrorModeText === "") ? VMGlobals.vmBlueSelected : VMGlobals.vmWhite;
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarge
        text: own.vmTitle
        //height: VMGlobals.adjustHeight(32)
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: parent.left
        anchors.leftMargin: own.vmSideMargins
    }

    Rectangle {
        id: divider
        width: parent.width - 2*own.vmSideMargins
        height: VMGlobals.adjustHeight(1)
        anchors.top: displayTitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.horizontalCenter: parent.horizontalCenter
        border.width: 0
        color: VMGlobals.vmGrayDialogDivider
    }

    Column {

        id: bulletedList
        spacing: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: divider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(5)
        width: parent.width
        height: own.vmLineH*items.count
        visible: (own.vmErrorModeText === "")

        Repeater {

            id: repeater
            model: items
            anchors.fill: parent

            Item {
                id: lineItem
                height: own.vmLineH
                width: parent.width

                Rectangle {
                    id: bullet
                    height: 0.2*own.vmLineH
                    width: height
                    anchors.left: parent.left
                    anchors.leftMargin: own.vmSideMargins + own.vmSideMargins;
                    color: VMGlobals.vmBlackText
                    anchors.verticalCenter: lineItem.verticalCenter
                    radius: width/2
                }

                Text {
                    id: text
                    text: {
                        if (items.get(index) === undefined) return "";
                        else return items.get(index).text
                    }
                    color: VMGlobals.vmBlackText
                    font.pixelSize: VMGlobals.vmFontLarge
                    font.weight: 400
                    anchors.left: bullet.right
                    anchors.leftMargin: VMGlobals.adjustWidth(5)
                    anchors.verticalCenter: lineItem.verticalCenter
                }

            }
        }

    }


    Text {
        id: errorModeText
        text: own.vmErrorModeText
        color: VMGlobals.vmWhite;
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: (own.vmErrorModeText !== "")
    }

}
