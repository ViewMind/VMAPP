import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.0

VMBase {

    id: viewStudyStart
    width: viewStudyStart.vmWIDTH
    height: viewStudyStart.vmHEIGHT

    readonly property string keysearch: "viewstudystart_"
    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0

    property var vmListOfStudiesToSelect: []
    property var vmListOfSelectedStudies: [];

    property var vmMultiPartStudies: [];

    property var vmCurrentMultiPartStudyFile: ""
    property var vmCurrentSelectedStudyName: ""

    Dialog {

        readonly property int vmMP_ACTION_FINALIZE: 1;
        readonly property int vmMP_ACTION_DELETE: 2;

        property string vmMsgTitle: ""
        property string vmMsgText: ""
        property int vmAction: 0

        id: multiPartActionConfirmDiag;
        modal: true
        width: mainWindow.width*0.48
        height: mainWindow.height*0.362
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.NoAutoClose

        contentItem: Rectangle {
            id: rectmultiPartActionConfirmDiag
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        VMDialogCloseButton {
            id: btnClose
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.032
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.02
            onClicked: {
                multiPartActionConfirmDiag.close();
            }
        }

        // The title is the study name.
        Text {
            id: multiPartActionConfirmTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43*viewHome.vmScale
            anchors.top: parent.top
            anchors.topMargin: mainWindow.height*0.072
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: multiPartActionConfirmDiag.vmMsgTitle
        }

        // The instruction text
        Text {
            id: multiPartActionConfirmMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13*viewHome.vmScale
            textFormat: Text.RichText
            anchors.top:  multiPartActionConfirmTitle.bottom
            anchors.topMargin: mainWindow.height*0.029
            anchors.horizontalCenter: parent.horizontalCenter
            text: multiPartActionConfirmDiag.vmMsgText
        }

        VMButton{
            id: btnNegative
            height: mainWindow.height*0.058
            vmText: "No"
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.029
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.039
            onClicked: {
                multiPartActionConfirmDiag.close();
            }
        }

        VMButton{
            id: btnPositive
            height: mainWindow.height*0.058
            vmText: "OK"
            vmFont: viewHome.gothamM.name
            anchors.bottom: parent.bottom
            anchors.bottomMargin: mainWindow.height*0.029
            anchors.right: parent.right
            anchors.rightMargin: mainWindow.width*0.039
            onClicked: {
                if (multiPartActionConfirmDiag.vmAction === multiPartActionConfirmDiag.vmMP_ACTION_DELETE){
                   loader.deleteMultiPartFile(vmCurrentMultiPartStudyFile)
                   swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                   multiPartActionConfirmDiag.close();
                }
                else if (multiPartActionConfirmDiag.vmAction === multiPartActionConfirmDiag.vmMP_ACTION_FINALIZE){
                   loader.finalizeMultiPartFile(vmCurrentMultiPartStudyFile)
                   swiperControl.currentIndex = swiperControl.vmIndexPatientList;
                   multiPartActionConfirmDiag.close();
                }
                else{
                   multiPartActionConfirmDiag.close();
                }
            }
        }

    }


    function setPatientName(){
        var name = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
        var uid = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_UID);
        var patData = loader.getCurrentPatientInformation();
        if (uimap.getStructure() === "P") labelPatientName.text = patData.firstname + " " + patData.lastname + " (" + patData.displayID + ")";
        else if (uimap.getStructure() === "S") labelPatientName.text = patData.displayID;
    }

    // This is the second function that is called when entering study start.
    // Is also used to populate the vmMultiPartStudies structure.
    function setDefaultSelections(){

        var studyList = loader.getStringListForKey("viewselectdata_studyList");
        vmListOfStudiesToSelect = [];
        vmListOfSelectedStudies = [];
        for (var i = 0; i < studyList.length; i++){
            vmListOfStudiesToSelect.push({"vmStudyName" : studyList[i], "vmIsSelected" : false, "vmStudyID" : i, "vmIndex": i})
        }
        fillList(vmListOfStudiesToSelect,studySelectList);
        fillList(vmListOfSelectedStudies,studySelectedList);
        btnStart.enabled = false;

        vmMultiPartStudies = loader.getMultiPartStudies();

    }

    function fillList(objArray,list, clear){
        list.clear();
        for (var i = 0; i < objArray.length; i++){
            if (clear) objArray[i].vmIsSelected = false;
            objArray[i].vmIndex = i;
            list.append(objArray[i]);
        }
    }

    function insertStudyInOrder(studyObj){
        for (var i = 0; i < vmListOfStudiesToSelect.length; i++){
            if (vmListOfStudiesToSelect[i].vmStudyID > studyObj.vmStudyID){
                vmListOfStudiesToSelect.splice(i, 0, studyObj)
                return;
            }
        }
        // If it got to here it should be inserted last.
        vmListOfStudiesToSelect.push(studyObj);
    }

    function sortByStudyIndex(objArray){
        var aSwapWasMade = true;
        while (aSwapWasMade){
            aSwapWasMade = false;
            for (var i = 0; i < objArray.length-1; i++){
                if (objArray[i].vmStudyID > objArray[i+1].vmStudyID){
                    var obj = objArray[i];
                    objArray[i] = objArray[i+1]
                    objArray[i+1] = obj;
                    aSwapWasMade = true;
                }
            }
        }
        return objArray;
    }

    function testPresentExperimentScreen(){

        // Testing READING Screen
        //vmSelectedExperiments.push(viewPatientReg.vmExpIndexReading);
        //loader.setValueForConfiguration(vmDefines.vmCONFIG_READING_EXP_LANG,"de",false);
        vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingBC);
        //vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingUC);
        loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS,"2",false);

        // Testing Binding Screen

        viewPresentExperimet.setTracker(vmSelectedExperiments);
        vmCurrentExperimentIndex = -1;
        viewPresentExperimet.advanceCurrentExperiment()
    }

    function selectionChanged(currentIndex, isStudyList){
        var obj, i;

        for (i = 0; i < vmListOfStudiesToSelect.length; i++){
            vmListOfStudiesToSelect[i].vmIsSelected = false;
        }
        for (i = 0; i < vmListOfSelectedStudies.length; i++){
            vmListOfSelectedStudies[i].vmIsSelected = false;
        }

        // Selecting the one actually clicked.
        var currentStudy = -1;
        if (isStudyList){
            vmListOfStudiesToSelect[currentIndex].vmIsSelected = true;
            currentStudy = vmListOfStudiesToSelect[currentIndex].vmStudyID;
            viewStudyStart.vmCurrentSelectedStudyName = vmListOfStudiesToSelect[currentIndex].vmStudyName
        }
        else{
            vmListOfSelectedStudies[currentIndex].vmIsSelected = true;
            currentStudy = vmListOfSelectedStudies[currentIndex].vmStudyID;
            viewStudyStart.vmCurrentSelectedStudyName = vmListOfSelectedStudies[currentIndex].vmStudyName
        }

        // Making everything invisible
        columnReadingLanguage.visible = false;
        columnTargetQuatity.visible = false;
        columnTargetSize.visible = false;
        columnNumberOfNBackTargets.visible = false;
        columnOfMultiStudiesOptions.visible = false;
        columnOfPerceptionType.visible = false;

        vmCurrentMultiPartStudyFile = "";

        if (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_READING){
            columnReadingLanguage.visible = true;
        }
        else if ((currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC) || (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC)){
            columnTargetQuatity.visible = true;
            columnTargetSize.visible = true;
        }
        else if (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_NBACKVS){
            columnNumberOfNBackTargets.visible = true;
        }
        else if (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_PERCEPTION){

            columnOfPerceptionType.visible = true;

            if (currentStudy in vmMultiPartStudies){

                columnOfMultiStudiesOptions.visible = true;

                // The first value is the file name, the second is the last study part identifier and the third the total number of parts.
                vmCurrentMultiPartStudyFile   = vmMultiPartStudies[currentStudy][0];
                var lastPart                  = parseInt(vmMultiPartStudies[currentStudy][1]);
                var totalParts                = vmMultiPartStudies[currentStudy][2];

                var action_resume = loader.getStringForKey(keysearch+"mpActionResume");

                // I need to add two becuase the last part is the tag of the last part so the next one is +1 but the first one should be 1 and not 0 so +2
                action_resume = action_resume.replace("N",lastPart+2);
                action_resume = action_resume.replace("T",totalParts);

                cbOfMultiStudiesOptions.setModelList([action_resume, loader.getStringForKey(keysearch+"mpActionStartOver"), loader.getStringForKey(keysearch+"mpActionFinalize")]);

            }

        }

        fillList(vmListOfStudiesToSelect,studySelectList, false);
        fillList(vmListOfSelectedStudies,studySelectedList, false);

    }

    function moveFromOneListToAnother(makeSelected){

        var source, dest, modelSource, modelDest;
        if (makeSelected){
            source = vmListOfStudiesToSelect;
            dest   = vmListOfSelectedStudies;
            modelSource = studySelectList;
            modelDest = studySelectedList;
        }
        else{
            source = vmListOfSelectedStudies;
            dest   = vmListOfStudiesToSelect;
            modelSource = studySelectedList;
            modelDest = studySelectList;
        }

        // Finding the selected study
        var selectedIndex = -1;
        for (var i = 0; i < source.length; i++){
            if (source[i].vmIsSelected){
                selectedIndex = i;
                break;
            }
        }
        if (selectedIndex === -1) return;

        var objsToMove = [];

        var obj = source.splice(selectedIndex,1)[0]

        // Making the insertion in the destination list.
        if (makeSelected){
            dest.push(obj);
        }
        else{
            insertStudyInOrder(obj);
        }

        // Finally reindexing both lists
        fillList(source,modelSource,true)
        fillList(dest,modelDest,true);

        if (vmListOfSelectedStudies.length > 0) btnStart.enabled = true;
        else btnStart.enabled = false;

        if (vmListOfSelectedStudies.length >= 4) btnAddStudy.enabled = false;
        else btnAddStudy.enabled = true;

    }

    function studyDesignChangeTriggered(){

        // Clearing the selected studies list.
        while (vmListOfSelectedStudies.length > 0){
            vmListOfSelectedStudies[0].vmIsSelected = true;
            moveFromOneListToAnother(false);
        }

        //vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_FIELDING

        var studiesToAdd = [];

        // Now selecting the studies based on then the current selection.
        switch (cbStudyDesign.vmCurrentIndex){
        case 1: // MCI
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_READING,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];
            break;
        case 2: // Amnestic MCI
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 3: // Mild Alzheimer's
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 4: // Asymptomatic Alzheimer's
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(1); // Three Targets.
            break;
        case 5: // Major Depression
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 6: // MS
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_READING,
                            viewPatList.vmDatSelector.vmLIST_INDEX_FIELDING];
            break;
        case 7: // ADHD
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];
            break;
        case 8: // Parkinson
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];
            break;
        case 9: // Frontotemporal Disease
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];
            break;
        case 10: // COVID-19
            studiesToAdd = [viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC,
                            viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT,
                            viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.

            break;
        }

        // Adding all the studies associated with the condition.
        for (var i = 0; i < studiesToAdd.length; i++){
            for (var j = 0; j < vmListOfStudiesToSelect.length; j++){
                if (vmListOfStudiesToSelect[j].vmStudyID === studiesToAdd[i]){
                    vmListOfStudiesToSelect[j].vmIsSelected = true;
                    moveFromOneListToAnother(true);
                    break;
                }
            }
        }

    }

    ListModel{
        id: studySelectList
    }

    ListModel{
        id: studySelectedList
    }


    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: mainWindow.height*0.032
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#297fca"
        text: loader.getStringForKey(keysearch+"labelTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.016
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#bcbec0"
        text: loader.getStringForKey(keysearch+"labelSubTitle");
    }

    Row {
        id: patientAndConditionSelectionRow
        spacing:  mainWindow.width*0.005
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.032
        anchors.horizontalCenter: parent.horizontalCenter
        z:2

        Column {
            id: patientNameColumn
            spacing: mainWindow.height*0.019
            width: mainWindow.width*0.3

            Text {
                id: labelSelPatient
                font.family: robotoB.name
                font.pixelSize: 15*viewHome.vmScale
                color: "#000000"
                text: {
                    if (uimap.getStructure() === "P") return loader.getStringForKey(keysearch+"labelSelPatient");
                    if (uimap.getStructure() === "S") return loader.getStringForKey(keysearch+"labelSelSubject");
                }

            }

            Rectangle{
                id: backgroundPatientName
                width: parent.width
                height: cbStudyDesign.height
                radius: 5
                color: "#ebf3fa"
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: labelPatientName
                    font.family: robotoR.name
                    font.pixelSize: 15*viewHome.vmScale
                    color: "#58595b"
                    anchors.verticalCenter: backgroundPatientName.verticalCenter
                    text: "";
                    anchors.left: backgroundPatientName.left
                    anchors.leftMargin: mainWindow.width*0.008
                }

            }
        }

        Column {
            id: studyDesignColumn
            spacing: mainWindow.height*0.019
            width: patientNameColumn.width

            Text {
                id: labelStudyDesignCB
                font.family: robotoB.name
                font.pixelSize: 15*viewHome.vmScale
                color: "#000000"
                text: loader.getStringForKey(keysearch+"labelPatientConditionsCB");
            }

            VMComboBox2{
                id: cbStudyDesign
                width: parent.width
                vmEnabled: true
                z:2
                Component.onCompleted: {
                    cbStudyDesign.setModelList(loader.getStringListForKey(keysearch+"patient_conditions"));
                }
                onSelectionChanged: {
                    studyDesignChangeTriggered();
                }
            }


        }

    }

    Row{
        id: rowStudySelection
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: patientAndConditionSelectionRow.bottom
        anchors.topMargin: mainWindow.height*0.019
        spacing: (patientAndConditionSelectionRow.width - (studySelectBackground.width + studySelectedBackground.width + btnAddStudy.width))/2

        Column {
            id: studySelectColumn
            spacing: mainWindow.height*0.01

            Text {
                id: studySelectText
                font.family: robotoR.name
                font.pixelSize: 12*viewHome.vmScale
                anchors.horizontalCenter: studySelectColumn.horizontalCenter
                color: "#bcbec0"
                text: loader.getStringForKey(keysearch+"labelChoose");
            }

            Rectangle {
                id: studySelectBackground
                color: "#ffffff"
                border.width: mainWindow.width*0.001
                border.color: "#EDEDEE"
                radius: 4
                width: mainWindow.width*0.25
                height: mainWindow.height*0.25

                ScrollView {
                    id: studySelectView
                    anchors.fill: parent
                    clip: true
                    ListView {
                        id: studySelectListView
                        anchors.fill: parent
                        model: studySelectList
                        delegate: VMStudyEntry {
                            width: studySelectBackground.width
                            height: studySelectBackground.height/4
                            onItemSelected: {
                                selectionChanged(vmIndex,true);
                            }
                        }
                    }
                }
            }

        }

        Column {
            id : addRemoveButtonsColumn
            spacing: mainWindow.height*0.05
            anchors.verticalCenter: parent.verticalCenter

            VMArrowButton {
                id: btnAddStudy
                height: mainWindow.height*0.05
                width:  mainWindow.width*0.05;
                vmRotate: true
                onClicked: {
                    moveFromOneListToAnother(true);
                }
            }

            VMArrowButton {
                id: btnRemoveStudy
                height: btnAddStudy.height
                width:  btnAddStudy.width;
                onClicked: {
                    moveFromOneListToAnother(false);
                }
            }

        }

        Column {
            id: studySelectedColumn
            spacing: studySelectColumn.spacing

            Text {
                id: studySelectedText
                font.family: robotoR.name
                font.pixelSize: 12*viewHome.vmScale
                anchors.horizontalCenter: studySelectedColumn.horizontalCenter
                color: "#bcbec0"
                text: loader.getStringForKey(keysearch+"labelEvaluations");
            }

            Rectangle {
                id: studySelectedBackground
                color: "#ffffff"
                border.width:  mainWindow.width*0.001
                border.color: "#EDEDEE"
                radius: 4
                width: studySelectBackground.width
                height: studySelectBackground.height

                ScrollView {
                    id: studySelecedtView
                    anchors.fill: parent
                    clip: true
                    ListView {
                        id: studySelectedListView
                        anchors.fill: parent
                        model: studySelectedList
                        delegate: VMStudyEntry {
                            width: studySelectBackground.width
                            height: studySelectBackground.height/4
                            onItemSelected: {
                                selectionChanged(vmIndex,false);
                            }
                        }
                    }
                }
            }

        }


    }

    // Message for configuring experiment options
    Text {
        id: labelInstruction2
        font.family: robotoB.name
        font.pixelSize: 15*viewHome.vmScale
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rowStudySelection.bottom
        anchors.topMargin:  mainWindow.height*0.010
        color: "#000000"
        text: loader.getStringForKey(keysearch+"labelInstruction2");
    }

    Row {
        id: studyOptions
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labelInstruction2.bottom
        anchors.topMargin: mainWindow.height*0.03
        spacing: mainWindow.width*0.01
        z: 2

        Column {
            id: columnEyeSelection
            spacing: mainWindow.height*0.019
            z: 2
            Text {
                id: labelEyeMsg
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: (patientAndConditionSelectionRow.width - 16)/5
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelEyeMsg");
            }

            VMComboBox2{
                id: cbEyeMsg
                width: (patientAndConditionSelectionRow.width - 16)/5
                vmEnabled: true
                z:2
                Component.onCompleted: {
                    cbEyeMsg.setModelList(loader.getStringListForKey(keysearch+"labelEyeType"));
                    cbEyeMsg.setSelection(2)
                }
            }
        }

        Column {
            id: columnTargetQuatity
            spacing: mainWindow.height*0.019
            z: 2
            visible: false
            Text {
                id: labelNTargets
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelNTargets");
            }

            VMComboBox2{
                id: cbNumberOfTargets
                width: cbEyeMsg.width
                z:2
                vmEnabled: {
                    if (uimap.getBlockedBindCount() === "N") return true;
                    else if (uimap.getBlockedBindCount() === "Y") return false;
                }
                Component.onCompleted: {
                    if (uimap.getBindDefaultCount() === "2") cbNumberOfTargets.setModelList(["2","3"]);
                    else if (uimap.getBindDefaultCount() === "3") cbNumberOfTargets.setModelList(["3","2"]);
                }
            }
        }

        Column {
            id: columnTargetSize
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelTargetSize
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelTargetSize");
            }

            VMComboBox2{
                id: cbTargetSize
                width: cbEyeMsg.width
                z:2
                vmEnabled: {
                    if (uimap.getBlockedBindSize() === "N") return true;
                    else if (uimap.getBlockedBindSize() === "Y") return false;
                }
                Component.onCompleted: cbTargetSize.setModelList(loader.getStringListForKey(keysearch+"targetOptions"))
            }
        }

        Column {
            id: columnNumberOfNBackTargets
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelNumberOfNBackTargets
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelNumberOfNBackTargets");
            }

            VMComboBox2{
                id: cbNumberOfNBAckTargets
                width: cbEyeMsg.width
                z:2
                Component.onCompleted: cbNumberOfNBAckTargets.setModelList(["3","4","5","6"])
            }
        }

        Column {
            id: columnReadingLanguage
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelReadingLanguge
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelReadingLanguge");
            }

            VMComboBox2{
                id: cbReadingLang
                width: cbEyeMsg.width
                vmMaxDisplayItems: 3
                z:2
                Component.onCompleted: {
                    cbReadingLang.setModelList(["English", "Español", "Deutsche", "Français","Íslensku"])

                    // Loading the last study language.
                    var index = loader.getConfigurationString(vmDefines.vmCONFIG_DEFAULT_READING_LANGUAGE)
                    if (index !== ""){
                        cbReadingLang.setSelection(index);
                    }

                }
                onSelectionChanged: {
                    loader.setSettingsValue(vmDefines.vmCONFIG_DEFAULT_READING_LANGUAGE,cbReadingLang.vmCurrentIndex);
                }
            }
        }

        Column {
            id: columnOfMultiStudiesOptions
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelOfMultiStudiesOptions
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelActions");
            }

            VMComboBox2{
                id: cbOfMultiStudiesOptions
                width: cbEyeMsg.width
                z:2
                onSelectionChanged: {
                    multiPartActionConfirmDiag.vmMsgTitle = viewStudyStart.vmCurrentSelectedStudyName
                    if (cbOfMultiStudiesOptions.vmCurrentIndex == 1){
                        multiPartActionConfirmDiag.vmAction = multiPartActionConfirmDiag.vmMP_ACTION_DELETE;
                        multiPartActionConfirmDiag.vmMsgText = loader.getStringForKey(keysearch + "confirmMPRestart")
                        multiPartActionConfirmDiag.open()
                    }
                    else if (cbOfMultiStudiesOptions.vmCurrentIndex == 2){
                        multiPartActionConfirmDiag.vmAction = multiPartActionConfirmDiag.vmMP_ACTION_FINALIZE;
                        multiPartActionConfirmDiag.vmMsgText = loader.getStringForKey(keysearch + "confirmMPFinalize")
                        multiPartActionConfirmDiag.open()
                    }
                }
            }
        }

        Column {
            id: columnOfPerceptionType
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelOfPerceptionType
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelPerceptionType");
            }

            VMComboBox2{
                id: cbOfPerceptionType
                width: cbEyeMsg.width
                z:2
                Component.onCompleted: cbOfPerceptionType.setModelList(loader.getStringListForKey(keysearch+"perceptionTypeOptions"));
            }
        }

    }

    // Buttons
    Row{
        id: buttonRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mainWindow.height*0.012
        spacing: mainWindow.width*0.0023

        VMButton{
            id: btnBack
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keysearch+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnStart
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey(keysearch+"btnStart");
            vmFont: viewHome.gothamM.name
            onClicked: {

                vmSelectedExperiments = [];

                for (var i = 0; i < vmListOfSelectedStudies.length; i++){
                    if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingBC);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_READING){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexReading);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexBindingUC);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_FIELDING){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexFielding);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_NBACKRT){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexNBackRT);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_GONOGO){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexGoNoGo);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_NBACKVS){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexNBackVS);
                    }
                    else if (vmListOfSelectedStudies[i].vmStudyID === viewPatList.vmDatSelector.vmLIST_INDEX_PERCEPTION){
                        vmSelectedExperiments.push(viewPatientReg.vmExpIndexPerception);

                        if (viewPatList.vmDatSelector.vmLIST_INDEX_PERCEPTION in vmMultiPartStudies){

                            var filename                  = vmMultiPartStudies[viewPatList.vmDatSelector.vmLIST_INDEX_PERCEPTION][0];
                            var lastPart                  = vmMultiPartStudies[viewPatList.vmDatSelector.vmLIST_INDEX_PERCEPTION][1];

                            loader.setValueForConfiguration(vmDefines.vmCONFIG_PERCEPTION_MP_CURRENT_STUDY_FILE,filename);
                            loader.setValueForConfiguration(vmDefines.vmCONFIG_PERCEPTION_MP_CURRENT_IDENTIFIER,lastPart);
                        }

                        loader.setValueForConfiguration(vmDefines.vmCONFIG_PERCEPTION_IS_TRAINING,(cbOfPerceptionType.vmCurrentIndex === 0));

                    }
                }

                if (vmSelectedExperiments.length > 0){

                    // All is good, so the parameters are set.
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_VALID_EYE,cbEyeMsg.vmCurrentIndex);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS,cbNumberOfTargets.vmCurrentText);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_TARGET_SMALL,(cbTargetSize.vmCurrentIndex == 1));
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_NBACKVS_SEQUENCE_LENGTH,(cbNumberOfNBAckTargets.vmCurrentIndex+3));

                    var readlang;
                    switch(cbReadingLang.vmCurrentIndex){
                    case 0:
                        readlang = "en";
                        break;
                    case 1:
                        readlang = "es";
                        break;
                    case 2:
                        readlang = "de"
                        break;
                    case 3:
                        readlang = "fr";
                        break;
                    case 4:
                        readlang = "is";
                        break;
                    }
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_READING_EXP_LANG,readlang);

                    viewPresentExperimet.setTracker(vmSelectedExperiments);
                    vmCurrentExperimentIndex = -1;
                    viewPresentExperimet.advanceCurrentExperiment()
                    if (loader.getConfigurationString(vmDefines.vmCONFIG_SELECTED_ET) === vmDefines.vmCONFIG_P_ET_HTCVIVEEYEPRO){
                        swiperControl.currentIndex = swiperControl.vmIndexPresentExperiment
                        viewVRDisplay.disableStartStudyButton(); // To enforce first calibraton.
                    }
                    else swiperControl.currentIndex = swiperControl.vmIndexCalibrationStart
                }
                else{
                    labelNoInstructionSetError.visible = true;
                }

            }
        }
    }


}
