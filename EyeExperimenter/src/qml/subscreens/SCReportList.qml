import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenReportList

    readonly property string vmSORT_INDEX_PATIENT: "subject_name"
    readonly property string vmSORT_INDEX_DATE: "order_code"

    readonly property var vmSORT_COLUMNS: [ vmSORT_INDEX_PATIENT, vmSORT_INDEX_DATE, "", "" ];

    readonly property int vmCOL_NAME_INDEX:  0
    readonly property int vmCOL_DATE_INDEX:  1

    property string vmCurrentSortOrder: vmSORT_INDEX_PATIENT
    property string vmCurrentOrderDirection: OLS.ORDER_ASCENDING

    property int vmNumberOfReports : 0

    function convertDateToDisplayDate(dbdate){
        var dateAndTime = dbdate.split(" ");
        if (dateAndTime.length !== 2) return dbdate;
        var time = dateAndTime[1]
        var dateParts = dateAndTime[0].split("/");
        if (dateParts.length !== 3) return dbdate
        var day = dateParts[0]
        var year = dateParts[2]
        var mm = parseInt(dateParts[1]) - 1

        var months = loader.getStringListForKey("viewpatlist_months");

        var month = "";

        if ((mm >= 0) && (mm <= months.length)){
            month = months[mm]
        }
        else return dbdate;

        return day + " " + month + " " + year + " " + time;

    }

    function loadReports(){

        //console.log("Loading evaluation studies");
        OLS.setModelList(loader.getReportsForLoggedEvaluator())
        OLS.sortByIndex(vmCurrentSortOrder,vmCurrentOrderDirection)
        vmNumberOfReports = OLS.getCount()

        if (vmNumberOfReports === 0){
            return;
        }

        var tableTexts = [];

        OLS.setupIteration();
        while (OLS.hasNext()){
            var data = OLS.next();
            var showDate = data["date"];

            // Transforming the date for showing.

            var row = [data["subject_name"],
                       convertDateToDisplayDate(data["date"]),
                       data["type"],
                       //data["medic_name"],
                       Math.round(data["qci"]) + "|" + data["qci_pass"]
                    ];
            tableTexts.push(row);

        }

        reportListTable.setList(tableTexts)
        reportListTable.setSortIndicator(vmSORT_COLUMNS.indexOf(vmCurrentSortOrder),vmCurrentOrderDirection)

        // We now check if this is an enabled instance to know if we need to disabled the buttons.
        if (loader.instanceDisabled()){
            reportListTable.vmArchiveActionEnabled = false;
            reportListTable.vmCustomActionsEnabled = false;
        }
        else {
            reportListTable.vmArchiveActionEnabled = true;
            reportListTable.vmCustomActionsEnabled = true;
        }

    }

    function changeSortColumn(newColumn,newDirection){
        // newColumn is an index and hence it needs to be transformed to the actual key name
        vmCurrentOrderDirection = newDirection
        vmCurrentSortOrder = vmSORT_COLUMNS[newColumn]
        loadReports();
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
        vmShowNumericWheel: [3]

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 230/949;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewevaluation_patient")] = element;

            element = {}
            element["width"] = 173/949;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewevaluation_date")] = element;

            element = {}
            element["width"] = 172/949;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewqc_study")] = element

            element = {}
            element["width"] = 120/949;
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewqc_data_quality")] = element

            // console.log("Settign up sortable table with: " + JSON.stringify(nameWidthMap));

            reportListTable.configureTable(nameWidthMap,loader.getStringForKey("viewpatlist_action"))

            // Defining the enabled actions (A custom button and the Edit action) by simply setting the language texts.
            var actions = ["","","",""];

            actions[reportListTable.vmActionButton] = loader.getStringForKey("viewqc_send")
            actions[reportListTable.vmActionArchive] = loader.getStringForKey("viewqc_archive");
            reportListTable.vmActionEnabledTexts = actions;

        }

        onSortChanged: function(col,order) {
            //console.log("Changeing order to column " + col + " and order " + order)
            changeSortColumn(col,order)
        }

        onCustomButtonClicked: function(vmIndex) {
            callSendOrDiscard(vmIndex,true)
        }

        onArchiveClicked: function (vmIndex) {
            callSendOrDiscard(vmIndex,false)
        }

        function callSendOrDiscard(vmIndex,isSend){

            let data = OLS.getDataAtIndex(vmIndex);

            //            console.log("Getting the data at vmIndex: " + vmIndex)
            //            console.log(JSON.stringify(data));
            //            let studyNameMap = loader.getStudyNameMap()
            //            let study_name = studyNameMap[data.type];

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


}
