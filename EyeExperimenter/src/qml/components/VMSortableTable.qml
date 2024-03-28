import QtQuick
import QtQuick.Controls
import "."
import "../"
import "../js/ObjectListSorter.js" as OLS

Rectangle {

    id: table

    readonly property int vmActionEdit:    0
    readonly property int vmActionDelete:  1
    readonly property int vmActionButton:  2
    readonly property int vmActionArchive: 3

    readonly property double vmRowHeight: VMGlobals.adjustHeight(66);
    readonly property double vmLeftMargin: VMGlobals.adjustWidth(29);
    readonly property double vmPercentWheelRowHeightRatio: 0.84
    readonly property double vmPercentColorDotRowHeightRatio: 0.5
    property double vmActionOffset: 0

    property var vmColWidths: [];
    property var vmColNames: [];
    property var vmColSortable: [];
    property var vmActionEnabledTexts: [];
    property var vmDataMatrix: [];
    property var vmToolTips: [];
    property bool vmCustomActionsEnabled: true;
    property bool vmEditActionEnabled: true;
    property bool vmDeleteActionEnabled: true;
    property bool vmArchiveActionEnabled: true;
    property var vmShowNumericWheel: []
    property var vmShowColorCircle : []

    signal sortChanged(int col, string order)
    signal editClicked(int rowIndex)
    signal deleteClicked(int rowIndex)
    signal archiveClicked(int rowIndex)
    signal customButtonClicked(int rowIndex)

    function configureTable(nameWidthMap,actionColName,tooltips){
        //console.log("Updating header names");
        vmColWidths = [];
        vmColSortable = [];
        vmActionOffset = 0;
        vmToolTips = [];

        if (tooltips === undefined){
            tooltips = [];
        }

        var sumW = 0;
        for (var key in nameWidthMap){
            //console.log("ELEMENT: " + JSON.stringify(nameWidthMap[key]))
            let w = nameWidthMap[key]["width"]
            vmColSortable.push(nameWidthMap[key]["sortable"])
            vmColNames.push(key)
            sumW = sumW + w
            vmColWidths.push(w)
            vmActionOffset = vmActionOffset + w;
            if (key in tooltips){
                vmToolTips.push(tooltips[key])
            }
            else {
                vmToolTips.push("");
            }
        }

        vmColWidths.push(width - sumW);
        //console.log("Column widths " + JSON.stringify(vmColWidths))
        vmColNames.push(actionColName)
        vmToolTips.push(""); // To make sure the action column does not a have tooltip.
        vmColSortable.push(false);

        headerNames.clear()
        for (var  i = 0; i < vmColNames.length; i++){
            headerNames.append({}) // Not really interested in the content. Just the size.
        }
    }

    function setList(list){
        model.clear()
        vmDataMatrix = list;
        for (var i = 0; i < list.length; i++){
            var element = {};
            element["vmIndex"] = i
            model.append(element)
        }
    }

    // This ONLY enables the proper graphical indicator.
    function setSortIndicator(sortIndex,sortDirection){
        if ((sortIndex >= 0) && (sortIndex < headerRow.count)){
            headerRow.itemAt(sortIndex).vmSortState = sortDirection;
        }
    }

    function computeXBasedOnIndex(index){
        if (index === 0){
            return vmLeftMargin
        }
        else{
            var margin = 0;
            for (var i = 0; i < index; i++){
                margin = margin + vmColWidths[i]
            }
            return margin*table.width;
        }
    }

    function changeSortIndicator(index,order){
        for (var i = 0; i < headerRow.count; i++){
            if (i !== index){
                headerRow.itemAt(i).vmSortState = ""
            }
            else{
                headerRow.itemAt(i).vmSortState = order
                sortChanged(index,order)
            }
        }
    }

    ListModel {
        id: model
    }

    ListModel {
        id: headerNames
    }

    Rectangle {
        id: header
        width: parent.width
        height: vmRowHeight
        Repeater {
            id: headerRow
            model: headerNames

            Rectangle {

                height: vmRowHeight
                width:  table.width*vmColWidths[index]
                x: {
                    var margin = 0;
                    for (var i = 0; i < index; i++){
                        margin = margin + vmColWidths[i]
                    }
                    if (index == vmColWidths.length-1){
                        // This patch is required so taht the Action label is aligned left with the icon or text in the button right bellow
                        return margin*table.width + VMGlobals.adjustWidth(20);
                    }
                    return margin*table.width;
                }
                color: "transparent"
                //color: "#ff0000"
                anchors.top: header.top

                property string vmSortState: ""

                onVmSortStateChanged: {
                    sortIndicator.requestPaint()
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked: {
                        if (!sortIndicator.visible) return;
                        if (vmSortState == OLS.ORDER_ASCENDING) changeSortIndicator(index,OLS.ORDER_DESCENDING);
                        else changeSortIndicator(index,OLS.ORDER_ASCENDING);
                    }
                }

                Text {
                    id: headerName
                    text: (vmColNames[index] === undefined) ? "" : vmColNames[index]
                    font.pixelSize: VMGlobals.vmFontLarge
                    font.weight: 600
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    x: (index === 0) ? vmLeftMargin : 0
                }

                VMInfoHeaderIcon {
                    id: infoHeaderIcon
                    anchors.top: headerName.top
                    anchors.left: sortIndicator.visible? sortIndicator.right : headerName.right
                    anchors.leftMargin: VMGlobals.adjustWidth(1)
                    height: 0.2*vmRowHeight
                    vmToolTip: {
                        // To avoid unncessary warnings.
                        if (vmToolTips[index] !== undefined) return vmToolTips[index];
                        else return "";
                    }
                    visible: (vmToolTips[index] !== "")
                }

                Canvas {
                    id: sortIndicator
                    height: VMGlobals.adjustHeight(10)
                    width: height*7/8
                    anchors.verticalCenter: headerName.verticalCenter
                    anchors.left: headerName.right
                    anchors.leftMargin: VMGlobals.adjustWidth(4.5);
                    contextType: "2d";
                    visible: vmColSortable[index]
                    onPaint: {
                        var ctx = sortIndicator.getContext("2d")
                        var upcolor = VMGlobals.vmBlackText
                        var downcolor = VMGlobals.vmBlackText

                        if (vmSortState === OLS.ORDER_ASCENDING){
                            upcolor = VMGlobals.vmBlueSelected
                            downcolor = VMGlobals.vmBlueNotSortIndicator
                        }
                        else if (vmSortState === OLS.ORDER_DESCENDING){
                            downcolor = VMGlobals.vmBlueSelected
                            upcolor = VMGlobals.vmBlueNotSortIndicator
                        }

                        //console.log("Drawing for index " + index + " that has the text " + headerName.text);

                        ctx.reset();

                        // Up indicator
                        ctx.beginPath();
                        ctx.fillStyle = upcolor;
                        ctx.moveTo(width/2,0)
                        ctx.lineTo(width,3*height/8)
                        ctx.lineTo(0,3*height/8)
                        ctx.closePath();
                        ctx.stroke();
                        ctx.fill()

                        // Down Indicator
                        ctx.beginPath();
                        ctx.fillStyle = downcolor;
                        ctx.moveTo(width/2,height)
                        ctx.lineTo(width,5*height/8)
                        ctx.lineTo(0,5*height/8)
                        ctx.closePath();
                        ctx.stroke();
                        ctx.fill()

                    }
                }

            }

        }

    }

    ListView {
        id: patientListView
        clip: true
        x: 0
        y: vmRowHeight
        width: parent.width
        height: parent.height - vmRowHeight
        model: model

        ScrollBar.vertical: ScrollBar {
            active: true
        }

        delegate: Rectangle {
            id: tableRow
            width: table.width
            height: vmRowHeight
            color: {
                if (rowMouseArea.containsMouse) return VMGlobals.vmWhiteButtonHiglight
                else return (vmIndex % 2) ? VMGlobals.vmWhite : VMGlobals.vmGrayToggleOff
            }
            border.width: VMGlobals.adjustWidth(1)
            border.color:  VMGlobals.vmGrayUnselectedBorder

            MouseArea {
                id: rowMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onContainsMouseChanged: {
                    if (containsMouse) tableRow.forceActiveFocus()
                }
            }

            Repeater {

                model: vmColWidths.length-1
                Text {
                    height: vmRowHeight
                    width: table.width*vmColWidths[index]
                    text: {
                        // PATCH: Avoids a warning that does not affect behaviour. Because at some point the data is found and shown.
                        if (vmDataMatrix[vmIndex] === undefined) return ""
                        else return vmDataMatrix[vmIndex][index]
                    }
                    font.pixelSize: VMGlobals.vmFontLarge
                    font.weight: 400
                    verticalAlignment: Text.AlignVCenter
                    x: computeXBasedOnIndex(index)
                    visible: {
                        if (vmShowNumericWheel.includes(index)) return false;
                        if (vmShowColorCircle.includes(index)) return false;
                        return true;
                    }
                }
            }


            Repeater {

                model: vmColWidths.length-1
                VMPercentWheel {
                    //x: computeXBasedOnIndex(index) + table.width*vmColWidths[index]/2 - vmRowHeight*vmPercentWheelRowHeightRatio/2
                    x: computeXBasedOnIndex(index)
                    y: vmRowHeight*(1-vmPercentWheelRowHeightRatio)/2
                    vmAnimationDuration: 800
                    vmPercent: {
                        vmOuterColor = VMGlobals.vmRedError
                        vmInnerColor = VMGlobals.vmRedBadIndexBackground

                        // PATCH: Avoids a warning that does not affect behaviour. Because at some point the data is found and shown.
                        if (vmDataMatrix[vmIndex] === undefined) {
                            return 0
                        }
                        let value = vmDataMatrix[vmIndex][index]

                        // In order to specify both the value and if it passes or not, the string used cotains the number | the boolean.
                        if (value === null) return 0;
                        if (typeof value != "string") return 0;
                        let value_parts = value.split("|");

                        if (value_parts.length !== 2){
                            return 0
                        }

                        let pass = value_parts[1]
                        value = parseInt(value_parts[0]);

                        // If it is effectively a number (the first part) then we know the data is formatted properly
                        // In this case if pass is true then we retun the value and set the color to green.
                        if (typeof value === 'number') {
                            if (pass === "true"){
                                vmOuterColor = VMGlobals.vmGreenSolidQCIndicator
                                vmInnerColor = VMGlobals.vmGreenBKGStudyMessages
                            }
                            return value;
                        }
                        else {
                            return 0;
                        }
                    }
                    onVmPercentChanged: {
                        setDesiredCircleHeight(vmRowHeight*vmPercentWheelRowHeightRatio);
                    }
                    visible: {
                        if (vmShowNumericWheel.includes(index)) return true;
                        return false;
                    }
                }
            }

            Repeater {

                // A repeater to show a colored circle in a given column.

                model: vmColWidths.length-1
                Rectangle {
                    //x: computeXBasedOnIndex(index) + table.width*vmColWidths[index]/2 - vmRowHeight*vmPercentWheelRowHeightRatio/2
                    x: computeXBasedOnIndex(index)
                    y: vmRowHeight*(1-vmPercentColorDotRowHeightRatio)/2
                    height: vmRowHeight*vmPercentColorDotRowHeightRatio
                    width: height
                    radius: height/2
                    color: {
                        // PATCH: Avoids a warning that does not affect behaviour. Because at some point the data is found and shown.
                        let row = vmDataMatrix[vmIndex];
                        if ((row === null) || (row === undefined)){
                            return "#000000"
                        }
                        let color = row[index];
                        if (color === undefined || color === null){
                            return "#000000"
                        }
                        return color;
                    }
                    visible: {
                        if (vmShowColorCircle.includes(index)) return true;
                        return false;
                    }
                }
            }


            Row {

                id: actionButtonRow
                x:  vmActionOffset*table.width
                height: vmRowHeight
                width: tableRow.width - vmActionOffset
                spacing: VMGlobals.adjustWidth(2)


                VMButton {
                    id: editButton
                    vmText: vmActionEnabledTexts[vmActionEdit]
                    vmIconSource: "qrc:/images/pencil.png"
                    vmButtonType: editButton.vmTypeTertiary
                    vmThinButton: true
                    anchors.verticalCenter: parent.verticalCenter
                    visible: (vmActionEnabledTexts[vmActionEdit] !== "")
                    vmEnabled: vmEditActionEnabled
                    onClickSignal: {
                        editClicked(vmIndex)
                    }
                }

                VMButton {
                    id: deleteButton
                    //vmText: "BORRAR"
                    vmText: vmActionEnabledTexts[vmActionDelete]
                    vmIconSource: "qrc:/images/trash.png"
                    vmButtonType: editButton.vmTypeTertiary
                    vmThinButton: true
                    anchors.verticalCenter: parent.verticalCenter
                    visible: (vmActionEnabledTexts[vmActionDelete] !== "")
                    vmEnabled: vmDeleteActionEnabled
                    onClickSignal: {
                        deleteClicked(vmIndex)
                    }
                }

                VMButton {
                    id: archiveButton
                    //vmText: "BORRAR"
                    vmText: vmActionEnabledTexts[vmActionArchive]
                    vmIconSource: "qrc:/images/archive-gray.png"
                    vmButtonType: editButton.vmTypeTertiary
                    vmThinButton: true
                    anchors.verticalCenter: parent.verticalCenter
                    visible: (vmActionEnabledTexts[vmActionArchive] !== "")
                    vmEnabled: vmArchiveActionEnabled
                    onClickSignal: {
                        archiveClicked(vmIndex)
                    }
                }

                VMButton {
                    id: customActionButton
                    vmText: vmActionEnabledTexts[vmActionButton]
                    vmThinButton: true
                    anchors.verticalCenter: parent.verticalCenter
                    visible: (vmActionEnabledTexts[vmActionButton] !== "")
                    vmEnabled: vmCustomActionsEnabled
                    onClickSignal: {
                        customButtonClicked(vmIndex)
                    }
                }
            }


        }
        onCurrentIndexChanged: {
        }
    }

}
