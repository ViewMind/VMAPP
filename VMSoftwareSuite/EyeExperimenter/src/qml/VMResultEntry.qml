import QtQuick 2.0

Item {

    id: vmResultEntry
    width: 576
    height: (vmDiagClassTitle == "")? 105 : 180

    //        width: 576
    //        height: 180

    /////////////////////////////// Normal Results

    Text{
        id: titleText
        textFormat: Text.RichText
        color: "#505050"
        text: vmTitleText
        font.family: viewHome.gothamM.name
        font.pixelSize: 14*viewHome.vmScale
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
        //textFormat: Text.RichText
        color: "#737577"
        text: vmExpText
        font.family: viewHome.robotoR.name
        font.pixelSize: 11*viewHome.vmScale
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
        font.pixelSize: 11*viewHome.vmScale
        anchors.bottom: itemDivision.top
        anchors.bottomMargin: 10
        anchors.left: titleText.left
    }

    Text{
        id: resultValue
        textFormat: Text.RichText
        text: vmResValue
        font.family: viewHome.gothamR.name
        font.pixelSize: 23*viewHome.vmScale
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

    /////////////////////////////// Diagnosis class rectangle
    Rectangle{
        id: rectDiagClass
        color: "#dfebf7"
        border.width: 2
        border.color: "#dfebf7"
        radius: 2
        width: 576
        height: 180
        anchors.fill: parent
        visible: vmDiagClassTitle !== ""

        Text{
            id: diagClassTitle
            color: "#367bb6"
            text: vmDiagClassTitle;
            font.family: gothamB.name
            font.pixelSize: 15*viewHome.vmScale
            anchors.left: diagClassDivider.left
            anchors.top: parent.top
            anchors.topMargin: 15
        }

        Text{
            id: diagClassText
            color: "#737577"
            text: vmDiagnosisClass;
            font.family: gothamB.name
            font.pixelSize: 12*viewHome.vmScale
            font.bold: true
            wrapMode: Text.WordWrap
            width: 0.9*parent.width
            anchors.left: diagClassTitle.left
            anchors.top: diagClassTitle.bottom
            anchors.topMargin: 6
        }

        Rectangle {
            id: diagClassDivider
            width: 0.9*parent.width
            height: 2;
            color: "#737577"
            border.color: "#737577"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: diagClassText.bottom
            anchors.topMargin: 10
        }

        Text{
            id: disclaimer
            color: "#737577"
            text: vmDisclaimer;
            font.family: robotoR.name
            font.pixelSize: 11*viewHome.vmScale
            font.italic: true
            wrapMode: Text.WordWrap
            width: 0.9*parent.width
            anchors.left: diagClassTitle.left
            anchors.top: diagClassDivider.bottom
            anchors.topMargin: 10
        }

    }

}
