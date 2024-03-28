import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenReportList

    property int vmNumberOfReports : 0

    function loadReports(){

        //console.log("Loading evaluation studies");
        OLS.setModelList(loader.getReportsForLoggedEvaluator())
        OLS.sortByIndex("timestamp",OLS.ORDER_DESCENDING)
        vmNumberOfReports = OLS.getCount()

        if (vmNumberOfReports === 0){
            return;
        }

        var tableTexts = [];

        OLS.setupIteration();
        while (OLS.hasNext()){
            var data = OLS.next();

            let color = VMGlobals.vmRedError
            if (data["status"] === "ongoing"){
                color = VMGlobals.vmGrayAccented;
            }
            else if (data["status"] === "ready_upload"){
                color = VMGlobals.vmGreenSolidQCIndicator
            }

            var row = [
                        data["subject"],
                        data["eval_type"],
                        data["clinician"],
                        data["time"],
                        color
                    ];

            tableTexts.push(row);

        }

        //console.log("Setting Up Reports Table with:\n" + JSON.stringify(tableTexts,null,2));

        reportListTable.setList(tableTexts)

        // We now check if this is an enabled instance to know if we need to disabled the buttons.
        reportListTable.vmArchiveActionEnabled = false;
        reportListTable.vmCustomActionsEnabled = true;


    }

    Text {
        id: title
        text: loader.getStringForKey("viewpatlist_qc")
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(31)
        visible: vmNumberOfReports !== 0
    }

    Text {
        id: reportCount
        text: "(" + vmNumberOfReports + ")"
        color: VMGlobals.vmGrayAccented
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        height: title.height
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: title.verticalCenter
        anchors.left: title.right
        anchors.leftMargin: VMGlobals.adjustWidth(10)
        visible: vmNumberOfReports !== 0
    }

    VMSortableTable {

        id: reportListTable
        width: parent.width - subScreenReportList.border.width*2
        height: VMGlobals.adjustHeight(580)
        anchors.bottom: parent.bottom
        x: subScreenReportList.border.width
        visible: vmNumberOfReports !== 0
        vmShowColorCircle: [4]

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 0.3;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewevaluation_patient")] = element;

            element = {}
            element["width"] = 0.15;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewongoing_eval")] = element;

            element = {}
            element["width"] = 0.15;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewongoing_clinician")] = element

            element = {}
            element["width"] = 0.17;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewevaluation_date")] = element

            element = {}
            element["width"] = 0.08;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewongoing_status")] = element


            // console.log("Settign up sortable table with: " + JSON.stringify(nameWidthMap));

            var tooltips = {};
            tooltips[loader.getStringForKey("viewongoing_status")] = loader.getStringForKey("viewqc_data_quality_tooltip")

            reportListTable.configureTable(nameWidthMap,"",tooltips);

            // Defining the enabled actions (A custom button and the Edit action) by simply setting the language texts.
            var actions = ["","","",""];

            actions[reportListTable.vmActionButton] = loader.getStringForKey("viewongoing_review")
            reportListTable.vmActionEnabledTexts = actions;

        }

        onCustomButtonClicked: function(vmIndex) {
            let data = OLS.getDataAtIndex(vmIndex);
            mainWindow.openEvaluationTaskDialog(data.id)
        }


        function callSendOrDiscard(vmIndex,isSend){

            let data = OLS.getDataAtIndex(vmIndex);

            //            console.log("Getting the data at vmIndex: " + vmIndex)
            //            console.log(JSON.stringify(data));

            // Getting the configuration data for the QC View.
            viewQC.configurePatientInformation(data.subject_name,
                                               data.subject_insitution_id,
                                               convertDateToDisplayDate(data.date),
                                               data.type, isSend)

            loader.setCurrentStudyFileToSendOrDiscard(data.file);

            mainWindow.swipeTo(VMGlobals.vmSwipeIndexQCView)

        }

    }


    Image {
        id: image
        source: "qrc:/images/reports.png"
        fillMode: Image.PreserveAspectFit
        height: VMGlobals.adjustHeight(150)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(153)
        visible: vmNumberOfReports === 0
    }

    Text {
        id: titleForNoReports
        text: loader.getStringForKey("viewpatlist_qc")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: image.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        visible: vmNumberOfReports === 0
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewqc_no_reports_to_evaluate")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleForNoReports.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: vmNumberOfReports === 0
    }


    VMButton {
        id: btnGoBackToPatientsList
        vmText: loader.getStringForKey("viewpatlist_title").toUpperCase();
        anchors.horizontalCenter: subtitle.horizontalCenter
        anchors.top: subtitle.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: vmNumberOfReports === 0
        onClickSignal: {
            viewMainSetup.goBackToPatientList();
        }
    }


}
