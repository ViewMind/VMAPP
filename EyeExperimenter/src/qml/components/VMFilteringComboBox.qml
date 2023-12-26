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
    property bool vmShowRemoveButton: false
    property bool vmIfShowRemoveButtonHideInFirst: false

    readonly property int vmMOUSE_OUT_TIME: 200

    readonly property double vmLeftMargin: VMGlobals.adjustWidth(10);
    readonly property double vmRightMargin: VMGlobals.adjustWidth(18);
    readonly property double vmDSOffset: width*0.01
    readonly property double vmListContainerBorderWidth: Math.round(vmComboBox.width/338);

    ///////////////////// FUNCTIONS
    function setModelList(list){
        display.originalList = [];
        //console.log("====================================");
        for (var i = 0; i < list.length; i++){
            //console.log(JSON.stringify(list[i]));
            if (typeof list[i] === 'object'){
                display.originalList.push({"vmText": list[i]["value"], "vmIndex": i, "vmMetadata" : list[i]["metadata"], "hasMouse" : false});
            }
            else{
                display.originalList.push({"vmText": list[i], "vmIndex": i, "hasMouse" : false});
            }
        }
        display.fillModelList(display.originalList)
        setSelection(-1)
    }

    function setSelection(selectedIndex){
        if (selectedIndex === -1){
            //console.log("Setting placeholder " + vmPlaceHolderText)
            vmCurrentIndex = -1;
            vmCurrentText = vmPlaceHolderText
            displayText.text = vmPlaceHolderText
            listContainer.visible = false;
            return
        }
        if (itemList.count < 1) return;
        displayText.text = display.originalList[selectedIndex].vmText;
        vmCurrentText = displayText.text;
        //console.log("Setting current text to " + vmCurrentText + " from index " + selectedIndex);
        vmCurrentIndex = selectedIndex;
        listContainer.visible = false;
    }

    function getCurrentlySelectedMetaDataField(){
        if (vmCurrentIndex === -1) return ""
        //let item =  itemList.get(vmCurrentIndex);
        let item = display.originalList[vmCurrentIndex];
        if (item["vmMetadata"] !== undefined) return item["vmMetadata"]
        else return "";
    }


    signal selectionChanged();
    signal removeClicked(int index, string text);

    ///////////////////// THE LIST MODEL
    ListModel {
        id: itemList
    }

    ///////////////////// MAIN DISPLAY
    Rectangle {

        id: display

        property string searchText: ""
        property var originalList: [];

        function onTyping(key){

            //console.log("On Typing. Key: " + JSON.stringify(key));

            // We are basically building a rudimentary input in this function. So we need to make sure that we need to test for some special function keys.
            if (key.key === Qt.Key_Escape){
                vmComboBox.setSelection(vmCurrentIndex);
                return;
            }
            else if (key.key === Qt.Key_Backspace){
                searchText = searchText.slice(0,-1)
            }
            else if (key.text === "|"){
                return;
            }
            else if ((key.key === Qt.Key_Enter) || (key.key === Qt.Key_Return)){
                //console.log("We've got an enter");
                if (vmListSize == 1){
                    // We select the first option and do nothing else.
                    setSelection(itemList.get(0).vmIndex);
                }
                return;
            }
            else {
                searchText = searchText + key.text;
            }

            // We need to check it fits.
            if (textMeasure.width > displayText.width){
                // We remove the last character and do nothing.
                searchText = bkp;
                searchText = searchText.slice(0,-1)
            }

            displayText.text = searchText + "|";
            filter();
        }

        function fillModelList(filteredList){
            itemList.clear()
            for (var i = 0; i < filteredList.length; i++){
                itemList.append(filteredList[i]);
            }
            vmListSize = filteredList.length
        }

        function prepForSearching(){
            display.searchText = "";
            displayText.text = "|";
            display.filter()
        }

        function filter(){
            let filteredList = [];
            if (searchText == ""){
                filteredList = originalList
            }
            else {
                // The actual searching.
                let to_search = searchText.toLowerCase();

                for (let i = 0; i < originalList.length; i++){
                    let searchable = originalList[i].vmText.toLowerCase();
                    if (searchable.includes(to_search)){
                        filteredList.push(originalList[i])
                    }
                }

            }

            if (filteredList.length == 0){
                // We add at least the empty option.
                filteredList.push({"vmText": "", "vmIndex": -2, "hasMouse" : false});
            }

            fillModelList(filteredList);
        }

        anchors.fill: parent
        color: {
            if (vmEnabled){
                if (vmErrorMsg === "") return  VMGlobals.vmWhite
                else VMGlobals.vmRedErrorBackground
            }
            else VMGlobals.vmGrayTextButtonDisabled
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
        TextInput {
            id: displayText
            text: "Placeholder"
            font.pixelSize: VMGlobals.vmFontBaseSize
            font.weight: 400
            autoScroll: true
            width: display.width - canvas.width - vmRightMargin*2
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

        TextMetrics {
            id: textMeasure
            font: displayText.font
            text: display.searchText
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
                    display.prepForSearching();
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
            else {
                if (vmCurrentIndex > -1){
                    displayText.text = vmCurrentText;
                }
                else {
                    displayText.text = vmPlaceHolderText;
                }
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
                id: containerRect
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
                        if (vmIndex === -2){
                            // This needs to be ignored.
                            return
                        }
                        setSelection(vmIndex);
                        selectionChanged();
                    }
                    onContainsMouseChanged: {
                        hasMouse = containsMouse;
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

                MouseArea {
                    id: btnRemove
                    height: VMGlobals.adjustHeight(10)
                    width: height
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: VMGlobals.adjustWidth(10)
                    hoverEnabled: true
                    propagateComposedEvents: true
                    visible: {//vmShowRemoveButton && ( (index !== 0) && vmIfShowRemoveButtonHideInFirst )
                        if (!vmShowRemoveButton) return false;
                        // We need to show the remove button, if we got here.
                        if (vmIfShowRemoveButtonHideInFirst){
                            if (index === 0) return false;  // We need to show the remove button, but not in the first one.
                            return true;
                        }
                        return true;
                    }
                    onClicked: {
                        let text = itemList.get(vmIndex).vmText
                        removeClicked(vmIndex,text)
                    }
                    onContainsMouseChanged: {
                        closeButton.requestPaint();
                    }
                    onPressed: {
                        closeButton.requestPaint();
                    }
                    onReleased: {
                        closeButton.requestPaint();
                    }

                    Canvas {
                        id: closeButton
                        anchors.fill: parent
                        visible: vmShowRemoveButton
                        contextType: "2d"
                        onPaint: {

                            var lw = VMGlobals.adjustHeight(1);

                            var ctx = closeButton.getContext("2d");
                            ctx.reset();

                            let w = width
                            let h = height
                            let x = 0
                            let y = 0;

                            if (btnRemove.containsPress){
                                w = w*0.8;
                                h = h*0.8;
                                x = w*0.1
                                y = h*0.1
                            }

                            let cx = w/2
                            let cy = h/2

                            if (btnRemove.containsMouse){
                                ctx.strokeStyle = VMGlobals.vmGrayButtonPressed
                                ctx.fillStyle = VMGlobals.vmGrayButtonPressed
                                ctx.fillRect(x,y,w,h)
                            }
                            else {
                                ctx.strokeStyle = containerRect.color
                                ctx.fillStyle = containerRect.color
                                ctx.fillRect(x,y,w,h)
                            }

                            ctx.strokeStyle = VMGlobals.vmBlackText
                            ctx.lineWidth = lw
                            ctx.lineCap = "round"

                            ctx.moveTo(lw+x,lw+y);
                            ctx.lineTo(w-lw-x,h-lw-y)
                            ctx.moveTo(w-lw-x,lw+y)
                            ctx.lineTo(lw+x,h-lw-y)

                            ctx.stroke()
                        }
                    }

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

        Keys.onPressed: (event) => {
            display.onTyping(event);
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
