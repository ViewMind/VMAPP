import QtQuick 2.6
import QtQuick.Controls 2.3

Item {

    id: vmDatSelectionComboBox
    property string vmTitle: ""
    property string vmSelection: ""
    property string vmPlaceHolderText: ""
    property bool vmFilledDisplay: false
    property bool vmEnabled: true
    property int vmCurrentIndex: -1;
    readonly property int vmDatItemHeight: 60


    function setModelList(list){
        for (var i = 0; i < list.length; i++){
            modelList.append({"vmDisplayText": list[i], "vmIndexInList": i});
        }
    }

    function clearSelection(){
        vmCurrentIndex = -1;
        vmSelection = "";
        vmFilledDisplay = false;
        listContainer.visible = false;
        modelList.clear();
    }

    signal archiveRequested(int indexInList);

    ListModel {
        id: modelList
    }


    MouseArea {
        id: mouseTitleDetector
        anchors.fill: parent
        onClicked: {
            if (vmEnabled){
                listContainer.visible = !listContainer.visible;
                updateDisplay();
            }
        }
    }

    function updateDisplay(){
        if (listContainer.visible) vmFilledDisplay = false;
        else {
            if (vmSelection === "") vmFilledDisplay = false;
            else vmFilledDisplay = true;
        }
    }

    Rectangle {
        id: titleBox
        anchors.fill: parent
        border.width: 1
        border.color: vmEnabled? "#3096ef" : "#888889"
        color: {
            if (!vmEnabled) return "#ffffff"
            else if (vmFilledDisplay) return "#2873b4"
            else return "#ffffff"
        }

        Column {

            anchors.verticalCenter: parent.verticalCenter
            anchors.left:  parent.left
            anchors.leftMargin: 20
            spacing: 5

            Text {
                id: titleText
                font.family: viewHome.gothamB.name
                font.pixelSize: 13
                text: vmTitle
                color: {
                    if (!vmEnabled) return "#888889"
                    else if (vmFilledDisplay) return "#ffffff"
                    else return "#000000"
                }
            }

            Text {
                id: selectionText
                font.family: viewHome.robotoR.name
                font.pixelSize: 12
                text: (vmSelection === "")? vmPlaceHolderText : vmSelection
                color: {
                    if (!vmEnabled) return "#888889"
                    else if (vmFilledDisplay) return "#ffffff"
                    else return "#000000"
                }
            }

        }

    }

    onVmEnabledChanged: {
        downArrow.requestPaint()
    }

    onVmFilledDisplayChanged: {
        downArrow.requestPaint();
    }

    Canvas {
        id: downArrow
        width: 12
        height: 8
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        contextType: "2d"
        onPaint: {
            var ctx = downArrow.getContext("2d");
            ctx.reset();
            if (listContainer.visible){
                ctx.moveTo(0, 8);
                ctx.lineTo(12, 8);
                ctx.lineTo(6, 0);
            }
            else{
                ctx.moveTo(0, 0);
                ctx.lineTo(12, 0);
                ctx.lineTo(6, 8);
            }
            ctx.closePath();
            if (!vmEnabled) ctx.fillStyle = "#888889"
            else if (vmFilledDisplay) ctx.fillStyle = "#ffffff"
            else ctx.fillStyle = "#2873b4"
            ctx.fill();
        }
    }


    Rectangle {

        id: listContainer
        color: "#ffffff"
        border.width: titleBox.border.width
        border.color: titleBox.border.color
        anchors.top: titleBox.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width-4
        height: { if (modelList.count > 3) return vmDatItemHeight*3
                  else return modelList.count*vmDatItemHeight; }
        visible: false;

        onVisibleChanged: {
            downArrow.requestPaint();
        }

        Rectangle {
            id: borderRect
            height: parent.height+2;
            width: parent.width+4
            border.width: titleBox.border.width
            border.color: titleBox.border.color
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ScrollView {
            id: listContainerScroll
            anchors.fill: parent
            clip: true
            ListView {
                id: listContainerListView
                anchors.fill: parent
                model: modelList
                delegate: VMDatItem {
                    width: parent.width
                    height: vmDatItemHeight
                    onArchiveItem: {
                        archiveRequested(indexInList)
                    }
                    onSelectionMade: {
                        vmCurrentIndex = indexInList;
                        listContainer.visible = false;
                        vmSelection = displayText;
                        updateDisplay();
                    }
                }
            }
        }
    }

}
