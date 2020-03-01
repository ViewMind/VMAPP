import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

VMBase {

    id: viewResults

    readonly property string keysearch: "viewresults_"

    property string vmPatient: ""
    property string vmAge: ""
    property string vmDrName: ""
    property string vmDate: ""
    property string vmTitleDemo: ""

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
                map.vmDiagClassTitle = ""
                map.vmDisclaimer = ""
                map.vmDiagnosisClass = ""
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

        //console.log("Diag data")
        //console.log(diagData)

        var diagEntry;
        diagEntry = {
            "vmTitleText": "",
            "vmExpText" : "",
            "vmRefText" : "",
            "vmResValue" : "",
            "vmResBarIndicator": "",
            "vmDisclaimer" : diagData[0],
            "vmDiagnosisClass" : diagData[1],
            "vmDiagClassTitle" : diagData[2]
        }
        resultsList.append(diagEntry)

        //console.log(vmDisclaimer)
        //console.log(vmDiagnosisClass)
        //console.log(vmDiagClassTitle)

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
        width: mainWindow.width*0.48
        height: mainWindow.height*0.87
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
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.128
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"diagWaitTitle")
        }

        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.top:  diagTitle.bottom
            anchors.topMargin: mainWindow.height*0.038
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: loader.getStringForKey(keysearch+"diagWaitMessage")
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: mainWindow.height*0.043
            x: (parent.width - slideAnimation.width)/2;
            scale: viewHome.vmScale
        }

    }

    Text{
        id: title
        text: loader.getStringForKey(keysearch+"title") + vmTitleDemo;
        color: "#297FCA"
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.117
        anchors.top: vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.007
    }

    // Patient info rectangle
    Rectangle{
        id: rectInfoPatient
        color: "#EDEDEE"
        radius: 2
        width: mainWindow.width*0.301
        height: mainWindow.height*0.3
        anchors.top: title.bottom
        anchors.topMargin: mainWindow.height*0.007
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
            font.pixelSize: 13*viewHome.vmScale
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.013
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.038
        }

        Rectangle{
            id: labelPatientDiv
            color: "#d3d3d4"
            width: mainWindow.width*0.276
            height: mainWindow.height*0.001
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelPatient.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

        // Age
        Text{
            id: labelAge
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelAge") + ": <b>" + vmAge + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.013
            anchors.top: labelPatientDiv.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

        Rectangle{
            id: labelAgeDiv
            color: "#d3d3d4"
            width: mainWindow.width*0.276
            height: mainWindow.height*0.001
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelAge.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

        // Dr. Name
        Text{
            id: labelDrName
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelDrName") + ": <b>" + vmDrName + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.013
            anchors.top: labelAgeDiv.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

        Rectangle{
            id: labelDrNameDiv
            color: "#d3d3d4"
            width: mainWindow.width*0.276
            height: mainWindow.height*0.001
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelDrName.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

        // Date
        Text{
            id: labelDate
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelDate") + ": <b>" + vmDate + "</b>" ;
            font.family: robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.013
            anchors.top: labelDrNameDiv.bottom
            anchors.topMargin: mainWindow.height*0.023
        }

    }

    // Result type rectangle
    Rectangle{
        id: rectInfoCodeColor
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#ededee"
        radius: 2
        width: mainWindow.width*0.301
        height: mainWindow.height*0.358
        anchors.top: rectInfoPatient.bottom
        anchors.topMargin: mainWindow.height*0.028
        anchors.left: title.left

        Text{
            id: labelExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelExp");
            font.family: robotoR.name
            font.pixelSize: 12*viewHome.vmScale
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.023
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.042
        }

        // Green rect and text
        Rectangle{
            id: rectGreen
            width: mainWindow.width*0.012
            height: mainWindow.height*0.022
            color: "#009949"
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.023
            anchors.top: labelExp.bottom
            anchors.topMargin: mainWindow.height*0.017
        }

        Text{
            id: labelSquareGreen
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareGreen");
            font.family: robotoR.name
            font.pixelSize: 12*viewHome.vmScale
            anchors.left: rectGreen.right
            anchors.leftMargin: mainWindow.width*0.008
            anchors.top: rectGreen.top
        }

        // Yellow rect and text
        Rectangle{
            id: rectYellow
            width: mainWindow.width*0.012
            height: mainWindow.height*0.022
            color: "#e4b32c"
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.023
            anchors.top: rectGreen.bottom
            anchors.topMargin: mainWindow.height*0.026
        }

        Text{
            id: labelSquareYellow
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareYellow");
            font.family: robotoR.name
            font.pixelSize: 12*viewHome.vmScale
            anchors.left: rectYellow.right
            anchors.leftMargin: mainWindow.width*0.008
            anchors.top: rectYellow.top
        }

        Rectangle{
            id: rectRed
            width: mainWindow.width*0.012
            height: mainWindow.height*0.022
            color: "#ca2026"
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.023
            anchors.top: rectYellow.bottom
            anchors.topMargin: mainWindow.height*0.032
        }

        Text{
            id: labelSquareRed
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelSquareRed");
            font.family: robotoR.name
            font.pixelSize: 12*viewHome.vmScale
            anchors.left: rectRed.right
            anchors.leftMargin: mainWindow.width*0.008
            anchors.top: rectRed.top
        }

    }

    // Results rectangle
    Rectangle{
        id: rectResults
        color: "#ffffff"
        border.width: mainWindow.width*0.002
        border.color: "#ededee"
        radius: 2
        width: mainWindow.width*0.45
        height: rectInfoPatient.height + rectInfoCodeColor.height + rectInfoCodeColor.anchors.topMargin
        anchors.top: rectInfoPatient.top
        anchors.left: rectInfoPatient.right
        anchors.leftMargin: mainWindow.width*0.015

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

        } // Scroll view end

    }

    VMButton{
        id: btnDownload
        vmText:  loader.getStringForKey(keysearch+"btnDownload");
        vmInvertColors: true
        vmSize: [mainWindow.width*0.094,mainWindow.height*0.072]
        vmFont: gothamM.name
        anchors.left: parent.left
        anchors.leftMargin: mainWindow.width*0.393
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.014
        onClicked: {
            flowControl.saveReportAs(loader.getStringForKey(keysearch+"save_report"));
        }
    }


    VMButton{
        id: btnFinalize
        vmText:  loader.getStringForKey(keysearch+"btnFinalize");
        vmSize: [mainWindow.width*0.094,mainWindow.height*0.072]
        vmFont: gothamM.name
        anchors.left: btnDownload.right
        anchors.leftMargin: mainWindow.width*0.014
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.014
        onClicked: {
            swiperControl.currentIndex = swiperControl.vmIndexShowReports;
        }
    }

}
