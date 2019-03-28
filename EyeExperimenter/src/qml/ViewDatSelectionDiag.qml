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

    readonly property string keybase: "viewselectdata_"
    id: viewSelectDatForReport
    modal: true
    width: 700
    height: 650
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

    onOpened: {
        loader.reloadPatientDatInformation();
        reading.clearSelection();
        bindingBC.clearSelection();
        bindingUC.clearSelection();
        btnGenerate.enabled = false;
        loader.operateOnRepGenStruct(-1,-1);

        // Loading the dat lists.
        var readingFiles = loader.getFileListForPatient("",vmLIST_INDEX_READING);
        readingFiles.unshift(loader.getStringForKey(keybase+"labelNoUse"));
        reading.setModelList(readingFiles);

        var bbcFiles = loader.getFileListForPatient("",vmLIST_INDEX_BINDING_BC);
        bbcFiles.unshift(loader.getStringForKey(keybase+"labelNoUse"));
        bindingBC.setModelList(bbcFiles);

        bindingBC.vmEnabled = false;
        bindingUC.vmEnabled = false;
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

    function enableGenerateButtonCheck(){

        var selectedType = [];
        if (reading.vmCurrentIndex > 0) selectedType.push(vmSEL_SELECTED_VALID)
        else selectedType.push(vmSEL_NOT_SELECTED);

        if ((bindingBC.vmCurrentIndex <= 0) && (bindingUC.vmCurrentIndex < 0)) selectedType.push(vmSEL_NOT_SELECTED);
        else if ((bindingBC.vmCurrentIndex > 0) && (bindingUC.vmCurrentIndex >= 0)) selectedType.push(vmSEL_SELECTED_VALID)
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

    function archiveFile(which,index){
        var filename = loader.getDatFileNameFromIndex(index,"",which);
        flowControl.moveFileToArchivedFileFolder(filename)
        viewPatList.loadPatients();
    }

    function doFrequencyAnalysis(which,index){
        var filename = loader.getDatFileNameFromIndex(index,"",which);
        flowControl.doFrequencyAnalysis(filename);
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

    Column {

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: 20
        spacing: 30
        z: 10

        VMDatSelection{
            id: reading;
            vmTitle: loader.getStringForKey(keybase+"labelColReading");
            vmPlaceHolderText: loader.getStringForKey(keybase+"labelSelectOption");
            width: 400
            height: 80
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
            id: bindingBC;
            vmTitle: loader.getStringForKey(keybase+"labelColBindingBC");
            vmPlaceHolderText: loader.getStringForKey(keybase+"labelSelectOption");
            width: 400
            height: 80
            z: 9
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
            width: 400
            height: 80
            z: 8
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
