import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenPatList

    readonly property string vmSORT_INDEX_SUBJECT: "sortable_name"
    readonly property string vmSORT_INDEX_BDATE: "birthdate"
    readonly property string vmSORT_INDEX_ID: "supplied_institution_id"

    readonly property var vmSORT_COLUMNS: [ vmSORT_INDEX_SUBJECT, vmSORT_INDEX_BDATE, "", vmSORT_INDEX_ID ];

    readonly property int vmCOL_NAME_INDEX: 0
    readonly property int vmCOL_BDATE_INDEX: 1
    readonly property int vmCOL_ID_INDEX: 3

    property string vmCurrentSortOrder: vmSORT_INDEX_SUBJECT
    property string vmCurrentOrderDirection: OLS.ORDER_ASCENDING
    property int vmNumberOfPatients: 0
    property bool vmNoPatientsAtAll: false;
    property bool vmStudiesEnabled: true;


    function loadPatients(){

        if (vmNoPatientsAtAll) return;

        var filterText = searchInput.vmCurrentText

        OLS.setModelList(loader.filterSubjectList(filterText))
        OLS.sortByIndex(vmCurrentSortOrder,vmCurrentOrderDirection)
        vmNumberOfPatients = OLS.getCount()

        var tableTexts = [];

        OLS.setupIteration();
        while (OLS.hasNext()){
            var data = OLS.next();
            var row = [data["sortable_name"],
                       data["bdate_display"],
                       data["gender"],
                       data["supplied_institution_id"] ];
            tableTexts.push(row);
        }

        //console.log("Studies enable is equal to " + vmStudiesEnabled)
        patListTable.vmCustomActionsEnabled = vmStudiesEnabled

        patListTable.setList(tableTexts)
        patListTable.setSortIndicator(vmSORT_COLUMNS.indexOf(vmCurrentSortOrder),vmCurrentOrderDirection)

    }

    function changeSortColumn(newColumn,newDirection){
        // newColumn is an index and hence it needs to be transformed to the actual key name
        vmCurrentOrderDirection = newDirection
        vmCurrentSortOrder = vmSORT_COLUMNS[newColumn]
        loadPatients();
    }

    Text {
        id: title
        text: loader.getStringForKey("viewpatlist_title")
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(31)
        visible: !vmNoPatientsAtAll
    }

    Text {
        id: patientCount
        text: "(" + vmNumberOfPatients + ")"
        color: VMGlobals.vmGrayAccented
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        height: title.height
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: title.verticalCenter
        anchors.left: title.right
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        visible: !vmNoPatientsAtAll
    }

    VMButton {
        id: addPatient
        vmText: loader.getStringForKey("viewpatlist_addpatient")
        vmButtonType: addPatient.vmTypeSecondary
        vmIconSource: "plus"
        anchors.verticalCenter: title.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(32)
        visible: !vmNoPatientsAtAll
        onClickSignal: {
            viewAddPatient.clear()
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddPatient);
        }
    }

    VMSearchInput {
        id: searchInput
        vmPlaceHolderText: loader.getStringForKey("viewpatlist_search")
        width: VMGlobals.adjustWidth(911)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: addPatient.bottom
        anchors.topMargin: VMGlobals.adjustHeight(30)
        visible: !vmNoPatientsAtAll
        onTextChanged: {
            if (searchInput.vmCurrentText.length >= 3){
                loadPatients();
            }
        }
    }

    VMSortableTable {

        id: patListTable
        width: parent.width - subScreenPatList.border.width*2
        height: VMGlobals.adjustHeight(500)
        anchors.bottom: parent.bottom
        x: subScreenPatList.border.width
        visible: !vmNoPatientsAtAll

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 327/969;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewpatlist_name")] = element;

            element = {}
            element["width"] = (61+85)/969;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewpatlist_bdate")] = element;

            element = {}
            element["width"] = (15+85)/969; // <- Modified so that new buttons don't have to be designed
            element["sortable"] = false;
            nameWidthMap[loader.getStringForKey("viewpatlist_sex")] = element

            element = {}
            element["width"] = (130)/969;    // <- Modified so that new buttons don't have to be designed
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewpatlist_id")] = element

            // console.log("Settign up sortable table with: " + JSON.stringify(nameWidthMap));

            patListTable.configureTable(nameWidthMap,loader.getStringForKey("viewpatlist_action"))

            // Defining the enabled actions (A custom button and the Edit action) by simply setting the language texts.
            var actions = ["","",""];
            actions[patListTable.vmActionButton] = loader.getStringForKey("viewpatlist_study")
            actions[patListTable.vmActionEdit] = loader.getStringForKey("viewpatlist_edit")
            patListTable.vmActionEnabledTexts = actions;
        }

        onSortChanged: function(col,order) {
            //console.log("Changeing order to column " + col + " and order " + order)
            changeSortColumn(col,order)
        }

        onEditClicked: function (vmIndex) {
            var data = OLS.getDataAtIndex(vmIndex)
            viewAddPatient.loadPatient(data)
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddPatient)
        }

        onCustomButtonClicked: function(vmIndex) {
            loader.setSelectedSubject(OLS.getKeyAtIndex(vmIndex))
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexEvalView)
        }

    }

    Rectangle {
        id: noPatientsFoundScreen
        anchors.fill: patListTable
        visible: (vmNumberOfPatients == 0) && (!vmNoPatientsAtAll)

        Image {
            id: nothingFoundImage
            height: VMGlobals.adjustHeight(150)
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/search.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: noResultsTitle.top
            anchors.bottomMargin: VMGlobals.adjustHeight(20)
        }

        Text {
            id: noResultsTitle
            text: loader.getStringForKey("viewpatlist_nothing_found");
            font.pixelSize: VMGlobals.vmFontExtraExtraLarge
            font.weight: 600
            height: VMGlobals.adjustHeight(43)
            verticalAlignment: Text.AlignVCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: noResultsSubTitle.top
            anchors.bottomMargin: VMGlobals.adjustHeight(9)
        }

        Text {
            id: noResultsSubTitle
            text: loader.getStringForKey("viewpatlist_nothing_found_exp");
            color: VMGlobals.vmGrayPlaceholderText
            font.pixelSize: VMGlobals.vmFontLarge
            font.weight: 400
            height: VMGlobals.adjustHeight(21)
            verticalAlignment: Text.AlignVCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: VMGlobals.adjustHeight(139)
        }
    }

    Image {
        id: image
        source: "qrc:/images/docuemnts.png"
        fillMode: Image.PreserveAspectFit
        height: VMGlobals.adjustHeight(150)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(153)
        visible: vmNoPatientsAtAll
    }

    Text {
        id: titleForNoPatients
        text: loader.getStringForKey("viewpatlist_title")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: image.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        visible: vmNoPatientsAtAll
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("viewpatlist_nopats")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleForNoPatients.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: vmNoPatientsAtAll
    }


    VMButton {
        vmText: loader.getStringForKey("viewpatlist_addpatient")
        vmIconSource: "plus"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(200)
        visible: vmNoPatientsAtAll
        onClickSignal: {
            viewAddPatient.clear()
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexAddPatient);
        }
    }


}
