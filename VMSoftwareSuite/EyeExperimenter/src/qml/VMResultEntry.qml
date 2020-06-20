import QtQuick 2.0

Item {

    id: vmResultEntry
    width: mainWindow.width*0.45
    height: (vmDiagClassTitle == "")? mainWindow.height*0.152 : mainWindow.height*0.261

    readonly property var vmBarColorArray2: ["#009949","#ca2026"]
    readonly property var vmBarColorArray3: ["#009949","#e4b32c","#ca2026"]
    readonly property var vmBarColorArray4: ["#00557f", "#009949","#e4b32c","#ca2026"]

    //        width: mainWindow.width*0.45
    //        height: mainWindow.height*0.261

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
        anchors.leftMargin: mainWindow.width*0.013
        anchors.topMargin: mainWindow.height*0.02
    }

    Rectangle{
        id: itemDivision
        width: parent.width*0.95
        height: mainWindow.height*0.001
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
        anchors.topMargin: mainWindow.height*0.007
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
        anchors.bottomMargin: mainWindow.height*0.014
        anchors.left: titleText.left
    }

    Text{
        id: resultValue
        textFormat: Text.RichText
        text: vmResValue
        font.family: viewHome.gothamR.name
        font.pixelSize: 23*viewHome.vmScale
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.024
        anchors.verticalCenter: parent.verticalCenter
    }

    VMResultBar{
        id: resultBar
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.12
        anchors.verticalCenter: parent.verticalCenter
        vmColorArray: {
            switch (vmNumOfSegements){
                case 2: return vmBarColorArray2;
                case 3: return vmBarColorArray3;
                case 4: return vmBarColorArray4;
            }
        }
        vmIndicator: vmResBarIndicator

    }

    /////////////////////////////// Diagnosis class rectangle
    Rectangle{
        id: rectDiagClass
        color: "#dfebf7"
        border.width: mainWindow.width*0.002
        border.color: "#dfebf7"
        radius: 2
        width: mainWindow.width*0.45
        height: mainWindow.height*0.261
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
            anchors.topMargin: mainWindow.height*0.022
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
            anchors.topMargin: mainWindow.height*0.009
        }

        Rectangle {
            id: diagClassDivider
            width: 0.9*parent.width
            height: 2;
            color: "#737577"
            border.color: "#737577"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: diagClassText.bottom
            anchors.topMargin: mainWindow.height*0.014
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
            anchors.topMargin: mainWindow.height*0.014
        }

    }

}
