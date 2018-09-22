import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

VMBase {

    id: viewPatientList
    width: viewPatientList.vmWIDTH
    height: viewPatientList.vmHEIGHT
    readonly property real vmTableWidth: 0.60*viewPatientList.vmWIDTH
    readonly property real vmTableHeight: 0.45*viewPatientList.vmHEIGHT

    readonly property string keybase: "viewpatientlist_"

    Connections {
        target: loader
        onSynchDone: {
            connectionDialog.close();
        }
    }


    Dialog {

        property string vmTitle: "TITLE"
        property string vmMessage: "MESSAGE"

        id: connectionDialog;
        modal: true
        width: 614
        height: 600
        y: (parent.height - height)/2
        x: (parent.width - width)/2
        closePolicy: Popup.CloseOnEscape

        contentItem: Rectangle {
            id: rectDialog
            anchors.fill: parent
            layer.enabled: true
            layer.effect: DropShadow{
                radius: 5
            }
        }

        // The instruction text
        Text {
            id: diagTitle
            font.family: viewHome.gothamB.name
            font.pixelSize: 43
            anchors.top: parent.top
            anchors.topMargin: 88
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmTitle
        }

        // The instruction text
        Text {
            id: diagMessage
            font.family: viewHome.robotoR.name
            font.pixelSize: 13
            anchors.top:  diagTitle.bottom
            anchors.topMargin: 26
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#297fca"
            text: connectionDialog.vmMessage
        }

        AnimatedImage {
            id: slideAnimation
            source: "qrc:/images/LOADING.gif"
            anchors.top: diagMessage.bottom
            anchors.topMargin: 30
            x: (parent.width - slideAnimation.width)/2;
        }

    }


    ListModel {
        id: patientList
    }

    // Loads the list model with the patient information
    function loadPatients() {

        // WARNING: Get patient list call fills the other two lists. It NEEDS to be called first.
        var patientNameList = loader.getPatientList();
        var uidList = loader.getUIDList();
        var isOkList = loader.getPatientIsOKList();

        // Clearing the current model.
        patientList.clear()

        //console.log("Patient List: " + patientNameList.length + ". UIDList: " + uidList.length + ". isOKList: " + isOkList.length);

        for (var i = 0; i < patientNameList.length; i++){
            patientList.append({"uid": uidList[i], "pname": patientNameList[i], "vmIsOk": (isOkList[i] === "true"), "index" : i, "vmIsSelected" : false});
        }

        patientListView.currentIndex = -1;
    }

    function requestReportToServer(index){
        console.log("Get Report for index: " +  index);
        //patientList.setProperty(index,"vmIsOk",true);
    }

    function setCurrentPatient(){
        if (patientListView.currentIndex == -1) return;
        loader.setValueForConfiguration(vmDefines.vmCONFIG_PATIENT_UID,patientList.get(patientListView.currentIndex).uid);
        loader.setValueForConfiguration(vmDefines.vmCONFIG_PATIENT_NAME,patientList.get(patientListView.currentIndex).pname);
        //console.log("PNAME: " + patientList.get(patientListView.currentIndex).pname + ". UID: " + patientList.get(patientListView.currentIndex).uid);
    }

    function startDBSync(){

        connectionDialog.vmMessage = loader.getStringForKey(keybase+"diagConnectMessage");
        connectionDialog.vmTitle = loader.getStringForKey(keybase+"diagConnectTitle");
        connectionDialog.open();
        loader.startDBSync();

    }

    // The Doctor Information Title and subtitle
    Text {
        id: title
        font.pixelSize: 43
        font.family: gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: vmBanner.bottom
        anchors.topMargin: 30
    }

    Text {
        id: subTitle
        font.pixelSize: 11
        font.family: gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: 11
    }

    Rectangle {

        id: toolBar
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        width: vmTableWidth
        height: 0.08*parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: subTitle.bottom
        anchors.topMargin: 20

        VMPlusButton {
            id: btnAddPatient
            height: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20;
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }

        VMPencilButton {
            id: btnEditPatient
            height: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: btnAddPatient.right
            anchors.leftMargin: 10;
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                viewPatientReg.loadPatientInformation();
                swiperControl.currentIndex = swiperControl.vmIndexPatientReg
            }
        }


        VMButton{
            id: btnView
            height: 30
            vmText: loader.getStringForKey(keybase+"btnView");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            onClicked: {
            }
        }
    }

    // The table header.
    Row {
        id: tableHeader
        anchors.top: toolBar.bottom
        anchors.topMargin: 10
        anchors.left: toolBar.left
        height: 30
        Rectangle {
            id: headerPatient
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: 0.7*vmTableWidth
            height: parent.height
            Text {
                id: patientText
                text: loader.getStringForKey(keybase+"headerPatient");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Rectangle {
            id: headerStatus
            color: "#ffffff"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: vmTableWidth-headerPatient.width
            height: parent.height
            Text {
                id: statusText
                text: loader.getStringForKey(keybase+"headerStatus");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    // The table where each of the patient entries will be put. and its background
    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        anchors.top: tableHeader.bottom
        anchors.left: tableHeader.left
        width: vmTableWidth
        height: vmTableHeight - tableHeader.height

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: patientListView
                anchors.fill: parent
                model: patientList
                delegate: VMPatientEntry {
                    vmPatientUID: uid
                    vmPatientName: pname
                    vmItemIndex: index
                    onFetchReport: {
                        console.log("Fetch report of " + index);
                        requestReportToServer(index)
                    }
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i != currentIndex){
                            patientList.setProperty(i,"vmIsSelected",false)
                        }
                    }
                    setCurrentPatient();
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
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexHome;
            }
        }

        VMButton{
            id: btnStart
            height: 50
            vmText: loader.getStringForKey(keybase+"btnStart");
            vmFont: viewHome.gothamM.name
            enabled: patientListView.currentIndex !== -1
            onClicked: {
                setCurrentPatient();
                //swiperControl.currentIndex = swiperControl.vmIndexStudyStart;
            }
        }
    }

}
