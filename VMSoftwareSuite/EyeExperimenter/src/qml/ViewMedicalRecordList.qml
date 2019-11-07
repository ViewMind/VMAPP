import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0


VMBase {

    id: viewMedRecList;

    readonly property string keysearch: "viewmedrec_"
    property string vmPatientName: "NO NAME";
    property var vmRecordList: [];

    function loadPatientRecordList(){
        var patientInfo = loader.getCurrentPatientInformation();
        vmRecordList = []
        vmRecordList = patientInfo.MEDICAL_RECORDS;
        medicalRecordList.clear();
        if (vmRecordList === undefined) return
        for (var i = 0; i < vmRecordList.length; i++){
            var ddate = vmRecordList[i].date;
            var parts = ddate.split("-");
            if (parts.length === 3) ddate = parts[2] + "/"  + parts[1] + "/" + parts[0]
            medicalRecordList.append({"vmDate" : ddate, "vmItemIndex": i, "vmIsSelected": false})
        }
    }

    ListModel{
        id: medicalRecordList;
    }

    // Title and subtitle
    Text {
        id: viewTitle
        font.family: gothamB.name
        font.pixelSize: 43*viewHome.vmScale
        anchors.top:  vmBanner.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3fa2f7"
        text: vmPatientName
    }

    Text {
        id: viewSubTitle
        font.family: robotoR.name
        font.pixelSize: 13*viewHome.vmScale
        anchors.top:  viewTitle.bottom
        anchors.topMargin: 13
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#5499d5"
        text: loader.getStringForKey(keysearch+"subTitle");
    }

    VMPlusButton{
        height: 30
        anchors.left: tableHeader.left
        anchors.bottom: tableHeader.top
        anchors.bottomMargin: 15
        onClicked: {
            viewMedicalInformation.clearAll()
            swiperControl.currentIndex = swiperControl.vmIndexMedicalInformation;
        }
    }

    // The table header.
    Row {
        id: tableHeader
        anchors.top: viewSubTitle.bottom
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        height: 30
        Rectangle {
            id: headerMain
            color: "#adadad"
            border.width: 2
            border.color: "#EDEDEE"
            radius: 4
            width: viewMedRecList.width*0.7
            height: parent.height
            Text {
                id: patientText
                text: loader.getStringForKey(keysearch+"headerMain");
                width: parent.width
                font.family: gothamB.name
                font.pixelSize: 15*viewHome.vmScale
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Rectangle {
        id: tableBackground
        color: "#ffffff"
        border.width: 2
        border.color: "#EDEDEE"
        radius: 4
        anchors.top: tableHeader.bottom
        anchors.left: tableHeader.left
        width: headerMain.width
        height: parent.height*0.5

        ScrollView {
            id: tableArea
            anchors.fill: parent
            clip: true
            ListView {
                id: medRecListView
                anchors.fill: parent
                model: medicalRecordList
                delegate: VMMedRecordEntry {
                }
                onCurrentIndexChanged: {
                    for (var i = 0; i < model.count; i++){
                        if (i != currentIndex){
                            medicalRecordList.setProperty(i,"vmIsSelected",false);
                        }
                    }
                }
            }
        }
    }


    Row{

        anchors.top: tableBackground.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: mainWindow.width*0.023

        VMButton{
            id: diagBtnBack
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                swiperControl.currentIndex = swiperControl.vmIndexPatientList;
            }
        }

        VMButton{
            id: diagBtnView
            height: 50
            vmText: loader.getStringForKey(keysearch+"btnView");
            vmFont: viewHome.gothamM.name
            enabled: medRecListView.currentIndex !== -1
            onClicked: {
                viewMedicalInformation.clearAll()
                viewMedicalInformation.setMedicalInformation(vmRecordList[medRecListView.currentIndex],medRecListView.currentIndex)
                swiperControl.currentIndex = swiperControl.vmIndexMedicalInformation;
            }
        }

    }

}
