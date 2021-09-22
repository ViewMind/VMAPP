import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

VMBase {

    id: viewStudyStart
    width: viewStudyStart.vmWIDTH
    height: viewStudyStart.vmHEIGHT

    // The following read only properties corresponds to the exact strings used by the RawDataContainer
    readonly property string vmSTUDY_BINDING:                "Binding";
    readonly property string vmSTUDY_GONOGO :                "Go No-Go";
    readonly property string vmSTUDY_NBACKMS :               "NBack MS";
    readonly property string vmSTUDY_NBACKRT:                "NBack RT";
    readonly property string vmSTUDY_NBACKVS:                "NBack VS";
    readonly property string vmSTUDY_PERCEPTION:             "Perception";
    readonly property string vmSTUDY_READING:                "Reading";

    // Study configuration parameter names.
    readonly property string vmSCP_EYES:                     "valid_eye";
    readonly property string vmSCP_LANGUAGE:                 "language";
    readonly property string vmSCP_NUMBER_OF_TARGETS:        "number_targets";
    readonly property string vmSCP_TARGET_SIZE:              "target_size";
    readonly property string vmSCP_BINDING_TYPE:             "binding_type";
    readonly property string vmSCP_PERCEPTION_PART:          "perception_part";

    // Study configuration parameter values
    readonly property string vmSCV_BINDING_TARGETS_2:        "2";
    readonly property string vmSCV_BINDING_TARGETS_3:        "3";
    readonly property string vmSCV_BINDING_TARGETS_LARGE:    "large";
    readonly property string vmSCV_BINDING_TARGETS_SMALL:    "small";
    readonly property string vmSCV_EYE_BOTH:                 "both";
    readonly property string vmSCV_EYE_LEFT:                 "left";
    readonly property string vmSCV_EYE_RIGHT:                "right";
    readonly property string vmSCV_LANG_DE:                  "German";
    readonly property string vmSCV_LANG_ES:                  "Spanish";
    readonly property string vmSCV_LANG_FR:                  "French";
    readonly property string vmSCV_LANG_EN:                  "English";
    readonly property string vmSCV_LANG_IS:                  "Iselandic";
    readonly property string vmSCV_BINDING_TYPE_BOUND:       "bound";
    readonly property string vmSCV_BINDING_TYPE_UNBOUND:     "unbound";
    readonly property string vmSCV_PERCEPTION_TYPE_TRAINING: "training";
    readonly property string vmSCV_PERCEPTION_TYPE_REHAB:    "rehab";
    readonly property string vmSCV_NBACKVS_TARGETS_3:        "3";
    readonly property string vmSCV_NBACKVS_TARGETS_4:        "4";
    readonly property string vmSCV_NBACKVS_TARGETS_5:        "5";
    readonly property string vmSCV_NBACKVS_TARGETS_6:        "6";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_1:  "Part1";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_2:  "Part2";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_3:  "Part3";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_4:  "Part4";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_5:  "Part5";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_6:  "Part6";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_7:  "Part7";
    readonly property string vmSCV_PERCEPTION_STUDY_PART_8:  "Part8";

    // This is only for UI unique identification
    readonly property string vmUNIQUE_STUDY_ID :             "unique_study_id";

    // Unique value index for each experiment
    readonly property int vmINDEX_READING:                    0
    readonly property int vmINDEX_BINDING_BC:                 1
    readonly property int vmINDEX_BINDING_UC:                 2
    readonly property int vmINDEX_NBACKMS:                    3
    readonly property int vmINDEX_NBACKRT:                    4
    readonly property int vmINDEX_NBACKVS:                    5
    readonly property int vmINDEX_PERCEPTION:                 6
    readonly property int vmINDEX_GONOGO:                     7

    property var vmSelectedExperiments: []
    property int vmCurrentExperimentIndex: 0

    property var vmListOfStudiesToSelect: []
    property var vmListOfSelectedStudies: [];

    //property var vmCurrentMultiPartStudyFile: ""
    property string vmCurrentSelectedStudyName: ""

    property var vmDisabledStudies: [vmINDEX_NBACKVS, vmINDEX_PERCEPTION, vmINDEX_NBACKMS, vmINDEX_READING];

    property string vmSelectedProtocol: ""
    property string vmSelectedMedic: ""



    function setPatientName(){
        var subject_info = loader.getCurrentSubjectInfo();
        if (subject_info.name === "") labelPatientName.text = subject_info.supplied_institution_id
        else labelPatientName.text = subject_info.name + " " + subject_info.lastname;
    }

    // This is the second function that is called when entering study start.
    function setDefaultSelections(){

        var studyList = loader.getStringListForKey("viewstudystart_studyList");
        vmListOfStudiesToSelect = [];
        vmListOfSelectedStudies = [];

        //console.log("Will check with " + vmDisabledStudies);

        for (var i = 0; i < studyList.length; i++){
            if (vmDisabledStudies.includes(i)) {
                //console.log(i + " is in disabled");
                continue;
            }
            vmListOfStudiesToSelect.push({"vmStudyName" : studyList[i], "vmIsSelected" : false, "vmStudyID" : i, "vmIndex": i})
        }
        fillList(vmListOfStudiesToSelect,studySelectList);
        fillList(vmListOfSelectedStudies,studySelectedList);
        btnStart.enabled = false;
        btnAddStudy.enabled = true;

        ////////////////////// TODO THIS IS CRUDE: IMPROVE ///////////////////////////////
        // Filling the part selection on perception.
        var model = Array();
        for (i = 0; i < 8; i++){
            model.push((i+1))
        }
        cbofPerceptionPart.setModelList(model);

        // Getting the marker for the study
        var perception_marker = loader.getStudyMarkerFor(vmSTUDY_PERCEPTION);
        if ((perception_marker >= 0) && (perception_marker < 7)){
            perception_marker++; // Next part.
            cbofPerceptionPart.setSelection(perception_marker);
        }
        ////////////////////// TODO THIS IS CRUDE: IMPROVE ///////////////////////////////

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
        columnOfPerceptionType.visible = false;

        //vmCurrentMultiPartStudyFile = "";

        if (currentStudy === vmINDEX_READING){
            columnReadingLanguage.visible = true;
        }
        else if ((currentStudy === vmINDEX_BINDING_BC) || (currentStudy === vmINDEX_BINDING_UC)){
            columnTargetQuatity.visible = true;
            //columnTargetSize.visible = true;
        }
        else if (currentStudy === vmINDEX_NBACKVS){
            columnNumberOfNBackTargets.visible = true;
        }
        else if (currentStudy === vmINDEX_PERCEPTION){

            columnOfPerceptionType.visible = true;

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

        //vmListOfSelectedStudies[i].vmStudyID === vmINDEX_NBACKMS

        var studiesToAdd = [];

        // Now selecting the studies based on then the current selection.
        switch (cbStudyDesign.vmCurrentIndex){
        case 1: // MCI
            studiesToAdd = [vmINDEX_READING,
                            vmINDEX_GONOGO];
            break;
        case 2: // Amnestic MCI
            studiesToAdd = [vmINDEX_BINDING_BC,
                            vmINDEX_BINDING_UC,
                            vmINDEX_GONOGO];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 3: // Mild Alzheimer's
            studiesToAdd = [vmINDEX_BINDING_BC,
                            vmINDEX_BINDING_UC,
                            vmINDEX_GONOGO];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 4: // Asymptomatic Alzheimer's
            studiesToAdd = [vmINDEX_BINDING_BC,
                            vmINDEX_BINDING_UC,
                            vmINDEX_NBACKRT];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(1); // Three Targets.
            break;
        case 5: // Major Depression
            studiesToAdd = [vmINDEX_BINDING_BC,
                            vmINDEX_BINDING_UC,
                            vmINDEX_NBACKRT];

            cbTargetSize.setSelection(0); // Large
            cbNumberOfTargets.setSelection(0); // Two Targets.
            break;
        case 6: // MS
            studiesToAdd = [vmINDEX_READING,
                            vmINDEX_NBACKMS];
            break;
        case 7: // ADHD
            studiesToAdd = [vmINDEX_NBACKRT,
                            vmINDEX_GONOGO];
            break;
        case 8: // Parkinson
            studiesToAdd = [vmINDEX_NBACKRT,
                            vmINDEX_GONOGO];
            break;
        case 9: // Frontotemporal Disease
            studiesToAdd = [vmINDEX_NBACKRT,
                            vmINDEX_GONOGO];
            break;
        case 10: // COVID-19
            studiesToAdd = [vmINDEX_BINDING_BC,
                            vmINDEX_BINDING_UC,
                            vmINDEX_NBACKRT,
                            vmINDEX_GONOGO];

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
        text: loader.getStringForKey("viewstudystart_labelTitle");
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: mainWindow.height*0.016
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#bcbec0"
        text: loader.getStringForKey("viewstudystart_labelSubTitle");
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
                text: loader.getStringForKey("viewstudystart_labelSelSubject");
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
                text: loader.getStringForKey("viewstudystart_labelPatientConditionsCB");
            }

            VMComboBox2{
                id: cbStudyDesign
                width: parent.width
                vmEnabled: false
                z:2
                Component.onCompleted: {
                    cbStudyDesign.setModelList(loader.getStringListForKey("viewstudystart_patient_conditions"));
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
                text: loader.getStringForKey("viewstudystart_labelChoose");
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
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
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
                text: loader.getStringForKey("viewstudystart_labelEvaluations");
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
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
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
        text: loader.getStringForKey("viewstudystart_labelInstruction2");
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
                text: loader.getStringForKey("viewstudystart_labelEyeMsg");
            }

            VMComboBox2{
                id: cbEyeMsg
                width: (patientAndConditionSelectionRow.width - 16)/5
                vmEnabled: true
                z:2
                Component.onCompleted: {
                    cbEyeMsg.setModelList(loader.getStringListForKey("viewstudystart_labelEyeType"));
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
                text: loader.getStringForKey("viewstudystart_labelNTargets");
            }

            VMComboBox2 {
                id: cbNumberOfTargets
                width: cbEyeMsg.width
                z:2
                Component.onCompleted:  cbNumberOfTargets.setModelList(["2","3"]);
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
                text: loader.getStringForKey("viewstudystart_labelTargetSize");
            }

            VMComboBox2{
                id: cbTargetSize
                width: cbEyeMsg.width
                z:2
                Component.onCompleted: cbTargetSize.setModelList(loader.getStringListForKey("viewstudystart_targetOptions"))
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
                text: loader.getStringForKey("viewstudystart_labelNumberOfNBackTargets");
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
                text: loader.getStringForKey("viewstudystart_labelReadingLanguge");
            }

            VMComboBox2{
                id: cbReadingLang
                width: cbEyeMsg.width
                vmMaxDisplayItems: 3
                z:2
                Component.onCompleted: {
                    cbReadingLang.setModelList(["English", "Español", "Deutsche", "Français","Íslensku"])

                    // Loading the last study language.
                    var index = loader.getConfigurationString("default_reading_study_language");
                    if (index !== ""){
                        cbReadingLang.setSelection(index);
                    }

                }
                onSelectionChanged: {
                    loader.setSettingsValue("default_reading_study_language",cbReadingLang.vmCurrentIndex);
                }
            }
        }

        Column {
            id: columnOfPerceptionPartSelection
            spacing: mainWindow.height*0.019
            visible: false
            Text {
                id: labelOfMultiStudiesOptions
                font.family: robotoB.name
                font.pixelSize: 13*viewHome.vmScale
                width: cbNumberOfTargets.width
                color: "#554545"
                text: loader.getStringForKey("viewstudystart_labelActions");
            }

            VMComboBox2{
                id: cbofPerceptionPart
                width: cbEyeMsg.width
                z:2
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
                text: loader.getStringForKey("viewstudystart_labelPerceptionType");
            }

            VMComboBox2{
                id: cbOfPerceptionType
                width: cbEyeMsg.width
                z:2
                Component.onCompleted: cbOfPerceptionType.setModelList(loader.getStringListForKey("viewstudystart_perceptionTypeOptions"));
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
            vmText: loader.getStringForKey("viewstudystart_btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: btnStart
            height: mainWindow.height*0.072
            vmText: loader.getStringForKey("viewstudystart_btnStart");
            vmFont: viewHome.gothamM.name
            onClicked: {

                vmSelectedExperiments = [];

                //console.log(JSON.stringify(vmListOfSelectedStudies));

                for (var i = 0; i < vmListOfSelectedStudies.length; i++){

                    var configuration_study_map = {};
                    configuration_study_map[vmUNIQUE_STUDY_ID] = vmListOfSelectedStudies[i].vmStudyID;

                    // Setting the selected EYE.
                    switch (cbEyeMsg.vmCurrentIndex){
                    case 0:
                        configuration_study_map[vmSCP_EYES] = vmSCV_EYE_LEFT;
                        break;
                    case 1:
                        configuration_study_map[vmSCP_EYES] = vmSCV_EYE_RIGHT;
                        break;
                    case 2:
                        configuration_study_map[vmSCP_EYES] = vmSCV_EYE_BOTH;
                        break;
                    }

                    if (vmListOfSelectedStudies[i].vmStudyID === vmINDEX_NBACKVS){
                        switch (cbNumberOfNBAckTargets.vmCurrentIndex){
                        case 0: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_NBACKVS_TARGETS_3
                            break;
                        case 1: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_NBACKVS_TARGETS_4
                            break;
                        case 2: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_NBACKVS_TARGETS_5
                            break;
                        case 3: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_NBACKVS_TARGETS_6
                            break;
                        }
                    }

                    else if (vmListOfSelectedStudies[i].vmStudyID === vmINDEX_READING){
                        switch(cbReadingLang.vmCurrentIndex){
                        case 0:configuration_study_map[vmSCP_LANGUAGE] = vmSCV_LANG_EN;
                            break;
                        case 1: configuration_study_map[vmSCP_LANGUAGE] = vmSCV_LANG_ES;
                            break;
                        case 2: configuration_study_map[vmSCP_LANGUAGE] = vmSCV_LANG_DE;
                            break;
                        case 3: configuration_study_map[vmSCP_LANGUAGE] = vmSCV_LANG_FR;
                            break;
                        case 4: configuration_study_map[vmSCP_LANGUAGE] = vmSCV_LANG_IS;
                            break;
                        }
                    }

                    else if ((vmListOfSelectedStudies[i].vmStudyID === vmINDEX_BINDING_BC) || (vmListOfSelectedStudies[i].vmStudyID === vmINDEX_BINDING_UC)){
                        switch(cbNumberOfTargets.vmCurrentIndex){
                        case 0: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_BINDING_TARGETS_2
                            break;
                        case 1: configuration_study_map[vmSCP_NUMBER_OF_TARGETS] = vmSCV_BINDING_TARGETS_3
                            break;
                        }
                        // Target size selection is disabled for now. So we make sure targets are always large.
                        configuration_study_map[vmSCP_TARGET_SIZE] = vmSCV_BINDING_TARGETS_LARGE

                        //switch(cbTargetSize.vmCurrentIndex){
                        //case 0: configuration_study_map[vmSCP_TARGET_SIZE] = vmSCV_BINDING_TARGETS_LARGE
                        //    break;
                        //case 1: configuration_study_map[vmSCP_TARGET_SIZE] = vmSCV_BINDING_TARGETS_SMALL
                        //    break;
                        //}

                        if (vmListOfSelectedStudies[i].vmStudyID === vmINDEX_BINDING_BC) configuration_study_map[vmSCP_BINDING_TYPE] = vmSCV_BINDING_TYPE_BOUND;
                        else configuration_study_map[vmSCP_BINDING_TYPE] = vmSCV_BINDING_TYPE_UNBOUND;
                    }

                    // Specific configuration if
                    else if (vmListOfSelectedStudies[i].vmStudyID === vmINDEX_PERCEPTION){
                        var part = cbofPerceptionPart.vmCurrentIndex + 1;
                        configuration_study_map[vmSCP_PERCEPTION_PART] = "Part" + part;
                        // Storing the marker for the "NEXT" part.
                        loader.setStudyMarkerFor(vmSTUDY_PERCEPTION,cbofPerceptionPart.vmCurrentIndex);
                    }

                    // We push the selected configuration
                    //console.log(JSON.stringify(configuration_study_map));
                    vmSelectedExperiments.push(configuration_study_map);

                }

                //console.log("STUDY START WITH:")
                //console.log(JSON.stringify(vmSelectedExperiments));
                if (vmSelectedExperiments.length > 0){
                    viewPresentExperimet.setTracker(vmSelectedExperiments);
                    vmCurrentExperimentIndex = -1;
                    viewPresentExperimet.advanceCurrentExperiment()
                    viewCalibrationStart.vmSelectedEye = cbEyeMsg.vmCurrentIndex;
                    if (!loader.getConfigurationBoolean("use_mouse") && loader.isVREnabled()){
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

