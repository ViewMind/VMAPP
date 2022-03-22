import QtQuick
import "../"
import "../components"

Rectangle {

    id: settingsAbout
    anchors.fill: parent
    radius: VMGlobals.adjustWidth(10);
    color: VMGlobals.vmGrayToggleOff

    readonly property double vmDivisorMargin: VMGlobals.adjustHeight(15);

    // Rectangle that straightens the left side edge.
    Rectangle {
        id: straightDivisor
        height: parent.height
        anchors.left: parent.left
        width: parent.radius
        border.width: 0
        color: parent.color
    }

    // Title
    Text {
        id: title
        text: loader.getStringForKey("viewsettings_about");
        font.weight: 600
        font.pixelSize: VMGlobals.vmFontLarger
        height: VMGlobals.adjustHeight(18);
        verticalAlignment: Text.AlignVCenter
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(27)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }

    //////////////////////////////////////////// APPLICATION VERSION

    Text {
        id: applicationName
        text: "EyeExplorer"
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        height: VMGlobals.adjustHeight(15);
        verticalAlignment: Text.AlignVCenter
        anchors.top: title.bottom
        anchors.topMargin: VMGlobals.adjustHeight(27)
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }

    Text {
        id: applicationVersion
        text: loader.getStringForKey("label_version") + " " + loader.getVersionNumber();
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        height: VMGlobals.adjustHeight(18);
        verticalAlignment: Text.AlignVCenter
        anchors.top: applicationName.top
        anchors.right: parent.right
        anchors.rightMargin: VMGlobals.adjustWidth(20)
    }

    Rectangle {
        id: div_app_name_bottom
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: applicationName.bottom
        anchors.topMargin: vmDivisorMargin
    }

    //////////////////////////////////////////// VIEWMIND S.A.

    Column {

        id: vmIndustryColumn
        anchors.top: div_app_name_bottom.bottom
        anchors.topMargin: vmDivisorMargin
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(89)
        spacing: VMGlobals.adjustHeight(0)

        Text {
            id: vmBody
            text: loader.getStringForKey("label_vm_body")
            font.weight: 400
            font.pixelSize: VMGlobals.vmFontSmall
            height: VMGlobals.adjustHeight(52);
            verticalAlignment: Text.AlignVCenter
        }

        Row {

            id: contactRow
            spacing: VMGlobals.adjustWidth(2);

            Text {
                id: vmMail
                font.weight: 400
                font.pixelSize: VMGlobals.vmFontSmall
                height: VMGlobals.adjustHeight(13);
                verticalAlignment: Text.AlignVCenter
                color: VMGlobals.vmBlueSelected
                Component.onCompleted: {
                    var list = loader.getStringListForKey("label_vm_contact");
                    text = list[0]
                }
            }


            Text {
                id: dash
                text: "|"
                font.weight: 400
                font.pixelSize: VMGlobals.vmFontSmall
                height: VMGlobals.adjustHeight(13);
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: vmWebsite
                font.weight: 400
                font.pixelSize: VMGlobals.vmFontSmall
                height: VMGlobals.adjustHeight(13);
                verticalAlignment: Text.AlignVCenter
                color: VMGlobals.vmBlueSelected
                Component.onCompleted: {
                    var list = loader.getStringListForKey("label_vm_contact");
                    text = list[1]
                }
            }

        }

    }

    Image {
        id: industryBlack
        source: "qrc:/images/labeling/industry.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(50)
        anchors.verticalCenter: vmIndustryColumn.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }

    Rectangle {
        id: div_vm_industry
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: vmIndustryColumn.bottom
        anchors.topMargin: vmDivisorMargin
    }

    //////////////////////////////////////////// VIEWMIND EC REP

    Image {
        id: ecrepImage
        source: "qrc:/images/labeling/ec.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(50)
        anchors.verticalCenter: ecrepText.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: VMGlobals.adjustWidth(19)
    }


    Text {
        id: ecrepText
        text: loader.getStringForKey("label_ecrep")
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        height: VMGlobals.adjustHeight(30);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_vm_industry.bottom
        anchors.topMargin: vmDivisorMargin
        anchors.left: vmIndustryColumn.left
    }

    Rectangle {
        id: div_vm_ecrept
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: ecrepText.bottom
        anchors.topMargin: vmDivisorMargin
    }

    //////////////////////////////////////////// MD

    Image {
        id: mdImage
        source: "qrc:/images/labeling/md.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(35)
        anchors.verticalCenter: mdText.verticalCenter
        anchors.horizontalCenter: industryBlack.horizontalCenter
    }


    Text {
        id: mdText
        text: "<b>EYE EXPLORER</b>"
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        height: VMGlobals.adjustHeight(13);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_vm_ecrept.bottom
        anchors.topMargin: vmDivisorMargin
        anchors.left: vmIndustryColumn.left
    }

    Rectangle {
        id: div_vm_md
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: mdText.bottom
        anchors.topMargin: vmDivisorMargin
    }


    //////////////////////////////////////////// DATE

    Image {
        id: dateImage
        source: "qrc:/images/labeling/industry_white.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(35)
        anchors.verticalCenter: dateText.verticalCenter
        anchors.horizontalCenter: industryBlack.horizontalCenter
    }


    Text {
        id: dateText
        text: "06-01-2022"
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        height: VMGlobals.adjustHeight(13);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_vm_md.bottom
        anchors.topMargin: vmDivisorMargin*1.2
        anchors.left: vmIndustryColumn.left
        Component.onCompleted: {
            // Getting the date from the lastest version changes.
            var title_and_body = loader.getLatestVersionChanges();
            let versionText = title_and_body[0];
            let titleParts = versionText.split(" -- ")
            let date = titleParts[1];
            date = date.replace("/","-")
            date = date.replace("/","-")
            dateText.text = date;
        }
    }

    Rectangle {
        id: div_vm_date
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: dateText.bottom
        anchors.topMargin: vmDivisorMargin*1.2
    }

    //////////////////////////////////////////// manual

    Image {
        id: manualImage
        source: "qrc:/images/labeling/manual.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(30)
        anchors.verticalCenter: manualText.verticalCenter
        anchors.horizontalCenter: industryBlack.horizontalCenter
    }


    Text {
        id: manualText
        text: loader.getStringForKey("label_manual")
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        height: VMGlobals.adjustHeight(13);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_vm_date.bottom
        anchors.topMargin: vmDivisorMargin*1.2
        anchors.left: vmIndustryColumn.left
    }

    Rectangle {
        id: div_vm_manual
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: manualText.bottom
        anchors.topMargin: vmDivisorMargin*1.2
    }

    //////////////////////////////////////////// ce

    Image {
        id: ceImage
        source: "qrc:/images/labeling/ce.png"
        fillMode: Image.PreserveAspectFit
        width: VMGlobals.adjustWidth(30)
        anchors.verticalCenter: ceText.verticalCenter
        anchors.horizontalCenter: industryBlack.horizontalCenter
    }


    Text {
        id: ceText
        text: loader.getStringForKey("label_ec")
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontSmall
        height: VMGlobals.adjustHeight(66);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_vm_manual.bottom
        anchors.topMargin: vmDivisorMargin
        anchors.left: vmIndustryColumn.left
    }

}


