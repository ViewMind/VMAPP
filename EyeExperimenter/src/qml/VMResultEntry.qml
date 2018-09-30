import QtQuick 2.0

Item {

    id: vmResultEntry
    width: 576
    height: 95

    Text{
        id: titleText
        textFormat: Text.RichText
        color: "#505050"
        text: vmTitleText
        font.family: viewHome.gothamM.name
        font.pixelSize: 14
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16
        anchors.topMargin: 14
    }

    Rectangle{
        id: itemDivision
        width: parent.width*0.95
        height: 1
        color: "#d3d3d4"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }

    Text{
        id: explanationText
        textFormat: Text.RichText
        color: "#737577"
        text: vmExpText
        font.family: viewHome.robotoR.name
        font.pixelSize: 11
        visible: (vmExpText !== "")
        anchors.top: titleText.bottom
        anchors.topMargin: 5
        anchors.left: titleText.left
    }


    Text{
        id: referenceText
        textFormat: Text.RichText
        text: vmRefText
        color: "#737577"
        font.family: viewHome.robotoR.name
        font.pixelSize: 11
        anchors.bottom: itemDivision.top
        anchors.bottomMargin: 10
        anchors.left: titleText.left
    }

    Text{
        id: resultValue
        textFormat: Text.RichText
        text: vmResValue
        font.family: viewHome.gothamR.name
        font.pixelSize: 23
        anchors.right: parent.right
        anchors.rightMargin: 31
        anchors.verticalCenter: parent.verticalCenter
    }

    VMResultBar{
        id: resultBar
        x: 300
        vmTwoSection: vmHasTwoSections
        anchors.verticalCenter: parent.verticalCenter
        vmIndicatorInSection: vmResBarIndicator
    }


}
