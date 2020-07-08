import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {


    readonly property int vmLIST_INDEX_READING:    0
    readonly property int vmLIST_INDEX_BINDING_BC: 1
    readonly property int vmLIST_INDEX_BINDING_UC: 2
    readonly property int vmLIST_INDEX_FIELDING:   3
    readonly property int vmLIST_INDEX_NBACKRT:    4

    readonly property string keybase: "viewselectdata_"

    property var vmDatSelection: [];
    property var vmStudiesList: [];
    property bool vmBlockDatSelectionChanged: false
    property int vmCurrentlySelectedStudy: -1

    id: viewSelectDatForReport
    modal: true
    width: mainWindow.width*0.546
    height:  mainWindow.height*0.942
    closePolicy: Popup.NoAutoClose

    property double vmHeightForDatSelector: mainWindow.height*0.07 //0.115

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

    ListModel{
        id: selectedDatList;
    }

    onOpened: {
        loader.reloadPatientDatInformation();
        btnGenerate.enabled = false;
        loader.operateOnRepGenStruct(-1,-1);
        vmDatSelection = [];
        vmCurrentlySelectedStudy = vmLIST_INDEX_READING;

        // Loading the Studies List
        vmStudiesList = loader.getStringListForKey(keybase+"studyList");
        studySelection.setModelList(vmStudiesList);
        for (var i = 0; i < vmStudiesList.length; i++){
            vmDatSelection.push(-1);
        }
        studySelection.setSelected(vmLIST_INDEX_READING);

        fillDatSelector(vmCurrentlySelectedStudy);

        selectedDatList.clear();

        var patname = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
        diagTitle.text = loader.getStringForKey(keybase+"labelTitle") + " - " + patname;

    }

    function getFileList(i){
        var fileList = [];
        if (i === vmLIST_INDEX_BINDING_UC){
            if (vmDatSelection[vmLIST_INDEX_BINDING_BC] !== -1){
               fileList = loader.getFileListCompatibleWithSelectedBC("",vmDatSelection[vmLIST_INDEX_BINDING_BC])
            }
        }
        else{
           fileList = loader.getFileListForPatient("",i);
        }
        return fileList;
    }

    function fillDatSelector(index){

        var filelist = [];
        vmBlockDatSelectionChanged = true;
        vmCurrentlySelectedStudy = index;
        //console.log("Changing currently selected study to " + index)
        datSelector.clearSelection();
        var selIndex = vmDatSelection[index];
        datSelector.visible = true;
        datSelector.vmTitle = vmStudiesList[index];

        filelist = getFileList(index);
        filelist.unshift(loader.getStringForKey(keybase+"labelNoUse"));

        datSelector.setModelList(filelist);
        if (selIndex !== -1){
            datSelector.setSelected(selIndex+1);
        }
        vmBlockDatSelectionChanged = false;

    }

    function datSelectorSelectionChanged(newIndex){

        //selectedDatList.append({"vmDatSelected": datSelector.getCurrentlyTextAt(newIndex)});
        if (vmBlockDatSelectionChanged) return;

        //console.log("Dat selection changed for " + vmCurrentlySelectedStudy);
        newIndex = newIndex - 1;
        vmDatSelection[vmCurrentlySelectedStudy] = newIndex;

        // If BC is changed, uc is unselected.
        if (vmCurrentlySelectedStudy == vmLIST_INDEX_BINDING_BC){
            vmDatSelection[vmLIST_INDEX_BINDING_UC] = -1;
        }

        selectedDatList.clear();

        // Adding all the values that have been selected.
        var enableGenerateReportBtn = false;
        for (var i = 0; i < vmDatSelection.length; i++){
            if (vmDatSelection[i] !== -1){
                enableGenerateReportBtn = true
                var fileList = getFileList(i);
                selectedDatList.append({"vmDatSelected": fileList[vmDatSelection[i]]});
            }
        }

        // This just check that there is at least one selected report
        if (enableGenerateReportBtn){
            enableGenerateReportBtn = false;
            // Checking that either there is no binding selected or that they are both un selected.
            if ((vmDatSelection[vmLIST_INDEX_BINDING_BC] !== -1) && (vmDatSelection[vmLIST_INDEX_BINDING_UC] !== -1)) enableGenerateReportBtn = true;
            else if ((vmDatSelection[vmLIST_INDEX_BINDING_BC] === -1) && (vmDatSelection[vmLIST_INDEX_BINDING_UC] === -1)) enableGenerateReportBtn = true;
        }

        btnGenerate.enabled = enableGenerateReportBtn
    }

    function archiveFile(which,index){
        var filename = loader.getDatFileNameFromIndex(index,"",which);
        flowControl.moveFileToArchivedFileFolder(filename)
        viewPatList.loadPatients();
    }

    function doFrequencyAnalysis(which,index){
        var filename = loader.getDatFileNameFromIndex(index,"",which);
        flowControl.doFrequencyAnalysis(filename);
    }

    function updateNumberOfEvals(){
        diagEvals.text = loader.getStringForKey(keybase+"labelEvals") + ": " + loader.getNumberOfEvalsString(true);
    }

    // The select report title
    Text {
        id: diagTitle
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"labelTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.08
    }

    // The number of available evaluations
    Text {
        id: diagEvals
        font.pixelSize: 15*viewHome.vmScale
        font.family: viewHome.gothamR.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"labelEvals") + ": " + loader.getNumberOfEvalsString(true);
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: mainWindow.height*0.014
    }

    // The subtitle with the explanation
    Text {
        id: subTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"labelSubTitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagEvals.bottom
        anchors.topMargin: mainWindow.height*0.015
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.031
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.019
        onClicked: {
            viewSelectDatForReport.close();
        }
    }

    VMDatSelection{
        id: studySelection;
        vmTitle: loader.getStringForKey(keybase+"labelStudySelection");
        vmPlaceHolderText:  loader.getStringForKey(keybase+"labelSelectOption");
        width: mainWindow.width*0.312
        height: vmHeightForDatSelector
        z: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: mainWindow.height*0.028
        vmHideArchiveButton: true
        vmHideFAButton: true
        vmNoFillOnSelection: true
        onVmCurrentIndexChanged: {
            fillDatSelector(vmCurrentIndex);
        }
    }

    VMDatSelection{
        id: datSelector;
        vmTitle: loader.getStringForKey(keybase+"labelColReading");
        vmPlaceHolderText: loader.getStringForKey(keybase+"labelSelectOption");
        width: mainWindow.width*0.312
        height: vmHeightForDatSelector
        anchors.left: studySelection.left
        anchors.top: studySelection.bottom
        anchors.topMargin: mainWindow.height*0.028 // mainWindow.height*0.043
        z: 9
        onVmCurrentIndexChanged: {
            datSelectorSelectionChanged(vmCurrentIndex);
        }
        onArchiveRequested: {
            if (indexInList > 0){
                viewDatSelectionDiag.close();
                viewPatList.configureShowMessageForArchive(vmCurrentlySelectedStudy,indexInList-1)
            }
        }
        onFrequencyAnalysisRequested: {
            if (indexInList > 0){
                viewDatSelectionDiag.close();
                viewSelectDatForReport.doFrequencyAnalysis(vmCurrentlySelectedStudy,indexInList-1)
            }
        }
    }

    Text {
        id: datSelectedTitle
        font.pixelSize: 11*viewHome.vmScale
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"labeFileView");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: datSelector.bottom
        anchors.topMargin: mainWindow.height*0.06
    }

    Rectangle {
        id: fileViewListBackground
        color: "#ffffff"
        border.width: mainWindow.width*0.001
        border.color: "#3096ef"
        radius: 4
        anchors.top: datSelectedTitle.bottom
        anchors.topMargin:  mainWindow.height*0.015
        anchors.left: datSelector.left
        width: datSelector.width
        height: vmHeightForDatSelector*4;

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: selectedDatListView
                anchors.fill: parent
                model: selectedDatList
                delegate: VMDatSelectedEntry {
                    height: fileViewListBackground.height/4
                    width: parent.width
                }
                onCurrentIndexChanged: {
                }
            }
        }
    }


    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.057
        spacing: mainWindow.width*0.023

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            width: mainWindow.width*0.125
            vmText: loader.getStringForKey(keybase+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                viewSelectDatForReport.close();
            }
        }

        VMButton{
            id: btnGenerate
            height: mainWindow.height*0.072
            width: mainWindow.width*0.125
            vmText: loader.getStringForKey(keybase+"btnGenReport");
            vmFont: viewHome.gothamM.name
            enabled: false
            onClicked: {
                // Storing all the information in the Report Generator Struct and calling the processing.
                for (var i = 0; i < vmDatSelection.length; i++){
                    loader.operateOnRepGenStruct(vmDatSelection[i],i);
                }
                viewPatList.requestReportToServer(0)
                viewSelectDatForReport.close();
            }
        }
    }


}
