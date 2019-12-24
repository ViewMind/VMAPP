import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {


    readonly property int vmLIST_INDEX_READING: 0
    readonly property int vmLIST_INDEX_BINDING_BC: 1
    readonly property int vmLIST_INDEX_BINDING_UC: 2
    readonly property int vmLIST_INDEX_FIELDING: 3

    readonly property int vmSEL_NOT_SELECTED:     0
    readonly property int vmSEL_SELECTED_VALID:   1
    readonly property int vmSEL_SELECTED_INVALID: 2

    readonly property string keybase: "viewselectdata_"
    id: viewSelectDatForReport
    modal: true
    width: mainWindow.width*0.546
    height:  mainWindow.height*0.942
    closePolicy: Popup.NoAutoClose

    property double vmHeightForDatSelector: mainWindow.height*0.09 //0.115

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

    onOpened: {
        loader.reloadPatientDatInformation();
        reading.clearSelection();
        bindingBC.clearSelection();
        bindingUC.clearSelection();
        fielding.clearSelection();
        btnGenerate.enabled = false;
        loader.operateOnRepGenStruct(-1,-1);

        // Loading the dat lists.
        var readingFiles = loader.getFileListForPatient("",vmLIST_INDEX_READING);
        readingFiles.unshift(loader.getStringForKey(keybase+"labelNoUse"));
        reading.setModelList(readingFiles);

        var fieldingFiles = loader.getFileListForPatient("",vmLIST_INDEX_FIELDING);
        fieldingFiles.unshift(loader.getStringForKey(keybase+"labelNoUse"));
        fielding.setModelList(fieldingFiles);

        var bbcFiles = loader.getFileListForPatient("",vmLIST_INDEX_BINDING_BC);
        bbcFiles.unshift(loader.getStringForKey(keybase+"labelNoUse"));
        bindingBC.setModelList(bbcFiles);

        bindingBC.vmEnabled = false;
        bindingUC.vmEnabled = false;

        var patname = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
        diagTitle.text = loader.getStringForKey(keybase+"labelTitle") + " - " + patname;

    }

    function readingSelectionChanged(index){
        // Since it's not possible to unselect once selected, the generate button is enabled.
        enableGenerateButtonCheck();
        if (index > 0) loader.operateOnRepGenStruct(index-1,vmLIST_INDEX_READING)
        bindingBC.vmEnabled = true;
    }

    function bindingBCSelectionChanged(index){
        // When this is clicked, the generate button is automatically disabled because this means
        // that any UC selection is cleared.
        bindingUC.clearSelection();
        enableGenerateButtonCheck();
        if (index <= 0) {
            bindingUC.vmEnabled = false;
            return;
        }
        var bindingUCFiles = loader.getFileListCompatibleWithSelectedBC("",index-1);
        if (bindingUCFiles.length > 0){
            bindingUC.setModelList(bindingUCFiles)
            loader.operateOnRepGenStruct(index-1,vmLIST_INDEX_BINDING_BC)
            bindingUC.vmEnabled = true;
        }
        else bindingUC.vmEnabled = false;
    }

    function bindingUCSelectionChanged(index){
        // When this is clicked then the generate button is enabled, as only valid options are shown.
        enableGenerateButtonCheck();
        loader.operateOnRepGenStruct(index,vmLIST_INDEX_BINDING_UC)
    }

    function fieldingSelectionChanged(index){
        enableGenerateButtonCheck();
        if (index > 0) loader.operateOnRepGenStruct(index-1,vmLIST_INDEX_FIELDING)
    }

    function enableGenerateButtonCheck(){

        var selectedType = [];
        if (reading.vmCurrentIndex > 0) selectedType.push(vmSEL_SELECTED_VALID)
        else selectedType.push(vmSEL_NOT_SELECTED);

        if ((bindingBC.vmCurrentIndex <= 0) && (bindingUC.vmCurrentIndex < 0)) selectedType.push(vmSEL_NOT_SELECTED);
        else if ((bindingBC.vmCurrentIndex > 0) && (bindingUC.vmCurrentIndex >= 0)) selectedType.push(vmSEL_SELECTED_VALID)
        else selectedType.push(vmSEL_SELECTED_INVALID);

        if (fielding.vmCurrentIndex > 0) selectedType.push(vmSEL_SELECTED_VALID);
        else selectedType.push(vmSEL_NOT_SELECTED);

        btnGenerate.enabled = false;
        for (var i = 0; i < selectedType.length; i++){
            if (selectedType[i] === vmSEL_SELECTED_INVALID) {
                btnGenerate.enabled = false;
                return;
            }
            if (selectedType[i] === vmSEL_SELECTED_VALID) btnGenerate.enabled = true;
        }
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

    Column {

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: mainWindow.height*0.028
        spacing: mainWindow.height*0.043
        z: 10

        VMDatSelection{
            id: reading;
            vmTitle: loader.getStringForKey(keybase+"labelColReading");
            vmPlaceHolderText: loader.getStringForKey(keybase+"labelSelectOption");
            width: mainWindow.width*0.312
            height: vmHeightForDatSelector
            z: 10
            onVmCurrentIndexChanged: {
                readingSelectionChanged(reading.vmCurrentIndex);
            }
            onArchiveRequested: {
                if (indexInList > 0){
                    viewDatSelectionDiag.close();
                    viewPatList.configureShowMessageForArchive(vmLIST_INDEX_READING,indexInList-1)
                }
            }
            onFrequencyAnalysisRequested: {
                if (indexInList > 0){
                    viewDatSelectionDiag.close();
                    viewSelectDatForReport.doFrequencyAnalysis(vmLIST_INDEX_READING,indexInList-1)
                }
            }
        }


        VMDatSelection{
            id: fielding
            vmTitle: loader.getStringForKey(keybase+"labelFielding");
            vmPlaceHolderText:  loader.getStringForKey(keybase+"labelSelectOption");
            width: mainWindow.width*0.312
            height: vmHeightForDatSelector
            z: 9
            onVmCurrentIndexChanged: {
                fieldingSelectionChanged(fielding.vmCurrentIndex);
            }
            onArchiveRequested: {
                viewDatSelectionDiag.close();
                viewPatList.configureShowMessageForArchive(vmLIST_INDEX_FIELDING,indexInList-1)
            }
            onFrequencyAnalysisRequested: {
                viewDatSelectionDiag.close();
                viewSelectDatForReport.doFrequencyAnalysis(vmLIST_INDEX_FIELDING,indexInList-1)
            }
        }

        VMDatSelection{
            id: bindingBC;
            vmTitle: loader.getStringForKey(keybase+"labelColBindingBC");
            vmPlaceHolderText: loader.getStringForKey(keybase+"labelSelectOption");
            width: mainWindow.width*0.312
            height: vmHeightForDatSelector
            z: 8
            onVmCurrentIndexChanged: {
                bindingBCSelectionChanged(bindingBC.vmCurrentIndex);
            }
            onArchiveRequested: {
                if (indexInList > 0){
                    viewDatSelectionDiag.close();
                    viewPatList.configureShowMessageForArchive(vmLIST_INDEX_BINDING_BC,indexInList-1)
                }
            }
            onFrequencyAnalysisRequested: {
                if (indexInList > 0){
                    viewDatSelectionDiag.close();
                    viewSelectDatForReport.doFrequencyAnalysis(vmLIST_INDEX_BINDING_BC,indexInList-1)
                }
            }
        }

        VMDatSelection{
            id: bindingUC;
            vmTitle: loader.getStringForKey(keybase+"labelColBindingUC");
            vmPlaceHolderText:  loader.getStringForKey(keybase+"labelSelectOption");
            width: mainWindow.width*0.312
            height: vmHeightForDatSelector
            z: 7
            onVmCurrentIndexChanged: {
                bindingUCSelectionChanged(bindingUC.vmCurrentIndex);
            }
            onArchiveRequested: {
                viewDatSelectionDiag.close();
                viewPatList.configureShowMessageForArchive(vmLIST_INDEX_BINDING_UC,indexInList)
            }
            onFrequencyAnalysisRequested: {
                viewDatSelectionDiag.close();
                viewSelectDatForReport.doFrequencyAnalysis(vmLIST_INDEX_BINDING_UC,indexInList)
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
                viewPatList.requestReportToServer(0)
                viewSelectDatForReport.close();
            }
        }
    }


}
