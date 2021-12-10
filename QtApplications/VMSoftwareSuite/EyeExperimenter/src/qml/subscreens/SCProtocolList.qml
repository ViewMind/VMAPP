import QtQuick
import "../"
import "../components"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: subScreenProtocolList

    readonly property string vmSORT_INDEX_PNAME: "protocol_name"
    readonly property string vmSORT_INDEX_DATE:  "creation_date"
    readonly property string vmSORT_INDEX_ID:    "protocol_id"

    readonly property var vmSORT_COLUMNS: [ vmSORT_INDEX_PNAME, vmSORT_INDEX_ID, vmSORT_INDEX_DATE ];

    readonly property int vmCOL_NAME_INDEX:  0
    readonly property int vmCOL_ID_INDEX:    1
    readonly property int vmCOL_DATE_INDEX:  2

    property string vmCurrentSortOrder: vmSORT_INDEX_PNAME
    property string vmCurrentOrderDirection: OLS.ORDER_ASCENDING
    property int vmNumberOfProtocols: 0


    function loadProtocols(){

        OLS.setModelList(loader.getProtocolList())
        OLS.sortByIndex(vmCurrentSortOrder,vmCurrentOrderDirection)
        vmNumberOfProtocols = OLS.getCount()

        if (vmNumberOfProtocols === 0){
            return;
        }

        var tableTexts = [];

        OLS.setupIteration();
        while (OLS.hasNext()){
            var data = OLS.next();
            var row = [data[vmSORT_INDEX_PNAME],
                       data[vmSORT_INDEX_ID],
                       data[vmSORT_INDEX_DATE]];
            tableTexts.push(row);
        }

        protocolTable.setList(tableTexts)
        protocolTable.setSortIndicator(vmSORT_COLUMNS.indexOf(vmCurrentSortOrder),vmCurrentOrderDirection)

    }

    function changeSortColumn(newColumn,newDirection){
        // newColumn is an index and hence it needs to be transformed to the actual key name
        vmCurrentOrderDirection = newDirection
        vmCurrentSortOrder = vmSORT_COLUMNS[newColumn]
        loadProtocols();
    }

    Text {
        id: title
        text: loader.getStringForKey("viewpatlist_protocol")
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 600
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: VMGlobals.adjustWidth(30)
        anchors.topMargin: VMGlobals.adjustHeight(31)
        visible: vmNumberOfProtocols != 0
    }

    Text {
        id: protocolCount
        text: "(" + vmNumberOfProtocols + ")"
        color: VMGlobals.vmGrayAccented
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        height: title.height
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: title.verticalCenter
        anchors.left: title.right
        anchors.leftMargin: VMGlobals.adjustWidth(5)
        visible: vmNumberOfProtocols != 0
    }

    VMButton {
        id: addProtocol
        vmText: loader.getStringForKey("protocol_add").toUpperCase()
        vmButtonType: addProtocol.vmTypeSecondary
        vmIconSource: "plus"
        anchors.verticalCenter: title.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(32)
        visible: vmNumberOfProtocols != 0
        onClickSignal: {
            viewEditProtocol.clear();
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexEditProtocol);
        }
    }

    VMSortableTable {

        id: protocolTable
        width: parent.width - subScreenProtocolList.border.width*2
        height: VMGlobals.adjustHeight(555)
        anchors.bottom: parent.bottom
        x: subScreenProtocolList.border.width
        visible: vmNumberOfProtocols != 0

        Component.onCompleted: {

            var nameWidthMap = {}

            // Configuring the column names and their table widths.
            let element = {};

            element = {}
            element["width"]    = 347/971;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("viewpatlist_name")] = element;

            element = {}
            element["width"] = (77+81)/971;
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("protocol_id")] = element;

            element = {}
            element["width"] = (94+80)/971; // <- Modified so that new buttons don't have to be designed
            element["sortable"] = true;
            nameWidthMap[loader.getStringForKey("protocol_creation_date")] = element

            protocolTable.configureTable(nameWidthMap,loader.getStringForKey("viewpatlist_action"))

            // Defining the enabled actions (A custom button and the Edit action) by simply setting the language texts.
            var actions = ["","",""];
            actions[protocolTable.vmActionEdit] = loader.getStringForKey("viewpatlist_edit")
            actions[protocolTable.vmActionDelete] = loader.getStringForKey("protocol_delete")
            protocolTable.vmActionEnabledTexts = actions;
        }

        onSortChanged: function(col,order) {
            //console.log("Changeing order to column " + col + " and order " + order)
            changeSortColumn(col,order)
        }

        onEditClicked: function (vmIndex) {
            var data = OLS.getDataAtIndex(vmIndex)
            viewEditProtocol.loadProtocol(data[vmSORT_INDEX_ID],data[vmSORT_INDEX_PNAME])
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexEditProtocol);
        }

        onDeleteClicked: function (vmIndex) {
            loader.deleteProtocol(OLS.getKeyAtIndex(vmIndex))
            loadProtocols();
        }

    }


    Image {
        id: image
        source: "qrc:/images/protocol.png"
        fillMode: Image.PreserveAspectFit
        height: VMGlobals.adjustHeight(150)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(153)
        visible: vmNumberOfProtocols == 0;
    }

    Text {
        id: titleForNoProtocols
        text: loader.getStringForKey("viewpatlist_protocol")
        font.pixelSize: VMGlobals.vmFontExtraExtraLarge
        font.weight: 400
        color: VMGlobals.vmBlackText
        height: VMGlobals.adjustHeight(42)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: image.bottom
        anchors.topMargin: VMGlobals.adjustHeight(20)
        visible: vmNumberOfProtocols == 0;
    }

    Text {
        id: subtitle
        text: loader.getStringForKey("protocol_no_protocols")
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 400
        color: VMGlobals.vmGrayPlaceholderText
        height: VMGlobals.adjustHeight(21)
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleForNoProtocols.bottom
        anchors.topMargin: VMGlobals.adjustHeight(10)
        visible: vmNumberOfProtocols == 0;
    }


    VMButton {
        vmText: loader.getStringForKey("protocol_add").toUpperCase()
        vmIconSource: "plus"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VMGlobals.adjustHeight(200)
        visible: vmNumberOfProtocols == 0;
        onClickSignal: {
            viewEditProtocol.clear()
            mainWindow.swipeTo(VMGlobals.vmSwipeIndexEditProtocol)
        }
    }


}
