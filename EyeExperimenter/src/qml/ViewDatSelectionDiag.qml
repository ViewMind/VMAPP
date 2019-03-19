import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewselectdata_"
    id: viewSelectDatForReport
    modal: true
    width: 700
    height: 600
    closePolicy: Popup.NoAutoClose

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    onOpened: {
        var list = [];
        for (var i = 0; i < 10; i++){
            list.push("READING ITEM " + i);
        }
        reading.setModelList(list)
    }

    // The select report title
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"labelTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 70
    }

    // The subtitle with the explanation
    Text {
        id: subTitle
        font.pixelSize: 11
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"labelSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: 11
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewSelectDatForReport.close();
        }
    }

    VMDatSelection{
        id: reading;
        vmTitle: "Reading"
        vmPlaceHolderText: "Select an option"
        width: 400
        height: 80
        //vmEnabled: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: 20
    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        spacing: 30

        VMButton{
            id: btnBack
            height: 50
            width: 160
            vmText: loader.getStringForKey(keybase+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {

            }
        }

        VMButton{
            id: btnGenerate
            height: 50
            width: 160
            vmText: loader.getStringForKey(keybase+"btnGenReport");
            vmFont: viewHome.gothamM.name
            enabled: false
            onClicked: {
            }
        }
    }


}
