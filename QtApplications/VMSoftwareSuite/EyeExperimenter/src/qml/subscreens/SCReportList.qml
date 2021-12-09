import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenReportList

    Connections {
        target: loader
        function onQualityControlDone () {

            // Close the wait message.
            mainWindow.closeWait()

            if (loader.qualityControlFailed()){
                mainWindow.popUpNotify(VMGlobals.vmNotificationRed,loader.getStringForKey("viewqc_qc_failed"));
                return;
            }

            mainWindow.swipeTo(VMGlobals.vmSwipeIndexQCView)

            viewQC.loadProgressLine();

        }
    }

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
            viewMainSetup.showNoReports();
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
                       data["medic_name"] ];
            tableTexts.push(row);

        }

        reportListTable.setList(tableTexts)
        reportListTable.setSortIndicator(vmSORT_COLUMNS.indexOf(vmCurrentSortOrder),vmCurrentOrderDirection)

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
    }


    VMSortableTable {

        id: reportListTable
        width: parent.width - subScreenReportList.border.width*2
        height: VMGlobals.adjustHeight(580)
        anchors.bottom: parent.bottom
        x: subScreenReportList.border.width

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 239/949;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewevaluation_patient")] = element;

            element = {}
            element["width"] = (193)/949;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewevaluation_date")] = element;

            element = {}
            element["width"] = (112)/949;  // <- Modified so that new buttons don't have to be designed
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewqc_study")] = element

            element = {}
            element["width"] = (47+124)/949;   // <- Modified so that new buttons don't have to be designed
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewqc_dr")] = element

            // console.log("Settign up sortable table with: " + JSON.stringify(nameWidthMap));

            reportListTable.configureTable(nameWidthMap,loader.getStringForKey("viewpatlist_action"))

            // Defining the enabled actions (A custom button) by simply setting the language texts.
            var actions = ["","",""];
            actions[reportListTable.vmActionButton] = loader.getStringForKey("viewpatlist_qc").toUpperCase()
            reportListTable.vmActionEnabledTexts = actions;
        }

        onSortChanged: function(col,order) {
            //console.log("Changeing order to column " + col + " and order " + order)
            changeSortColumn(col,order)
        }

        onCustomButtonClicked: function(vmIndex) {
            //console.log("Should start QC fo the report at list position " + OLS.getDataAtIndex(vmIndex).file_path)
            mainWindow.openWait(loader.getStringForKey("viewqc_wait_msg"))
            //console.log("Data of selected report " + JSON.stringify(OLS.getDataAtIndex(vmIndex)));

            let data = OLS.getDataAtIndex(vmIndex);

            // Getting the configuration data for the QC View.
            viewQC.configurePatientInformation(data.subject_name,
                                               data.subject_insitution_id,
                                               convertDateToDisplayDate(data.date))

            loader.setCurrentStudyFileForQC(data.file_path);
        }

    }



}
