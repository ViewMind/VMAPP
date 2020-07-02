import QtQuick 2.6
import QtQuick.Controls 2.3

VMBase {

    id: viewStudyStart
    width: viewStudyStart.vmWIDTH
    height: viewStudyStart.vmHEIGHT

    readonly property string keysearch: "viewstudystart_"
    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0

    property var vmListOfStudiesToSelect: []
    property var vmListOfSelectedStudies: [];

    function setPatientName(){
        var name = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_NAME);
        var uid = loader.getConfigurationString(vmDefines.vmCONFIG_PATIENT_UID);
        var patData = loader.getCurrentPatientInformation();
        if (uimap.getStructure() === "P") labelPatientName.text = patData.firstname + " " + patData.lastname + " (" + patData.displayID + ")";
        else if (uimap.getStructure() === "S") labelPatientName.text = patData.displayID;

    }

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
        }
        else{
            vmListOfSelectedStudies[currentIndex].vmIsSelected = true;
            currentStudy = vmListOfSelectedStudies[currentIndex].vmStudyID;
        }

        // Making everything invisible
        columnReadingLanguage.visible = false;
        columnTargetQuatity.visible = false;
        columnTargetSize.visible = false;

        if (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_READING){
            columnReadingLanguage.visible = true;
        }
        else if ((currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_BC) || (currentStudy === viewPatList.vmDatSelector.vmLIST_INDEX_BINDING_UC)){
            columnTargetQuatity.visible = true;
            columnTargetSize.visible = true;
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

    // Message for study selection
    Text {
        id: labelSelPatient
        font.family: robotoB.name
        font.pixelSize: 15*viewHome.vmScale
        color: "#000000"
        text: {
            if (uimap.getStructure() === "P") return loader.getStringForKey(keysearch+"labelSelPatient");
            if (uimap.getStructure() === "S") return loader.getStringForKey(keysearch+"labelSelSubject");
        }

        anchors.top: viewSubTitle.bottom
        anchors.topMargin: mainWindow.height*0.032
        anchors.left: backgroundPatientName.left
    }

    Rectangle{
        id: backgroundPatientName
        width: mainWindow.width*0.6
        height: mainWindow.height*0.058
        radius: 5
        color: "#ebf3fa"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labelSelPatient.bottom
        anchors.topMargin: mainWindow.height*0.019

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

    Row{
        id: rowStudySelection
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: backgroundPatientName.bottom
        anchors.topMargin: mainWindow.height*0.019
        spacing: (backgroundPatientName.width - (studySelectBackground.width + studySelectedBackground.width + btnAddStudy.width))/2

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
                width: (backgroundPatientName.width - 16)/5
                color: "#554545"
                text: loader.getStringForKey(keysearch+"labelEyeMsg");
            }

            VMComboBox2{
                id: cbEyeMsg
                width: (backgroundPatientName.width - 16)/5
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
                Component.onCompleted: cbReadingLang.setModelList(["English", "Español", "Deutsche", "Français","Íslensku"])
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
                }

                if (vmSelectedExperiments.length > 0){

                    // All is good, so the parameters are set.
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_VALID_EYE,cbEyeMsg.vmCurrentIndex,false);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_NUMBER_OF_TARGETS,cbNumberOfTargets.vmCurrentText,false);
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_BINDING_TARGET_SMALL,(cbTargetSize.vmCurrentIndex == 1),false);

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
                    loader.setValueForConfiguration(vmDefines.vmCONFIG_READING_EXP_LANG,readlang,false);

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
