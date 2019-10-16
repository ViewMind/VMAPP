import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


VMBase {

    id: viewMedicalInformation
    width: viewMedicalInformation.vmWIDTH
    height: viewMedicalInformation.vmHEIGHT

    readonly property string keysearch: "viewmedinfo_"

    property string vmPatientName: "NO NAME";
    property int vmRecordIndex: -1

    property var vmMedicationList: [];
    property int vmMedicationEntryHeight: 30
    property int vmMedicationTableHeight: 10

    property var vmEvaluationList: [];
    property int vmEvaluationEntryHeight: 30
    property int vmEvaluationTableHeight: 10

    property int vmSpaceBetweenTitleAndData: 15
    property int vmSpaceBetweenDataAndTitle: 40
    property int vmContentHeight: 0

    //////////////////////// LIST MODEL ITEMS

    ListModel{
        id: medicationList;
    }

    ListModel{
        id: evaluationList;
    }

    //////////////////////// GUI AND LOGIC FUNCTIONS
    function clearAll(){
        medicationList.clear()
        evaluationList.clear()
        inpDose.clear()
        inpOtherMeds.clear()
        inpPresumptiveDiag.setText("")
        inpRMN_Amyg.clear()
        inpRMN_HOC.clear()
        inpRMN_Hipp.clear()
        inpRMN_InfLatVent.clear()
        inpRMN_LAmyg.clear()
        inpRMN_LCorThick.clear()
        inpRMN_LEntCort.clear()
        inpRMN_LHip.clear()
        inpRMN_RAmyg.clear()
        inpRMN_RCorThick.clear()
        inpRMN_REntCort.clear()
        inpRMN_RHip.clear()
        inpRMN_SupLatVent.clear()
        inpRMN_TotCertCort.clear()
        inpRMN_TotGrayMatt.clear()
        inpRMN_TotWhiteMatt.clear()
        inpRecordDate.clear()
        inpScores.clear()
        vmEvaluationList = [];
        vmMedicationList = [];
        vmRecordIndex = -1
        updateContentHeight();
        medicationTableHeader.visible = false;
        evaluationTableHeader.visible = false;
        inpMedication.setSelection(0)
        inpEvaluation.setSelection(0)
    }

    function setMedicalInformation(medicalRecord, recordIndex){

        // Setting the rnm values
        var inputList = [ inpRMN_Amyg, inpRMN_HOC , inpRMN_Hipp,
                          inpRMN_InfLatVent, inpRMN_SupLatVent,
                          inpRMN_LAmyg, inpRMN_LCorThick , inpRMN_LEntCort , inpRMN_LHip,
                          inpRMN_RAmyg, inpRMN_RCorThick , inpRMN_REntCort , inpRMN_RHip,
                          inpRMN_TotCertCort, inpRMN_TotGrayMatt , inpRMN_TotWhiteMatt];
        var nameList  = [ "RMN_Amyg", "RMN_HOC" , "RMN_Hipp",
                          "RMN_InfLatVent", "RMN_SupLatVent",
                          "RMN_LAmyg", "RMN_LCorThick" , "RMN_LEntCort" , "RMN_LHip",
                          "RMN_RAmyg", "RMN_RCorThick" , "RMN_REntCort" , "RMN_RHip",
                          "RMN_TotCertCort", "RMN_TotGrayMatt" , "RMN_TotWhiteMatt"];
        for (var i = 0; i < nameList.length; i++) inputList[i].setText(medicalRecord.RNM[nameList[i]])

        // Setting the medication list.
        for (var medName in medicalRecord.medication){
            vmMedicationList.push({"name": medName, "dose": medicalRecord.medication[medName]});
        }
        loadMedicationList()

        // Setting the evaluation list
        for (var eval in medicalRecord.evaluations){
           vmEvaluationList.push({"name": eval, "result": medicalRecord.evaluations[eval]});
        }
        loadEvaluationList()

        // Setting the formative years.
//        if (medicalRecord.formative_years !== -1){
//            inpFomativeYears.setText(medicalRecord.formative_years)
//        }

        // Setting the date
        inpRecordDate.setISODate(medicalRecord.date)

        // Setting the diagnosis content.
        inpPresumptiveDiag.setText(medicalRecord.presumptive_diagnosis)

        vmRecordIndex = recordIndex

    }

    function addMedicationToList(medication,dose,othermeds){
        inpDose.vmErrorMsg = "";
        inpMedication.vmErrorMsg = "";

        // Selecting which medication to user
        var meds = "";
        if (othermeds === "") meds = medication;
        else meds = othermeds;

        for (var i = 0; i < vmMedicationList.length; i++){
            if (vmMedicationList[i].name === meds){
                inpMedication.vmErrorMsg = loader.getStringForKey(keysearch+"errorMedFound");
                return;
            }
        }
        var value = parseFloat(dose)
        if (isNaN(value)){
            inpDose.vmErrorMsg = loader.getStringForKey(keysearch+"errorInvalidNum");
            return;
        }
        vmMedicationList.push({"name": meds, "dose": value});
        loadMedicationList()
    }

    function addEvaluationToList(evaluation,result){
        inpScores.vmErrorMsg = "";
        inpEvaluation.vmErrorMsg = "";
        for (var i = 0; i < vmEvaluationList.length; i++){
            if (vmEvaluationList[i].name === evaluation){
                inpEvaluation.vmErrorMsg = loader.getStringForKey(keysearch+"errorEvalFound");
                return;
            }
        }
        var value = parseFloat(result)
        if (isNaN(value)){
            inpScores.vmErrorMsg = loader.getStringForKey(keysearch+"errorInvalidNum");
            return;
        }
        vmEvaluationList.push({"name": evaluation, "result": value});
        loadEvaluationList()
    }

    function loadMedicationList(){
        medicationList.clear();
        for (var i = 0; i < vmMedicationList.length; i++){
            var item = {"vmIsSelected" : false,
                "vmMedication" : vmMedicationList[i].name,
                "vmDose": vmMedicationList[i].dose,
                "vmItemIndex": i};
            medicationList.append(item);
        }
        vmMedicationTableHeight = vmMedicationEntryHeight*vmMedicationList.length;
        if (vmMedicationList.length > 0) medicationTableHeader.visible = true;
        else medicationTableHeader.visible = false;
        updateContentHeight()
    }

    function loadEvaluationList(){
        evaluationList.clear();
        for (var i = 0; i < vmEvaluationList.length; i++){
            var item = {"vmIsSelected" : false,
                "vmEvaluation" : vmEvaluationList[i].name,
                "vmResult": vmEvaluationList[i].result,
                "vmItemIndex": i};
            evaluationList.append(item);
        }
        vmEvaluationTableHeight = vmEvaluationEntryHeight*vmEvaluationList.length;
        if (vmEvaluationList.length > 0) evaluationTableHeader.visible = true;
        else evaluationTableHeader.visible = false;
        updateContentHeight()
    }

    function updateContentHeight() {
        vmContentHeight = generalInfoTitle.height +
                generalInfoRow.height +
                medicationInfoTitle.height +
                medicationAddRow.height +
                medicationTableHeader.height +
                evaluationAddRow.height +
                evaluationInfoTitle.height+
                evaluationTableHeader.height +
                inpOtherMeds.height +
                rmnInfoTitle.height+
                rmnAddRow.height +
                diagInfoTitle.height +
                inpPresumptiveDiag.height +
                vmMedicationTableHeight +
                vmEvaluationTableHeight;
        vmContentHeight = vmContentHeight + 11*vmSpaceBetweenTitleAndData + 4*vmSpaceBetweenDataAndTitle;
    }

    function checkValidFloat(vmInput){
        vmInput.vmErrorMsg = "";
        var ans = {"value": -1, "ok": false};
        if (vmInput.vmEnteredText === ""){
            ans.ok = true;
            return ans;
        }
        var fvalue = parseFloat(vmInput.vmEnteredText);
        if (isNaN(fvalue) || (fvalue < 0)){
            vmInput.vmErrorMsg = loader.getStringForKey(keysearch+"errorInvalidNum");
            return ans;
        }
        vmInput.setText(fvalue);
        ans.value = fvalue;
        ans.ok = true;
        return ans;
    }

    function generateMapFromModel(model,keyProperty,valueProperty){
        var map = {};
        for (var i = 0; i < model.count; i++){
            var obj = model.get(i);
            map[obj[keyProperty]] = obj[valueProperty]
        }
        return map;
    }

    //////////////////////////////// TITLE AND SUBTITLE

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: vmPatientName
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 13
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"subTitle");
    }

    //////////////////////////////// MAIN FORM

    ScrollView {
        id: form
        clip: true
        width: parent.width
        height: parent.height*0.5;
        anchors.left: parent.left
        anchors.top: viewSubTitle.top
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        anchors.topMargin: 40
        contentHeight: vmContentHeight

        //////////////////////////// GENERAL INFORMATION
        Text {
            id: generalInfoTitle
            font.family: gothamB.name
            font.pixelSize: 16
            anchors.top:  parent.top
            anchors.left: parent.left
            anchors.leftMargin: 200
            color: "#b5b5b5"
            text: loader.getStringForKey(keysearch+"labGeneral");
        }

        Row {

            id: generalInfoRow
            width: parent.width*0.6
            spacing: 100
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: generalInfoTitle.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData

//            VMTextDataInput{
//                id: inpFomativeYears
//                width: (parent.width-generalInfoRow.spacing)/2
//                vmPlaceHolder: loader.getStringForKey(keysearch+"formalEducation");
//                //Keys.onTabPressed: labelLastName.vmFocus = true;
//            }

            VMDateInputField{
                id: inpRecordDate
                width: (parent.width-generalInfoRow.spacing)/2
                vmEnableDateCheck: false
                vmPlaceHolder: loader.getStringForKey(keysearch+"recordDate");
                //Keys.onTabPressed: labelDocument_number.vmFocus = true;
            }

        }

        //////////////////////////// MEDICATION
        Text {
            id: medicationInfoTitle
            font.family: gothamB.name
            font.pixelSize: 16
            anchors.top:  generalInfoRow.bottom
            anchors.topMargin: vmSpaceBetweenDataAndTitle
            anchors.left: generalInfoTitle.left
            color: "#b5b5b5"
            text: loader.getStringForKey(keysearch+"labMED");
        }

        Row {

            id: medicationAddRow
            width: parent.width*0.6
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: medicationInfoTitle.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData
            z: 2

            VMComboBox2 {
                id: inpMedication
                width: parent.width*0.6
                anchors.bottom: parent.bottom
                Component.onCompleted: inpMedication.setModelList(loader.getStringListForKey(keysearch+"medList"))
                onVmCurrentIndexChanged: {
                    if ((vmListSize-1) == vmCurrentIndex){
                        // This is the last options, which means the others option
                        inpOtherMeds.visible = true;
                        inpOtherMeds.clear();
                    }
                    else{
                        inpOtherMeds.visible = false;
                        inpOtherMeds.clear();
                    }
                }
            }

            VMTextDataInput{
                id: inpDose
                width: (parent.width-2*medicationAddRow.spacing - inpMedication.width - btnAddMedication.width)
                vmPlaceHolder: loader.getStringForKey(keysearch+"dose");
                //Keys.onTabPressed: labelLastName.vmFocus = true;
            }

            VMPlusButton {
                id: btnAddMedication
                height: 30
                anchors.bottom: parent.bottom
                onClicked: addMedicationToList(inpMedication.vmCurrentText,inpDose.vmEnteredText,inpOtherMeds.vmEnteredText)
            }

        }

        VMTextDataInput{
            id: inpOtherMeds
            width: medicationAddRow.width
            anchors.top: medicationAddRow.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData
            anchors.left: medicationAddRow.left
            vmPlaceHolder: loader.getStringForKey(keysearch+"labOtherMED");
            visible: false
        }

        Row {
            id: medicationTableHeader
            anchors.top: inpOtherMeds.visible ? inpOtherMeds.bottom : medicationAddRow.bottom;
            anchors.topMargin: 2*vmSpaceBetweenTitleAndData
            anchors.left: medicationAddRow.left
            height: 30
            width: medicationAddRow.width
            visible: false

            Rectangle {
                id: headerMedication
                color: "#adadad"
                border.width: 2
                border.color: "#EDEDEE"
                radius: 4
                width: parent.width*0.6
                height: parent.height
                Text {
                    id: patientText
                    text: loader.getStringForKey(keysearch+"headerMedication");
                    width: parent.width
                    font.family: gothamB.name
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                id: headerDose
                color: "#adadad"
                border.width: 2
                border.color: "#EDEDEE"
                radius: 4
                width: parent.width*0.3
                height: parent.height
                Text {
                    id: readingText
                    text: loader.getStringForKey(keysearch+"dose");
                    width: parent.width
                    font.family: gothamB.name
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        ListView {
            id: medicationListView
            width: medicationAddRow.width
            anchors.left: medicationAddRow.left
            anchors.top: medicationTableHeader.bottom
            height: vmMedicationTableHeight
            model: medicationList
            delegate: VMMedicationEntry {
                height: vmMedicationEntryHeight
                onRemoveItem: {
                    vmMedicationList.splice(itemIndex,1);
                    loadMedicationList();
                }
            }
            onCurrentIndexChanged: {
                for (var i = 0; i < model.count; i++){
                    if (i != currentIndex){
                        medicationList.setProperty(i,"vmIsSelected",false);
                    }
                }
            }
        }

        //////////////////////////// EVALUATIONS
        Text {
            id: evaluationInfoTitle
            font.family: gothamB.name
            font.pixelSize: 16
            anchors.top:  (medicationTableHeader.visible)? medicationListView.bottom : (inpOtherMeds.visible? inpOtherMeds.bottom : medicationAddRow.bottom)
            anchors.topMargin: vmSpaceBetweenDataAndTitle
            anchors.left: generalInfoTitle.left
            color: "#b5b5b5"
            text: loader.getStringForKey(keysearch+"labScores");
        }

        Row {

            id: evaluationAddRow
            width: parent.width*0.6
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: evaluationInfoTitle.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData
            z: 1

            VMComboBox2{
                id: inpEvaluation
                width: parent.width*0.6
                anchors.bottom: parent.bottom
                Component.onCompleted: inpEvaluation.setModelList(loader.getStringListForKey(keysearch+"testNames"));
            }

            VMTextDataInput{
                id: inpScores
                width: (parent.width-2*evaluationAddRow.spacing - inpEvaluation.width - btnAddEvalution.width)
                vmPlaceHolder: loader.getStringForKey(keysearch+"testScore");
                //Keys.onTabPressed: labelLastName.vmFocus = true;
            }

            VMPlusButton {
                id: btnAddEvalution
                height: 30
                anchors.bottom: parent.bottom
                onClicked: addEvaluationToList(inpEvaluation.vmCurrentText,inpScores.vmEnteredText)
            }

        }

        Row {
            id: evaluationTableHeader
            anchors.top: evaluationAddRow.bottom
            anchors.topMargin: 2*vmSpaceBetweenTitleAndData
            anchors.left: evaluationAddRow.left
            height: 30
            width: evaluationAddRow.width
            visible: false

            Rectangle {
                id: headerEvaluation
                color: "#adadad"
                border.width: 2
                border.color: "#EDEDEE"
                radius: 4
                width: parent.width*0.6
                height: parent.height
                Text {
                    id: evalutionText
                    text: loader.getStringForKey(keysearch+"headerEvaluation");
                    width: parent.width
                    font.family: gothamB.name
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                id: headerResults
                color: "#adadad"
                border.width: 2
                border.color: "#EDEDEE"
                radius: 4
                width: parent.width*0.3
                height: parent.height
                Text {
                    id: resultText
                    text: loader.getStringForKey(keysearch+"testScore");
                    width: parent.width
                    font.family: gothamB.name
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        ListView {
            id: evaluationListView
            width: evaluationAddRow.width
            anchors.left: evaluationAddRow.left
            anchors.top: evaluationTableHeader.bottom
            height: vmEvaluationTableHeight
            model: evaluationList
            delegate: VMEvaluationEntry {
                height: vmEvaluationEntryHeight
                onRemoveItem: {
                    vmEvaluationList.splice(itemIndex,1);
                    loadEvaluationList();
                }
            }
            onCurrentIndexChanged: {
                for (var i = 0; i < model.count; i++){
                    if (i != currentIndex){
                        evaluationList.setProperty(i,"vmIsSelected",false);
                    }
                }
            }
        }

        //////////////////////////// EVALUATIONS
        Text {
            id: rmnInfoTitle
            font.family: gothamB.name
            font.pixelSize: 16
            anchors.top:  (evaluationTableHeader.visible)? evaluationListView.bottom : evaluationAddRow.bottom
            anchors.topMargin: vmSpaceBetweenDataAndTitle
            anchors.left: generalInfoTitle.left
            color: "#b5b5b5"
            text: loader.getStringForKey(keysearch+"labRMN");
        }

        Row {
            id: rmnAddRow
            width: parent.width*0.6
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rmnInfoTitle.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData

            Column {
                id: rmnAddColumnLeft
                width: (parent.width-rmnAddRow.spacing)/2
                spacing: vmSpaceBetweenTitleAndData*1.5

                VMTextDataInput{
                    id: inpRMN_Hipp
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_Hipp");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_LHip
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_LHip");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_RHip
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_RHip");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_Amyg
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_Amyg");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_LAmyg
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_LAmyg");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_RAmyg
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_RAmyg");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_SupLatVent
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_SupLatVent");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_InfLatVent
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_InfLatVent");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

            }

            Column {
                id: rmnAddColumnRight
                width: (parent.width-rmnAddRow.spacing)/2
                spacing: vmSpaceBetweenTitleAndData*1.5

                VMTextDataInput{
                    id: inpRMN_HOC
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_HOC");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_TotGrayMatt
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_TotGrayMatt");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_TotWhiteMatt
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_TotWhiteMatt");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_TotCertCort
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_TotCerCort");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_LEntCort
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_LEntCort");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_REntCort
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_REntCort");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_LCorThick
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_LCorThick");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

                VMTextDataInput{
                    id: inpRMN_RCorThick
                    width: parent.width
                    vmPlaceHolder: loader.getStringForKey(keysearch+"RMN_RCorThick");
                    //Keys.onTabPressed: labelLastName.vmFocus = true;
                }

            }

        }

        //////////////////////////// DIAGNOSIS
        Text {
            id: diagInfoTitle
            font.family: gothamB.name
            font.pixelSize: 16
            anchors.top:  rmnAddRow.bottom
            anchors.topMargin: vmSpaceBetweenDataAndTitle
            anchors.left: generalInfoTitle.left
            color: "#b5b5b5"
            text: loader.getStringForKey(keysearch+"labPDag");
        }

        VMTextInputArea {
            id: inpPresumptiveDiag
            width: parent.width*0.6
            height: 100
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: diagInfoTitle.bottom
            anchors.topMargin: vmSpaceBetweenTitleAndData
        }

    } // End of the Scroll View

    // Buttons
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: form.bottom
        anchors.topMargin: 26
        spacing: 29

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnCancel");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                clearAll();
                swiperControl.currentIndex = swiperControl.vmIndexMedicalRecordList;
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keysearch+"btnSave");
            vmFont: viewHome.gothamM.name
            onClicked: {

                var medicalRecord = {};

                // Adding the diagnosis.
                var pdiag = inpPresumptiveDiag.getText();
                inpPresumptiveDiag.vmErrorMsg = ""
                if (pdiag.includes(';') || pdiag.includes('|')){
                    inpPresumptiveDiag.vmErrorMsg = loader.getStringForKey(keysearch + "errorInvalidChars") + " '|', ';'";
                    return
                }
                medicalRecord.presumptive_diagnosis = pdiag;

                // Adding the date.
                if (inpRecordDate.vmEnteredText === ""){
                    inpRecordDate.vmErrorMsg = loader.getStringForKey(keysearch + "errorDateEmpty");
                    return
                }
                medicalRecord.date = inpRecordDate.getISODate();

                // Checking the rnm values
                var inputList = [ inpRMN_Amyg, inpRMN_HOC , inpRMN_Hipp,
                                  inpRMN_InfLatVent, inpRMN_SupLatVent,
                                  inpRMN_LAmyg, inpRMN_LCorThick , inpRMN_LEntCort , inpRMN_LHip,
                                  inpRMN_RAmyg, inpRMN_RCorThick , inpRMN_REntCort , inpRMN_RHip,
                                  inpRMN_TotCertCort, inpRMN_TotGrayMatt , inpRMN_TotWhiteMatt];
                var nameList  = [ "RMN_Amyg", "RMN_HOC" , "RMN_Hipp",
                                  "RMN_InfLatVent", "RMN_SupLatVent",
                                  "RMN_LAmyg", "RMN_LCorThick" , "RMN_LEntCort" , "RMN_LHip",
                                  "RMN_RAmyg", "RMN_RCorThick" , "RMN_REntCort" , "RMN_RHip",
                                  "RMN_TotCertCort", "RMN_TotGrayMatt" , "RMN_TotWhiteMatt"];


                var rnm = {}
                for (var i = 0; i < inputList.length; i++){
                    var ans = checkValidFloat(inputList[i]);
                    if (!ans.ok) return;
                    if (ans.value > -1) rnm[nameList[i]] = ans.value;
                }
                medicalRecord.RNM = rnm;

                // Adding the medication.
                medicalRecord.medication = generateMapFromModel(medicationList,"vmMedication","vmDose")

                // Adding the evaluations
                medicalRecord.evaluations = generateMapFromModel(evaluationList,"vmEvaluation","vmResult")

                loader.addPatientMedicalRecord(medicalRecord,vmRecordIndex);
                swiperControl.currentIndex = swiperControl.vmIndexMedicalRecordList;

            }
        }
    }



}
