import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

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
        btnGenerate.enabled = true;
    }

    function bindingBCSelectionChanged(index){
        // When this is clicked, the generate button is automatically disabled because this means
        // that any UC selection is cleared.
        btnGenerate.enabled = false;
    }

    function bindingUCSelectionChanged(index){
        // When this is clicked then the generate button is enabled, as only valid options are shown.
        btnGenerate.enabled = true;
    }

    onOpened: {
        // Loading the dat lists.
        var readingFiles = loader.getFileListForPatient(0);
        for (var i = 0; i < readingFiles.length; i++){
            readingList.append({"vmDisplayText": readingFiles[i], "vmIndexInList": i, "vmIsSelected" : false});
        }

        var bbcFiles = loader.getFileListForPatient(1);
        for (i = 0; i < bbcFiles.length; i++){
            bindingBCList.append({"vmDisplayText": bbcFiles[i], "vmIndexInList": i, "vmIsSelected" : false});
        }

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

       // Reading Column
       Column {
           id: readingColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;

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
                               readingSelectionChanged(indexInList)
                               readingListView.currentIndex = indexInList;
                               readingColumn.vmEnableDelete = true;
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
               }
           }

       }

       // Binding BC Column
       Column {
           id: bindingBCColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;

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
                               bindingBCSelectionChanged(indexInList)
                               bindingBCListView.currentIndex = indexInList;
                               bindingBCColumn.vmEnableDelete = true;
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
               }
           }

       }

       // Binding UC Column
       Column {
           id: bindingUCColumn
           width: parent.width/3
           height: parent.height

           property bool vmEnableDelete: false;

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
                               bindingUCSelectionChanged(indexInList)
                               bindingUCListView.currentIndex = indexInList;
                               bindingUCColumn.vmEnableDelete = true;
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
               enabled: bindingBCColumn.vmEnableDelete
               onClicked: {
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
            }
        }
    }


}
