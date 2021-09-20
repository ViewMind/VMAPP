import QtQuick
import "DateLocalization.js" as DL

Item {

    id: vmFinalizedStudyEntry

    readonly property int vmHeight: mainWindow.height*0.043
    readonly property int vmFontSize: 12*viewHome.vmScale

    height: vmHeight
    width: dateRect.width + subjectRect.width + studyRect.width + doctorRect.width

    MouseArea {
        id: selectArea
        anchors.fill: parent
        onClicked: {
            vmIsSelected = true;
            studyListView.currentIndex = vmItemIndex
        }
    }

    Rectangle {

        id: dateRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerDate.width
        anchors.left: parent.left
        anchors.top: parent.top
        Text {
            id: dateText
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: {
                //console.log("Setting the date to " + date)
                DL.setDate(date,"dd/MM/yyyy HH:mm");
                return DL.getDateAndTime()
            }
            color: vmIsSelected? "#ffffff" : "#000000"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mainWindow.width*0.016
        }
    }


    Rectangle {
        id: subjectRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerSubject.width
        anchors.left: dateRect.right
        anchors.top: parent.top
        visible: true
        Text {
            id: subjectText
            color: vmIsSelected? "#ffffff" : "#000000"
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: {
                if (subject_name !== " ") return subject_name;
                else return subject_insitution_id;
            }
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: studyRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerStudy.width
        anchors.left: subjectRect.right;
        anchors.top: parent.top
        visible: true
        Text {
            id: studyText
            color: vmIsSelected? "#ffffff" : "#000000"
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: type
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: doctorRect
        color: vmIsSelected? "#4984b3" : "#ffffff"
        border.color: "#EDEDEE"
        border.width: mainWindow.width*0.002
        height: vmHeight
        width: headerDoctor.width
        anchors.left: studyRect.right;
        anchors.top: parent.top
        visible: true
        Text {
            id: doctorText
            color: vmIsSelected? "#ffffff" : "#000000"
            font.family: viewHome.gothamR.name
            font.pixelSize: vmFontSize
            text: medic_name
            anchors.centerIn: parent
        }
    }

}
