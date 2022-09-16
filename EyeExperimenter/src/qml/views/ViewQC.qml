import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"
import "../components"
import "../subscreens"

ViewBase {

    id: qcView

    readonly property int vmMAX_L_COMMMENT_LINES: 19
    readonly property int vmMAX_C_COMMENT_CHARS: 92

    readonly property int vmSC_INDEX_QCGRAPHS:       0
    readonly property int vmSC_INDEX_SEND_REPORT:    1
    readonly property int vmSC_INDEX_ARCHIVE:        2

    function configurePatientInformation(name, id, reportdate){
        var patient = loader.getStringForKey("viewevaluation_patient") + ",<br>";
        patient = patient + "<b>" + name + "</b>";
        //console.log("Setting patient to " + patient)
        patientText.text = patient;
        personalIDValue.text = id;
        dateValue.text = reportdate
        viewer.currentIndex = vmSC_INDEX_QCGRAPHS
    }



    function loadProgressLine(){

        // Selecting the different types of graphps.
        let graphNames = [
                loader.getStringForKey("viewqc_SubICI"),
                loader.getStringForKey("viewqc_SubFixations")
            ];


        // Creating the study name language map
        var nameList = loader.getStringListForKey("viewQC_StudyNameMap");
        let vmStudyNameMap = [];
        var key;
        var i = 0;
        for (i = 0; i < nameList.length; i++){
            var name = nameList[i].replace("\n","");
            if ((i % 2) === 0){
                // This is a key:
                key = name;
            }
            else{
                // This is a value.
                vmStudyNameMap[key] = name;
            }
        }

        // Getting the study list.
        var studies = loader.getStudyList();

        // Setting up the progress line.
        let plineSetup = {};
        for (i in studies){
            let name = vmStudyNameMap[studies[i]]
            plineSetup[name] = graphNames;
        }

        // Adding the "Send Evaluations" step. No substeps.
        plineSetup[loader.getStringForKey("view_qc_send_report")] = []

        progressLine.vmOnlyColorCurrent = true;
        progressLine.setup(plineSetup);
        progressLine.reset();

        let evalNameAndGraph = progressLine.getCurrentTexts()
        qcgraphs.setStudyAndGraphName(evalNameAndGraph[0],evalNameAndGraph[1])

        // Initializing and loading the first graph.
        qcgraphs.initializeForGraphTraversal();

        // Making sure the previous button is visible
        // previousButton.visible = true;

    }

    function moveProgressLine(advance){
        if (advance)  progressLine.indicateNext();
        else progressLine.indicatePrevious();
        return progressLine.getCurrentTexts();
    }

    VMButton {
        id: backButton
        vmText: loader.getStringForKey("viewlogin_back")
        vmIconSource: "arrow"
        vmButtonType: backButton.vmTypeTertiary
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(592)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        onClickSignal: {
            switch (viewer.currentIndex){
            case vmSC_INDEX_QCGRAPHS:
                mainWindow.swipeTo(VMGlobals.vmSwipeIndexMainScreen)
                break;
            default:
                loadProgressLine()
                viewer.setCurrentIndex(vmSC_INDEX_QCGRAPHS)
                break;
            }
        }
    }


    Text {
        id: patientText
        font.pixelSize: VMGlobals.vmFontVeryLarge
        font.weight: 400
        visible: viewer.currentIndex != vmSC_INDEX_ARCHIVE
        /// VERY VERY IMPORTANT. Qt BUG: So here is what happens. When I developed this rich text formatting got disabled if
        /// before I set the text string a context form a Canvas object set a font. The work around I found is to simply 'have'
        /// HTML text, BEFORE the Canvas does it's operations and then when over writing the text, it shows up correctly.
        /// I managed to build a MWE (which I have since deleted). Steps to reproduce:
        ///    Create a Text QML object with "<b>Bold</b><br>Not Bold" (call it id: text)
        ///    Create a Canvas and on the onPaint method, get the canvas (ctx variable) and do cxt.font = "10px sans-serif"
        ///    Create a button. On the onClicked Method of the button set the text.text = "<b>More Bold</b><br>More Not Bold"
        ///    This will work.
        ///    Now simply comment the text property in the id: text object and re run.
        ///    When pressing the button, the text will appear but it won't be in bold.
        text: "Patient,<br><b>patientname</b>"
        textFormat: Text.RichText
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(64)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(511)
        anchors.leftMargin: VMGlobals.adjustWidth(15)
    }

    Row {
        id: personalIDRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: patientText.bottom
        anchors.left: patientText.left
        anchors.topMargin: VMGlobals.adjustHeight(14)
        spacing: VMGlobals.adjustWidth(5)
        visible: viewer.currentIndex != vmSC_INDEX_ARCHIVE

        Text {
            id: personalIDName
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: loader.getStringForKey("viewpatlist_id") + ":"
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: personalIDValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Row {
        id: dateRow
        height: VMGlobals.adjustHeight(20)
        anchors.top: personalIDRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        anchors.left: patientText.left
        spacing: VMGlobals.adjustWidth(5)
        visible: viewer.currentIndex != vmSC_INDEX_ARCHIVE

        Text {
            id: dateName
            text: loader.getStringForKey("viewevaluation_date") + ":"
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
        Text {
            id: dateValue
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            color: VMGlobals.vmBlackText
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
        }
    }

    Rectangle {
        property double vmBorderPadding: VMGlobals.adjustWidth(1)
        //property double vmBorderPadding: VMGlobals.adjustWidth(10)
        id: mainRect
        clip: true
        radius: VMGlobals.adjustHeight(10)
        width: VMGlobals.adjustWidth(969);
        height: VMGlobals.adjustHeight(670+10-84);
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(15)
        anchors.bottom: horizontalDivider.top
        anchors.bottomMargin: -radius
        border.width: vmBorderPadding
        border.color: VMGlobals.vmGrayUnselectedBorder

        SwipeView {
            id: viewer
            interactive: false
            currentIndex: 0
            width: mainRect.width - mainRect.vmBorderPadding*2
            height: mainRect.height - mainRect.radius - mainRect.vmBorderPadding
            x: mainRect.vmBorderPadding
            y: mainRect.vmBorderPadding

            Item {
                SCQCGraphs {
                    id: qcgraphs
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    //anchors.fill: parent
                    width: parent.width
                    height: parent.height + radius
                }
            }

            Item {
                SCSendReport {
                    id: sendReport
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    //border.color: "#ff0000"
                    width: parent.width
                    height: parent.height + radius
                }
            }

            Item {
                SCArchive {
                    id: archiveReport
                    radius: mainRect.radius
                    border.width:  mainRect.border.width
                    border.color: mainRect.border.color
                    width: parent.width
                    height: parent.height + radius
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex === vmSC_INDEX_ARCHIVE) archiveReport.reset()
                else if (currentIndex === vmSC_INDEX_SEND_REPORT) sendReport.reset();
            }

        }


    }

    Rectangle {
        id: horizontalDivider
        height: VMGlobals.adjustHeight(84-1)
        anchors.right: mainRect.right
        anchors.bottom: parent.bottom
        width: mainRect.width
        border.width: VMGlobals.adjustHeight(1)
        border.color: VMGlobals.vmGrayUnselectedBorder

        VMButton {
            id: nextButton
            vmText: qcgraphs.vmIsLastGraph ? loader.getStringForKey("viewqc_send") : loader.getStringForKey("viewevaluation_next_button")
            vmIconSource: qcgraphs.vmIsLastGraph ? "" : "next"
            vmIconToTheRight: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            visible: (viewer.currentIndex != vmSC_INDEX_ARCHIVE)
            onClickSignal: {
                switch(viewer.currentIndex){
                case vmSC_INDEX_QCGRAPHS:
                    if (qcgraphs.vmIsLastGraph){
                        moveProgressLine(true)
                        viewer.setCurrentIndex(vmSC_INDEX_SEND_REPORT)
                    }
                    else {
                        qcgraphs.moveGraph(true)
                    }
                    break;
                case vmSC_INDEX_SEND_REPORT:
                    sendReport.sendToProcess();
                    break;
                }

            }
        }

        VMButton {
            id: confirmArchiveButton
            vmText: loader.getStringForKey("viewqc_confirm_archive")
            visible: (viewer.currentIndex === vmSC_INDEX_ARCHIVE)
            vmButtonType: confirmArchiveButton.vmTypeWarning
            vmIconSource: "qrc:/images/archive-white.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: VMGlobals.adjustWidth(29)
            onClickSignal: {
                archiveReport.archive();
            }
        }

        VMButton {
            id: archiveButton
            vmText:loader.getStringForKey("viewqc_archive")
            vmButtonType: archiveButton.vmTypeSecondary
            //vmIconToTheRight: false
            vmIconSource: "qrc:/images/archive.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: nextButton.left
            anchors.rightMargin: VMGlobals.adjustWidth(20)
            visible: (qcgraphs.vmIsLastGraph && (viewer.currentIndex == vmSC_INDEX_QCGRAPHS))
            onClickSignal: {
                viewer.setCurrentIndex(vmSC_INDEX_ARCHIVE)
            }
        }

        VMButton {
            id: previousButton
            vmText: loader.getStringForKey("viewqc_previous")
            vmIconSource: "next"
            vmIconToTheRight: false
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: VMGlobals.adjustWidth(29)
            visible: (viewer.currentIndex === vmSC_INDEX_QCGRAPHS)
            onClickSignal: {
                qcgraphs.moveGraph(false)
            }
        }

    }

    VMProgressLine {
        id: progressLine
        anchors.top: dateRow.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(15)
        visible: (viewer.currentIndex !== vmSC_INDEX_ARCHIVE)
    }

}
