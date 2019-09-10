import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

VMBase {

    id: viewResults
    width: viewHome.vmWIDTH
    height: viewHome.vmHEIGHT

    readonly property string keysearch: "viewresults_"

    property string vmPatient: ""
    property string vmAge: ""
    property string vmDrName: ""
    property string vmDate: ""
    property string vmTitleDemo: ""
    property string vmDisclaimer: ""
    property string vmDiagnosisClass: ""
    property string vmDiagClassTitle: ""

    readonly property string vmDEMO_DATE:             "03/06/2010 10:15"
    readonly property string vmDEMO_DATE_FROM_REPORT: "03/06/2010 10:15"

    ListModel{
        id: resultsList
    }

    function fillFieldsFromReportInfo(){

        // Preparing all the items that should go on the list.
        resultsList.clear();
        flowControl.prepareSelectedReportIteration();

        var done = false;
        while (!done){
            var map = flowControl.nextSelectedReportItem();
            done = !("vmTitleText" in map);
            if (!done){
                //                console.log("================= APPENDING =====================");
                //                for (var key in map){
                //                    console.log("   " + key + ": " + map[key]);
                //                }
                resultsList.append(map);
            }
        }

        // Patient and doctor data
        var list  = flowControl.getSelectedReportInfo();
        var freqErr = false;
        vmPatient = list[1];
        vmDrName  = list[0];
        vmAge     = list[2];
        vmDate    = list[3];
        freqErr   = list[4];

        if (vmDate === vmDEMO_DATE_FROM_REPORT){
            vmTitleDemo = " (DEMO)";
        }
        else vmTitleDemo = "";

        if (freqErr === "true"){
            viewResults.vmErrorDiag.vmErrorCode = viewHome.vmErrorDiag.vmERROR_FREQ_ERR;
            var titleMsg = viewHome.getErrorTitleAndMessage("error_freq_check");
            viewResults.vmErrorDiag.vmErrorMessage = titleMsg[1];
            viewResults.vmErrorDiag.vmErrorTitle = titleMsg[0];
            viewResults.vmErrorDiag.open();
            return;
        }

        var diagData = flowControl.getDiagnosticClass();

        vmDisclaimer     = diagData[0];
        vmDiagnosisClass = diagData[1];
        vmDiagClassTitle = diagData[2];

    }

    Connections{
        target: flowControl
        onReportGenerationRequested: {
            reportDialog.open();
            flowControl.saveReport();
        }
        onReportGenerationDone: {
            reportDialog.close();
        }
    }

    Dialog {

        id: reportDialog;
        modal: true
        width: 614
        height: 600
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The instruction text
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 88
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"diagWaitTitle")
        }

        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            anchors.top:  diagTitle.bottom
            anchors.topMargin: 26
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"diagWaitMessage")
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: 30
            x: (parent.width - slideAnimation.width)/2;
        }

    }

    Text{
        id: title
        text: loader.getStringForKey(keysearch+"title") + vmTitleDemo;
        color: "#297FCA"
        font.family: gothamB.name
        font.pixelSize: 43
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.leftMargin: 150
        anchors.top: vmBanner.bottom
        anchors.topMargin: 5
    }

    // Patient info rectangle
    Rectangle{
        id: rectInfoPatient
        color: "#EDEDEE"
        radius: 2
        width: 385
        height: 207
        anchors.top: title.bottom
        anchors.topMargin: 5
        anchors.left: title.left

        // Patient
        Text{
            id: labelPatient
            textFormat: Text.RichText
            color: "#737577"
            text: {
                if (uimap.getStructure() === "P") return loader.getStringForKey(keysearch+"labelPatient") + ": <b>" + vmPatient + "</b>" ;
                if (uimap.getStructure() === "S") return loader.getStringForKey(keysearch+"labelSubject") + ": <b>" + vmPatient + "</b>" ;
            }
            font.family: robotoR.name
            font.pixelSize: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: parent.top
            anchors.topMargin: 26
        }

        Rectangle{
            id: labelPatientDiv
            color: "#d3d3d4"
            width: 353
            height: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelPatient.bottom
            anchors.topMargin: 16
        }

        // Age
        Text{
            id: labelAge
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelAge") + ": <b>" + vmAge + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelPatientDiv.bottom
            anchors.topMargin: 16
        }

        Rectangle{
            id: labelAgeDiv
            color: "#d3d3d4"
            width: 353
            height: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelAge.bottom
            anchors.topMargin: 16
        }

        // Dr. Name
        Text{
            id: labelDrName
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelDrName") + ": <b>" + vmDrName + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelAgeDiv.bottom
            anchors.topMargin: 16
        }

        Rectangle{
            id: labelDrNameDiv
            color: "#d3d3d4"
            width: 353
            height: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelDrName.bottom
            anchors.topMargin: 16
        }

        // Date
        Text{
            id: labelDate
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelDate") + ": <b>" + vmDate + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelDrNameDiv.bottom
            anchors.topMargin: 16
        }

    }

    // Result type rectangle
    Rectangle{
        id: rectInfoCodeColor
        color: "#ffffff"
        border.width: 2
        border.color: "#ededee"
        radius: 2
        width: 385
        height: 247
        anchors.top: rectInfoPatient.bottom
        anchors.topMargin: 19
        anchors.left: title.left

        Text{
            id: labelExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelExp");
            font.family: robotoR.name
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: parent.top
            anchors.topMargin: 29
        }

        // Green rect and text
        Rectangle{
            id: rectGreen
            width: 15
            height: 15
            color: "#009949"
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: labelExp.bottom
            anchors.topMargin: 12
        }

        Text{
            id: labelSquareGreen
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareGreen");
            font.family: robotoR.name
            font.pixelSize: 12
            anchors.left: rectGreen.right
            anchors.leftMargin: 10
            anchors.top: rectGreen.top
        }

        // Yellow rect and text
        Rectangle{
            id: rectYellow
            width: 15
            height: 15
            color: "#e4b32c"
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: rectGreen.bottom
            anchors.topMargin: 18
        }

        Text{
            id: labelSquareYellow
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareYellow");
            font.family: robotoR.name
            font.pixelSize: 12
            anchors.left: rectYellow.right
            anchors.leftMargin: 10
            anchors.top: rectYellow.top
        }

        Rectangle{
            id: rectRed
            width: 15
            height: 15
            color: "#ca2026"
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: rectYellow.bottom
            anchors.topMargin: 22
        }

        Text{
            id: labelSquareRed
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareRed");
            font.family: robotoR.name
            font.pixelSize: 12
            anchors.left: rectRed.right
            anchors.leftMargin: 10
            anchors.top: rectRed.top
        }

    }

    // Results rectangle
    Rectangle{
        id: rectResults
        color: "#ffffff"
        border.width: 2
        border.color: "#ededee"
        radius: 2
        width: 576
        height: 273
        anchors.top: rectInfoPatient.top
        anchors.left: rectInfoPatient.right
        anchors.leftMargin: 19

        ScrollView {
            id: resultsArea
            anchors.fill: parent
            clip: true
            verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn
            ListView {
                id: resultsListView
                anchors.fill: parent
                model: resultsList
                delegate: VMResultEntry {
                }
            }
        }

    }

    // Diagnosis class rectangle
    Rectangle{
        id: rectDiagClass
        color: "#dfebf7"
        border.width: 2
        border.color: "#dfebf7"
        radius: 2
        width: 576
        height: 180
        anchors.left: rectResults.left
        anchors.top: rectResults.bottom
        anchors.topMargin: 20

        Text{
            id: diagClassTitle
            color: "#367bb6"
            text: vmDiagClassTitle;
            font.family: gothamB.name
            font.pixelSize: 15
            anchors.left: diagClassDivider.left
            anchors.top: parent.top
            anchors.topMargin: 15
        }

        Text{
            id: diagClassText
            color: "#737577"
            text: vmDiagnosisClass;
            font.family: gothamB.name
            font.pixelSize: 12
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
            font.pixelSize: 11
            font.italic: true
            wrapMode: Text.WordWrap
            width: 0.9*parent.width
            anchors.left: diagClassTitle.left
            anchors.top: diagClassDivider.bottom
            anchors.topMargin: 10
        }

    }

    VMButton{
        id: btnDownload
        vmText:  loader.getStringForKey(keysearch+"btnDownload");
        vmInvertColors: true
        vmSize: [121,49]
        vmFont: gothamM.name
        anchors.left: parent.left
        anchors.leftMargin: 503
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onClicked: {
            flowControl.saveReportAs(loader.getStringForKey(keysearch+"save_report"));
        }
    }


    VMButton{
        id: btnFinalize
        vmText:  loader.getStringForKey(keysearch+"btnFinalize");
        vmSize: [121,49]
        vmFont: gothamM.name
        anchors.left: btnDownload.right
        anchors.leftMargin: 18
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexShowReports;
        }
    }

}
