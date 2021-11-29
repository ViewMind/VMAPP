import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../"

Item {

    id: vmComboBox
    height: VMGlobals.vmControlsHeight

    property string vmErrorMsg: ""
    property bool vmEnabled: true
    property int vmMaxDisplayItems: 5
    property int vmCurrentIndex: -1
    property string vmCurrentText: ""
    property int vmListSize: 0
    property string vmPlaceHolderText: "Placeholder"
    property string vmLabel: ""

    readonly property int vmMOUSE_OUT_TIME: 200

    readonly property double vmLeftMargin: Math.ceil(width*10/338);
    readonly property double vmRightMargin: Math.ceil(width*18/338);
    readonly property double vmDSOffset: width*0.01
    readonly property double vmListContainerBorderWidth: Math.round(vmComboBox.width/338);

    ///////////////////// FUNCTIONS
    function setModelList(list){
        itemList.clear()
        //console.log("====================================");
        for (var i = 0; i < list.length; i++){
            //console.log(JSON.stringify(list[i]));
            if (typeof list[i] === 'object'){
                itemList.append({"vmText": list[i]["value"], "vmIndex": i, "vmMetadata" : list[i]["metadata"], "hasMouse" : false});
            }
            else{
                itemList.append({"vmText": list[i], "vmIndex": i, "hasMouse" : false});
            }
        }
        vmListSize = list.length
        setSelection(-1)
    }

    onVmPlaceHolderTextChanged: {
        setSelection(vmCurrentIndex)
    }

    function setSelection(selectedIndex){
        if (selectedIndex === -1){
            //console.log("Setting placeholder " + vmPlaceHolderText)
            vmCurrentIndex = -1;
            vmCurrentText = vmPlaceHolderText
            displayText.text = vmPlaceHolderText
            //mouseOutTimer.running = false;
            listContainer.visible = false;
            return
        }
        if (itemList.count < 1) return;        
        displayText.text = itemList.get(selectedIndex).vmText;        
        vmCurrentText = displayText.text;
        //console.log("Setting current text to " + vmCurrentText + " from index " + selectedIndex);
        //mouseOutTimer.running = false;
        listContainer.visible = false;
        vmCurrentIndex = selectedIndex;
    }

//    function shouldStartDropDownTimer(hasMouse){
//        if (!hasMouse){
//            if (!mouseOutTimer.running){
//                mouseOutTimer.running = true;
//            }
//        }
//        else mouseOutTimer.running = false;
//    }

//    function dropdownHasMouse(){

//        //console.log("Dropdown Has Mouse");

//        for (var i = 0; i < itemList.count; i++){
//            var item = itemList.get(i);
//            if (item.hasMouse) {
//                //console.log("Item " + i + " has the mouse");
//                return; // At least one item has the mouse.
//            }
//        }

//        // No item has the mouse, we need to check if the scroll bar has it.
//        if (scrollControlMA.containsMouse) {
//            //console.log("Scroll bar has the mouse");
//            return ;
//        }

//        listContainer.visible = false;

//    }

    signal selectionChanged();

//    ///////////////////// DROPDOWN CLOSE CHECK TIMER
//    Timer {
//        id: mouseOutTimer
//        interval: vmMOUSE_OUT_TIME;
//        running: false;
//        repeat: false
//        onTriggered: {
//            dropdownHasMouse()
//        }
//    }

    ///////////////////// THE LIST MODEL
    ListModel {
        id: itemList
    }

    ///////////////////// MAIN DISPLAY
    Rectangle {

        id: display
        anchors.fill: parent
        color: {
            if (vmErrorMsg === "") return  VMGlobals.vmWhite
            else VMGlobals.vmRedErrorBackground
        }
        border.color: {
            if (vmErrorMsg === ""){
                return listContainer.visible? VMGlobals.vmBlueSelected : VMGlobals.vmGrayUnselectedBorder
            }
            else {
                return VMGlobals.vmRedError;
            }
        }
        border.width: Math.ceil(vmComboBox.width/300);
        radius: Math.round(10*vmComboBox.height/44);

        // The display Text.
        Text {
            id: displayText
            text: "Placeholder"
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
            color: {
                if (vmErrorMsg == ""){
                    return listContainer.visible? VMGlobals.vmGrayAccented : VMGlobals.vmGrayPlaceholderText
                }
                else{
                    return VMGlobals.vmBlackText
                }
            }
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: vmLeftMargin
        }

        // The triangle that functions as an indicator.
        Canvas {
            id: canvas

            width: Math.round(8*display.width/338);
            height: Math.round(4*display.height/44);
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: vmRightMargin

            contextType: "2d"

            onPaint: {
                var ctx = canvas.getContext("2d");
                ctx.reset();
                if (listContainer.visible){
                    // Up triangle.
                    ctx.moveTo(0, height);
                    ctx.lineTo(width, height);
                    ctx.lineTo(width / 2, 0);
                }
                else{
                    // Down triangle.
                    ctx.moveTo(0, 0);
                    ctx.lineTo(width, 0);
                    ctx.lineTo(width / 2, height);
                }
                ctx.closePath();
                ctx.fillStyle = VMGlobals.vmGrayAccented;
                ctx.fill();
            }
        }

        MouseArea {
            id: mainDisplayMA
            anchors.fill: parent
            onClicked: {
                if (vmEnabled){
                    if (itemList.count === 0) return;
                    listContainer.visible = true;
                    vmErrorMsg = "";
                }
            }
        }

    }

    ///////////////////// DROP DOWN
    Rectangle {
        id: listContainer
        color: VMGlobals.vmWhite
        border.color: VMGlobals.vmGrayUnselectedBorder
        border.width: vmListContainerBorderWidth
        anchors.top: display.bottom
        anchors.topMargin: Math.round(5*vmComboBox.height/44);
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        radius: display.radius

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

        onActiveFocusChanged: {
            if (!activeFocus) visible = false
        }

        onVisibleChanged: {
            canvas.requestPaint()
            if (visible){
                listContainer.forceActiveFocus()
            }

        }

        ListView {
            id: listContainerListView
            width: vmComboBox.width
            // We need to tell the list view to occupy a bit less than the height of the container so that are no part of the items "out" of the container box.
            height: listContainer.height*0.98
            y: (listContainer.height - height)/2
            model: itemList
            clip: true

            onContentYChanged: {
                if (scrollBar.isScrolling) return;
                var y = (contentY - originY) * (height / contentHeight);
                scrollControl.y = y;
            }

            delegate: Rectangle {
                width: vmComboBox.width*0.98
                height: vmComboBox.height
                x: (vmComboBox.width - width)/2
                color: hasMouse? VMGlobals.vmBlueBackgroundHighlight : VMGlobals.vmWhite
                radius: listContainer.radius

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
                        //shouldStartDropDownTimer(containsMouse);
                    }
                }

                Text {
                    id: textItem
                    text: vmText
                    font.pixelSize: VMGlobals.vmFontLarge
                    font.weight: 600
                    color: hasMouse? VMGlobals.vmBlueTextHighlight : VMGlobals.vmBlackText
                    verticalAlignment: Text.AlignVCenter
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: vmRightMargin - parent.x
                }
            }


        }

        Rectangle {

            id: scrollBar
            width: 0.02*listContainer.width
            height: listContainer.height - display.radius*2
            anchors.verticalCenter: listContainer.verticalCenter
            anchors.right: listContainer.right
            anchors.rightMargin: vmListContainerBorderWidth*2
            radius: listContainer.radius;

            property bool isScrolling: false

            function scroll(y){
                // Doing the scroll computations.
                var totalH = vmListSize*vmComboBox.height;
                var maxScrollY = scrollControlMA.height - scrollControl.height;
                var yPosForList = totalH*y/maxScrollY
                var itemThatShouldBeVisble = Math.ceil(yPosForList/vmComboBox.height)

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

                    //shouldStartDropDownTimer(containsMouse);
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

                color: VMGlobals.vmGrayAccented
                onYChanged: {
                    if (!scrollBar.isScrolling) return;
                    scrollBar.scroll(y)
                }
            }

        }


        layer.enabled: true
        layer.effect: DropShadow{
            transparentBorder: true
            source: listContainer
            horizontalOffset: vmDSOffset
            verticalOffset: vmDSOffset
            radius: 10
            color: VMGlobals.vmGrayTranslucent
        }

    }

    //////////////////// THE ERROR MESSAGE
    Text{
        id: errorMsg
        text: vmErrorMsg
        color:  VMGlobals.vmRedError
        font.pixelSize: VMGlobals.vmFontBaseSize
        font.weight: 400
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 10
        z: vmComboBox.z - 1
        visible: (vmErrorMsg !== "")
    }

    //////////////////// THE LABEL (if present).
    Text {
        id: label
        text: vmLabel
        color: VMGlobals.vmBlackText
        font.pixelSize: VMGlobals.vmFontLarge
        font.weight: 600
        anchors.left: parent.left
        anchors.bottom: parent.top
        anchors.bottomMargin: 10;
        z: vmComboBox.z - 1
        visible: (vmLabel !== "")
    }

}
