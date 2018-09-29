import QtQuick 2.6
import QtQuick.Controls 2.3
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

    property string vmResPAttentional: "0"
    property string vmResPExecutives:  "0"
    property string vmResWorkMem:      "0"
    property string vmResMemRec:       "0"
    property string vmResMemEnc:       "0"

    property int vmPAttentionalResBarPos : 0
    property int vmPExecutivesResBarPos : 0
    property int vmWorkMemResBarPos : 0
    property int vmMemRecResBarPos : 0
    property int vmMemEncResBarPos : 0

    function fillFieldsFromReportInfo(){

        // Values and bar positions
        vmResPAttentional = flowControl.getReportDataField(vmDefines.vmCONFIG_RESULTS_ATTENTIONAL_PROCESSES);
        if (vmResPAttentional !== "N/A"){
            vmPAttentionalResBarPos = flowControl.getReportResultBarPosition(vmDefines.vmCONFIG_RESULTS_ATTENTIONAL_PROCESSES);
        }

        vmResPExecutives = flowControl.getReportDataField(vmDefines.vmCONFIG_RESULTS_EXECUTIVE_PROCESSES);
        if (vmResPExecutives !== "N/A"){
            vmPExecutivesResBarPos = flowControl.getReportResultBarPosition(vmDefines.vmCONFIG_RESULTS_EXECUTIVE_PROCESSES);
        }

        vmResMemEnc = flowControl.getReportDataField(vmDefines.vmCONFIG_RESULTS_MEMORY_ENCODING);
        if (vmResMemEnc !== "N/A"){
            vmMemEncResBarPos = flowControl.getReportResultBarPosition(vmDefines.vmCONFIG_RESULTS_MEMORY_ENCODING);
        }

        vmResMemRec = flowControl.getReportDataField(vmDefines.vmCONFIG_RESULTS_RETRIEVAL_MEMORY);
        if (vmResMemRec !== "N/A"){
            vmMemRecResBarPos = flowControl.getReportResultBarPosition(vmDefines.vmCONFIG_RESULTS_RETRIEVAL_MEMORY);
        }

        vmResWorkMem = flowControl.getReportDataField(vmDefines.vmCONFIG_RESULTS_WORKING_MEMORY);
        if (vmResWorkMem !== "N/A"){
            vmWorkMemResBarPos = flowControl.getReportResultBarPosition(vmDefines.vmCONFIG_RESULTS_WORKING_MEMORY);
        }

        // Patient and doctor data
        vmPatient = flowControl.getReportDataField(vmDefines.vmCONFIG_PATIENT_NAME);
        vmDrName = flowControl.getReportDataField(vmDefines.vmCONFIG_DOCTOR_NAME);
        vmAge = flowControl.getReportDataField(vmDefines.vmCONFIG_PATIENT_AGE);
        vmDate = flowControl.getReportDataField(vmDefines.vmCONFIG_REPORT_DATE);

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
        text: loader.getStringForKey(keysearch+"title");
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
            text: loader.getStringForKey(keysearch+"labelPatient") + ": <b>" + vmPatient + "</b>" ;
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
        height: 473
        anchors.top: rectInfoPatient.top
        anchors.left: rectInfoPatient.right
        anchors.leftMargin: 19

        // ------------ Attentional processes -------------
        Text{
            id: labelAttetionalProcesses
            textFormat: Text.RichText
            color: "#505050"
            text: loader.getStringForKey(keysearch+"labelAttetionalProcesses");
            font.family: gothamM.name
            font.pixelSize: 14
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: parent.top
            anchors.topMargin: 21
        }

        Text{
            id: labelAttetionalProcessesExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelAttetionalProcessesExp");
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.bottom: labelAttetionalProcessesDiv.top
            anchors.bottomMargin: 5
        }

        Rectangle{
            id: labelAttetionalProcessesDiv
            width: 544
            height: 1
            color: "#d3d3d4"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 97
        }

        Text{
            id: labelAttetionalProcessesRes
            textFormat: Text.RichText
            text: vmResPAttentional
            font.family: gothamR.name
            font.pixelSize: 23
            anchors.right: parent.right
            anchors.rightMargin: 31
            y: (labelAttetionalProcessesDiv.y - labelAttetionalProcessesRes.height)/2
        }

        VMResultBar{
            id: labelAttetionalProcessesResBar
            x: 300
            anchors.verticalCenter: labelAttetionalProcessesRes.verticalCenter
            vmIndicatorInSection: vmPAttentionalResBarPos
        }

        // ------------ Executives processes -------------
        Text{
            id: labelExecutivesProcesses
            textFormat: Text.RichText
            color: "#505050"
            text: loader.getStringForKey(keysearch+"labelExecutivesProcesses");
            font.family: gothamM.name
            font.pixelSize: 14
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelAttetionalProcessesDiv.bottom
            anchors.topMargin: 15
        }

        Rectangle{
            id: labelExecutivesProcessesDiv
            width: 544
            height: 1
            color: "#d3d3d4"
            y: labelAttetionalProcessesDiv.y + 94
            anchors.topMargin: 21
        }

        Text{
            id: labelExecutivesProcessesRes
            textFormat: Text.RichText
            text: vmResPExecutives
            font.family: gothamR.name
            font.pixelSize: 23
            anchors.right: parent.right
            anchors.rightMargin: 31
            y: (labelExecutivesProcessesDiv.y + labelAttetionalProcessesDiv.y - labelExecutivesProcessesRes.height)/2
        }

        Text{
            id: labelExecutivesProcessesRef
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelExecutivesProcessesRef")
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelExecutivesProcesses.bottom
            anchors.topMargin: 7
        }

        Text{
            id: labelExecutivesProcessesExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelExecutivesProcessesExp");
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.bottom: labelExecutivesProcessesDiv.top
            anchors.bottomMargin: 5

        }

        VMResultBar{
            id: labelExecutivesProcessesResBar
            x: 300
            anchors.verticalCenter: labelExecutivesProcessesRes.verticalCenter
            vmIndicatorInSection: vmPExecutivesResBarPos
        }

        // ------------ Working Memory -------------
        Text{
            id: labelWorkingMemmory
            textFormat: Text.RichText
            color: "#505050"
            text: loader.getStringForKey(keysearch+"labelWorkingMemmory");
            font.family: gothamM.name
            font.pixelSize: 14
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelExecutivesProcessesDiv.bottom
            anchors.topMargin: 21
        }

        Text{
            id: labelWorkingMemmoryExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelWorkingMemmoryExp");
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.bottom: labelWorkingMemmoryDiv.top
            anchors.bottomMargin: 5

        }

        Rectangle{
            id: labelWorkingMemmoryDiv
            width: 544
            height: 1
            color: "#d3d3d4"
            anchors.horizontalCenter: parent.horizontalCenter
            y: labelExecutivesProcessesDiv.y + 94
        }

        Text{
            id: labelWorkingMemmoryRes
            textFormat: Text.RichText
            text: vmResWorkMem
            font.family: gothamR.name
            font.pixelSize: 23
            anchors.right: parent.right
            anchors.rightMargin: 31
            y: (labelWorkingMemmoryDiv.y + labelExecutivesProcessesDiv.y - labelWorkingMemmoryRes.height)/2
        }

        Text{
            id: labelWorkingMemmoryRef
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelWorkingMemmoryRef")
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelWorkingMemmory.bottom
            anchors.topMargin: 7
        }


        VMResultBar{
            id: labelWorkingMemmoryResBar
            x: 300
            anchors.verticalCenter: labelWorkingMemmoryRes.verticalCenter
            vmIndicatorInSection: vmWorkMemResBarPos
        }

        // ------------ Retrieval memory -------------
        Text{
            id: labelRetrievalMemory
            textFormat: Text.RichText
            color: "#505050"
            text: loader.getStringForKey(keysearch+"labelRetrievalMemory");
            font.family: gothamM.name
            font.pixelSize: 14
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelWorkingMemmoryDiv.bottom
            anchors.topMargin: 21
        }

        Text{
            id: labelRetrievalMemoryExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelRetrievalMemoryExp");
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.bottom: labelRetrievalMemoryDiv.top
            anchors.bottomMargin: 5
        }

        Rectangle{
            id: labelRetrievalMemoryDiv
            width: 544
            height: 1
            color: "#d3d3d4"
            anchors.horizontalCenter: parent.horizontalCenter
            y: labelWorkingMemmoryDiv.y + 94
        }

        Text{
            id: labelRetrievalMemoryRes
            textFormat: Text.RichText
            text: vmResMemRec
            font.family: gothamR.name
            font.pixelSize: 23
            anchors.right: parent.right
            anchors.rightMargin: 31
            y: (labelRetrievalMemoryDiv.y + labelWorkingMemmoryDiv.y - labelRetrievalMemoryRes.height)/2
        }

        Text{
            id: labelRetrievalMemoryRef
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelRetrievalMemoryRef")
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelRetrievalMemory.bottom
            anchors.topMargin: 7
        }

        VMResultBar{
            id: labelRetrievalMemoryResBar
            x: 300
            anchors.verticalCenter: labelRetrievalMemoryRes.verticalCenter
            vmIndicatorInSection: vmMemRecResBarPos
        }

        // ------------ Visual short term memory -------------
        Text{
            id: labelVisualShortTermMemory
            textFormat: Text.RichText
            color: "#505050"
            text: loader.getStringForKey(keysearch+"labelVisualShortTermMemory");
            font.family: gothamM.name
            font.pixelSize: 14
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelRetrievalMemoryDiv.bottom
            anchors.topMargin: 15
        }

        Text{
            id: labelVisualShortTermMemoryExp
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelVisualShortTermMemoryExp");
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

        }

        Text{
            id: labelVisualShortTermMemoryRes
            textFormat: Text.RichText
            text: vmResMemEnc
            font.family: gothamR.name
            font.pixelSize: 23
            anchors.right: parent.right
            anchors.rightMargin: 31
            y: (parent.height + labelRetrievalMemoryDiv.y - labelVisualShortTermMemoryRes.height)/2
        }


        Text{
            id: labelVisualShortTermMemoryRef
            textFormat: Text.RichText
            color: "#737577"
            text: loader.getStringForKey(keysearch+"labelVisualShortTermMemoryRef")
            font.family: robotoR.name
            font.pixelSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.top: labelVisualShortTermMemory.bottom
            anchors.topMargin: 7
        }

        VMResultBar{
            id: labelVisualShortTermMemoryResBar
            vmTwoSection: true
            x: 300
            anchors.verticalCenter: labelVisualShortTermMemoryRes.verticalCenter
            vmIndicatorInSection: vmMemEncResBarPos
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
            swiperControl.currentIndex = swiperControl.vmIndexPatientList;
        }
    }

}
