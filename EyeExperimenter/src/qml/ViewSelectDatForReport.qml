import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property int vmLIST_INDEX_READING: 0
    readonly property int vmLIST_INDEX_BINDING_BC: 1
    readonly property int vmLIST_INDEX_BINDING_UC: 2

    readonly property int vmSEL_NOT_SELECTED:     0
    readonly property int vmSEL_SELECTED_VALID:   1
    readonly property int vmSEL_SELECTED_INVALID: 2

    ListModel {
        id: readingList
    }

    ListModel {
        id: bindingBCList
    }

    ListModel {
        id: bindingUCList
    }

    function readingSelectionChanged(index){
        // Since it's not possible to unselect once selected, the generate button is enabled.
        enableGenerateButtonCheck();
        loader.operateOnRepGenStruct(index,vmLIST_INDEX_READING)
    }

    function bindingBCSelectionChanged(index){
        // When this is clicked, the generate button is automatically disabled because this means
        // that any UC selection is cleared.
        bindingUCColumn.currentIndex = -1;
        enableGenerateButtonCheck();
        var bindingUCFiles = loader.getFileListCompatibleWithSelectedBC(index);
        bindingUCList.clear();
        for (var i = 0; i < bindingUCFiles.length; i++){
            bindingUCList.append({"vmDisplayText": bindingUCFiles[i], "vmIndexInList": i, "vmIsSelected" : false});
        }
        loader.operateOnRepGenStruct(index,vmLIST_INDEX_BINDING_BC)
    }

    function bindingUCSelectionChanged(index){
        // When this is clicked then the generate button is enabled, as only valid options are shown.
        enableGenerateButtonCheck();
        loader.operateOnRepGenStruct(index,vmLIST_INDEX_BINDING_UC)
    }

    function enableGenerateButtonCheck(){

        var selectedType = [];
        if (readingColumn.currentIndex > -1) selectedType.push(vmSEL_SELECTED_VALID)
        else selectedType.push(vmSEL_NOT_SELECTED);

        if ((bindingBCColumn.currentIndex == -1) && (bindingUCColumn.currentIndex == -1)) selectedType.push(vmSEL_NOT_SELECTED);
        else if ((bindingBCColumn.currentIndex > -1) && (bindingUCColumn.currentIndex > -1)) selectedType.push(vmSEL_SELECTED_VALID)
        else selectedType.push(vmSEL_SELECTED_INVALID);

        btnGenerate.enabled = false;
        for (var i = 0; i < selectedType.length; i++){
            if (selectedType[i] === vmSEL_SELECTED_INVALID) {
                btnGenerate.enabled = false;
                return;
            }
            if (selectedType[i] === vmSEL_SELECTED_VALID) btnGenerate.enabled = true;

        }
    }

    function relodInformationOnOpened(){
        loader.reloadPatientDatInformationForCurrentDoctor();
        readingList.clear();
        bindingBCList.clear();
        bindingUCList.clear();
        tableRow.disableArchiveButtons();
        tableRow.resetIndexes();
        btnGenerate.enabled = false;
        loader.operateOnRepGenStruct(-1,-1);

        // Loading the dat lists.
        var readingFiles = loader.getFileListForPatient(vmLIST_INDEX_READING);
        for (var i = 0; i < readingFiles.length; i++){
            readingList.append({"vmDisplayText": readingFiles[i], "vmIndexInList": i, "vmIsSelected" : false});
        }

        var bbcFiles = loader.getFileListForPatient(vmLIST_INDEX_BINDING_BC);
        for (i = 0; i < bbcFiles.length; i++){
            bindingBCList.append({"vmDisplayText": bbcFiles[i], "vmIndexInList": i, "vmIsSelected" : false});
        }
    }

    onOpened: {
        relodInformationOnOpened();
    }


    readonly property string keybase: "viewselectdata_"
    id: viewSelectDatForReport
    modal: true
    width: 1000
    height: 600
    closePolicy: Popup.NoAutoClose

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    // The select report title
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"labelTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 70
    }

    // The subtitle with the explanation
    Text {
        id: subTitle
        font.pixelSize: 11
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"labelSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: 11
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewSelectDatForReport.close();
        }
    }

    Row {
       id: tableRow
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.top: subTitle.bottom
       anchors.topMargin: 20
       spacing: 10
       width: parent.width*0.8
       height: parent.height*0.5

       function disableArchiveButtons(){
          readingColumn.vmEnableDelete = false;
          bindingBCColumn.vmEnableDelete = false;
          disableUnboundButton();          
       }

       function resetIndexes(){
           readingColumn.currentIndex = -1;
           bindingBCColumn.currentIndex = -1;
           bindingUCColumn.currentIndex = -1;
       }

       function disableUnboundButton(){
          bindingUCColumn.vmEnableDelete = false;
       }

       // Reading Column
       Column {
           id: readingColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;
           property int currentIndex: -1

           // The header of the column
           Rectangle {
               id: headerReading
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: 30
               Text {
                   id: patientText
                   text: loader.getStringForKey(keybase+"labelColReading");
                   width: parent.width
                   font.family: gothamB.name
                   font.pixelSize: 15
                   horizontalAlignment: Text.AlignHCenter
                   anchors.verticalCenter: parent.verticalCenter
               }
           }

           // The list view
           Rectangle {
               id: readingBackground
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: parent.height - headerReading.height

               ScrollView {
                   id: tableAreaReading
                   anchors.fill: parent
                   clip: true
                   ListView {
                       id: readingListView
                       anchors.fill: parent
                       model: readingList
                       delegate: VMDatSelectEntry {
                           width: parent.width
                           onSelected: {
                               readingListView.currentIndex = indexInList;
                               readingColumn.vmEnableDelete = true;
                               readingColumn.currentIndex = indexInList;
                               readingSelectionChanged(indexInList)
                           }
                       }
                       onCurrentIndexChanged: {
                           for (var i = 0; i < model.count; i++){
                               if (i != currentIndex){
                                   readingList.setProperty(i,"vmIsSelected",false)
                               }
                           }
                       }
                   }
               }
           }

           // Buffer space
           Rectangle{
               id: readingWhiteSpace
               border.width: 0
               color: "#FFFFFF"
               width: parent.width
               height: 15
           }

           // The delete button.
           VMButton{
               id: btnDeleteReading
               height: 30
               width: parent.width
               vmText: loader.getStringForKey(keybase+"btnDelete");
               vmFont: viewHome.gothamM.name
               vmInvertColors: true
               enabled: readingColumn.vmEnableDelete
               onClicked: {
                   if (readingColumn.currentIndex > -1){
                      var fileName = loader.getDatFileNameFromIndex(readingColumn.currentIndex,vmLIST_INDEX_READING);
                      flowControl.archiveSelectedFile(fileName);
                      relodInformationOnOpened();
                   }
               }
           }

       }

       // Binding BC Column
       Column {
           id: bindingBCColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;
           property int currentIndex: -1

           // The header of the column
           Rectangle {
               id: headerBindingBC
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: 30
               Text {
                   id: headerBindingBCText
                   text: loader.getStringForKey(keybase+"labelColBindingBC");
                   width: parent.width
                   font.family: gothamB.name
                   font.pixelSize: 15
                   horizontalAlignment: Text.AlignHCenter
                   anchors.verticalCenter: parent.verticalCenter
               }
           }

           // The list view
           Rectangle {
               id: bidingBCBackground
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: parent.height - headerReading.height

               ScrollView {
                   id: tableAreaBC
                   anchors.fill: parent
                   clip: true
                   ListView {
                       id: bindingBCListView
                       anchors.fill: parent
                       model: bindingBCList
                       delegate: VMDatSelectEntry {
                           width: parent.width
                           onSelected: {                               
                               bindingBCListView.currentIndex = indexInList;
                               bindingBCColumn.vmEnableDelete = true;
                               bindingBCColumn.currentIndex = indexInList;
                               bindingBCSelectionChanged(indexInList)
                           }
                       }
                       onCurrentIndexChanged: {
                           for (var i = 0; i < model.count; i++){
                               if (i != currentIndex){
                                   bindingBCList.setProperty(i,"vmIsSelected",false)
                               }
                           }
                       }
                   }
               }
           }

           // Buffer space
           Rectangle{
               id: bbcWhiteSpace
               border.width: 0
               color: "#FFFFFF"
               width: parent.width
               height: 15
           }

           // The delete button.
           VMButton{
               id: btnDeleteBBC
               height: 30
               width: parent.width
               vmText: loader.getStringForKey(keybase+"btnDelete");
               vmFont: viewHome.gothamM.name
               vmInvertColors: true
               enabled: bindingBCColumn.vmEnableDelete
               onClicked: {
                   if (bindingBCColumn.currentIndex > -1){
                       var fileName = loader.getDatFileNameFromIndex(bindingBCColumn.currentIndex,vmLIST_INDEX_BINDING_BC);
                       flowControl.archiveSelectedFile(fileName);
                       relodInformationOnOpened();
                   }
               }
           }

       }

       // Binding UC Column
       Column {
           id: bindingUCColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;
           property int currentIndex: -1

           // The header of the column
           Rectangle {
               id: headerBindingUC
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: 30
               Text {
                   id: headerBindingUCText
                   text: loader.getStringForKey(keybase+"labelColBindingUC");
                   width: parent.width
                   font.family: gothamB.name
                   font.pixelSize: 15
                   horizontalAlignment: Text.AlignHCenter
                   anchors.verticalCenter: parent.verticalCenter
               }
           }

           // The list view
           Rectangle {
               id: bidingUCBackground
               color: "#ffffff"
               border.width: 2
               border.color: "#EDEDEE"
               radius: 4
               width: parent.width
               height: parent.height - headerReading.height

               ScrollView {
                   id: tableAreaUC
                   anchors.fill: parent
                   clip: true
                   ListView {
                       id: bindingUCListView
                       anchors.fill: parent
                       model: bindingUCList
                       delegate: VMDatSelectEntry {
                           width: parent.width
                           onSelected: {                               
                               bindingUCListView.currentIndex = indexInList;
                               bindingUCColumn.vmEnableDelete = true;
                               bindingUCColumn.currentIndex = indexInList;
                               bindingUCSelectionChanged(indexInList)
                           }
                       }
                       onCurrentIndexChanged: {
                           for (var i = 0; i < model.count; i++){
                               if (i != currentIndex){
                                   bindingUCList.setProperty(i,"vmIsSelected",false)
                               }
                           }
                       }
                   }
               }
           }

           // Buffer space
           Rectangle{
               id: bucWhiteSpace
               border.width: 0
               color: "#FFFFFF"
               width: parent.width
               height: 15
           }

           // The delete button.
           VMButton{
               id: btnDeleteBUC
               height: 30
               width: parent.width
               vmText: loader.getStringForKey(keybase+"btnDelete");
               vmFont: viewHome.gothamM.name
               vmInvertColors: true
               enabled: bindingUCColumn.vmEnableDelete
               onClicked: {
                   if (bindingUCColumn.currentIndex > -1){
                      var fileName = loader.getDatFileNameFromIndex(bindingUCColumn.currentIndex,vmLIST_INDEX_BINDING_UC);
                      flowControl.archiveSelectedFile(fileName);
                      relodInformationOnOpened();
                   }
               }
           }

       }

    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        spacing: 30

        VMButton{
            id: btnBack
            height: 50
            width: 160
            vmText: loader.getStringForKey(keybase+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                viewSelectDatForReport.close();
            }
        }

        VMButton{
            id: btnGenerate
            height: 50
            width: 160
            vmText: loader.getStringForKey(keybase+"btnGenReport");
            vmFont: viewHome.gothamM.name
            enabled: false
            onClicked: {
                viewPatList.requestReportToServer(0)
                viewSelectDatForReport.close();
            }
        }
    }


}
