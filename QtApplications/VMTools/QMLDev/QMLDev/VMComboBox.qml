import QtQuick 2.6
//import QtQuick.Controls 2.3
//import QtQuick.Controls.Styles 1.4
//import QtQuick.Controls 1.4

Item {

    id: vmComboBox
    height: mainWindow.height*0.043

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property int vmMaxDisplayItems: 5
    property int vmCurrentIndex: -1
    property string vmCurrentText: ""
    property int vmListSize: 0
    readonly property int vmMOUSE_OUT_TIME: 200


    ///////////////////// FUNCTIONS
    function setModelList(list){
        itemList.clear()
        //console.log("====================================");
        for (var i = 0; i < list.length; i++){
            //console.log(JSON.stringify(list[i]));
            if (typeof list[i] === 'object'){
                //console.log("Is array");
                itemList.append({"vmText": list[i]["value"], "vmIndex": i, "vmMetadata" : list[i]["metadata"], "hasMouse" : false});
            }
            else{
                //console.log("Is not array");
                itemList.append({"vmText": list[i], "vmIndex": i, "hasMouse" : false});
            }
        }
        vmListSize = list.length
        setSelection(0)
    }

    function setSelection(selectedIndex){
        vmCurrentIndex = selectedIndex;
        displayText.text = itemList.get(selectedIndex).vmText;
        vmCurrentText = displayText.text;
        mouseOutTimer.running = false;
        listContainer.visible = false;
    }

    function shouldStartDropDownTimer(hasMouse){
        if (!hasMouse){
            if (!mouseOutTimer.running){
                mouseOutTimer.running = true;
            }
        }
        else mouseOutTimer.running = false;
    }

    function dropdownHasMouse(){

        //console.log("Dropdown Has Mouse");

        for (var i = 0; i < itemList.count; i++){
            var item = itemList.get(i);
            if (item.hasMouse) {
                //console.log("Item " + i + " has the mouse");
                return; // At least one item has the mouse.
            }
        }

        // No item has the mouse, we need to check if the scroll bar has it.
        if (scrollControlMA.containsMouse) {
            //console.log("Scroll bar has the mouse");
            return ;
        }

        listContainer.visible = false;

    }

    signal selectionChanged();


    ///////////////////// DROPDOWN CLOSE CHECK TIMER
    Timer {
        id: mouseOutTimer
        interval: vmMOUSE_OUT_TIME;
        running: false;
        repeat: false
        onTriggered: {
            dropdownHasMouse()
        }
    }

    ///////////////////// THE LIST MODEL
    ListModel {
        id: itemList
    }

    ///////////////////// MAIN DISPLAY
    Rectangle {
        id: display
        anchors.fill: parent
        color: vmEnabled? "#ebf3fa" : "#bcbec0"

        // The display Text.
        Text {
            id: displayText
            text: "Test Text"
//            font.family: viewHome.robotoR.name
//            font.pixelSize: 13*viewHome.vmScale
            color: "#58595b"
            verticalAlignment: Text.AlignVCenter
            anchors.bottom: divisorLine.top
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.004
        }

        // The divisor line at the bottom.
        Rectangle{
            id: divisorLine
            color: "#297fca"
            border.color: "#297fca"
            border.width: 0;
            height: 1;
            width: parent.width
            anchors.bottom: parent.bottom
        }

        // The triangle that functions as an indicator.
        Canvas {
            id: canvas
            width: mainWindow.width*0.009
            height: mainWindow.height*0.012
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.004
            contextType: "2d"

            onPaint: {
                var ctx = canvas.getContext("2d");
                ctx.reset();
                ctx.moveTo(0, 0);
                ctx.lineTo(width, 0);
                ctx.lineTo(width / 2, height);
                ctx.closePath();
                ctx.fillStyle = "#000000";
                ctx.fill();
            }
        }

        MouseArea {
            id: mainDisplayMA
            anchors.fill: parent
            onClicked: {
                if (vmEnabled){
                    listContainer.visible = true;
                }
            }
        }

    }

    ///////////////////// DROP DOWN
    Rectangle {
        id: listContainer
        color: "#dadada"
        anchors.top: display.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: {
            var numDisplay = vmMaxDisplayItems;
            if (numDisplay > vmListSize) {
                scrollBar.visible = false;
                numDisplay = vmListSize;
            }
            else{
                scrollBar.visible = true;
            }
            return numDisplay*vmComboBox.height;
        }
        visible: false
        clip: true

        ListView {
            id: listContainerListView
            anchors.fill: parent
            model: itemList

            onContentYChanged: {
                if (scrollBar.isScrolling) return;
                var y = (contentY - originY) * (height / contentHeight);
                scrollControl.y = y;
            }

            delegate: Rectangle {
                width: vmComboBox.width
                height: vmComboBox.height
                border.width: mainWindow.width*0.002
                border.color: "#dadada"
                color: hasMouse? "#eeeeee" : "#ffffff"

                MouseArea {
                    id: mouseItemDetector
                    anchors.fill: parent
                    hoverEnabled: true
                    propagateComposedEvents: true
                    onClicked: {
                        setSelection(vmIndex);
                        selectionChanged();
                    }
                    onContainsMouseChanged: {
                        hasMouse = containsMouse;
                        shouldStartDropDownTimer(containsMouse);
                    }
                }

                Text {
                    id: textItem
                    text: vmText
//                    font.family: viewHome.robotoR.name
//                    font.pixelSize: 13*viewHome.vmScale
                    color: "#58595b"
                    verticalAlignment: Text.AlignVCenter
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: mainWindow.width*0.004
                }
            }


        }

        Rectangle {

            id: scrollBar
            width: 0.05*listContainer.width
            height: listContainer.height
            anchors.top: listContainer.top
            anchors.right: listContainer.right
            border.color: "#dadada"
            radius: width*0.5;

            property bool isScrolling: false

            function scroll(y){
                // Doing the scroll computations.
                var totalH = vmListSize*vmComboBox.height;
                var maxScrollY = scrollControlMA.height - scrollControl.height;
                var yPosForList = totalH*y/maxScrollY
                var itemThatShouldBeVisble = Math.ceil(yPosForList/vmComboBox.height)
                console.log("Item that should be visible: " + itemThatShouldBeVisble)
                listContainerListView.positionViewAtIndex(itemThatShouldBeVisble,ListView.End)

            }

            MouseArea {
                id: scrollControlMA
                anchors.fill: parent
                hoverEnabled: true
                onContainsMouseChanged: {

                    // Making sure that no item has the mouse.
                    // This should not be necessary but for some reason the delegates of the list view sometimes do not detect when the mouse leaves them.
                    for (var i = 0; i < itemList.count; i++){
                        itemList.setProperty(i,"hasMouse",false);
                    }

                    shouldStartDropDownTimer(containsMouse);
                }

                drag.target: scrollControl
                drag.axis: Drag.YAxis
                drag.minimumY: 0
                drag.maximumY: scrollControlMA.height - scrollControl.height

                drag.onActiveChanged: {
                    if (drag.active){
                        scrollBar.isScrolling = true;
                    }
                    else{
                        scrollBar.isScrolling = false;
                    }
                }

                onClicked: {
                    // If the click falls inside the scroll controller, then it is ignored.
                    var y = mouseY
                    var maxY = (scrollBar.height - scrollControl.height);
                    if ( (y > scrollControl.y) && (y < (scrollControl.y + scrollControl.height)) ) return;

                    if (scrollBar.isScrolling) return;

                    y = y - scrollControl.height/2;
                    if (y < 0) y = 0;
                    if (y > maxY) y = maxY;

                    scrollControl.y = y;
                    console.log("Scroll click is " + y);
                    scrollBar.scroll(y)

                }

            }

            Rectangle {
                id: scrollControl
                width: parent.width
                height: {
                    var maxH = parent.height;
                    var scale = vmMaxDisplayItems/vmListSize;
                    if (scale > 1) return parent.height;
                    if (scale < 0.08) scale = 0.08;
                    return parent.height*scale
                }

                radius: scrollBar.radius

                color: "#dadada"
                onYChanged: {
                    if (!scrollBar.isScrolling) return;
                    console.log("On moving Scroll Bar: " + y);
                    scrollBar.scroll(y)
                }
            }

        }

    }

    //////////////////// THE ERROR MESSAGE
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  "#ca2026"
//        font.family: viewHome.robotoR.name
//        font.pixelSize: 12*viewHome.vmScale
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: mainWindow.height*0.007
        z: vmComboBox.z - 1
        visible: (vmErrorMsg !== "")
    }

}
