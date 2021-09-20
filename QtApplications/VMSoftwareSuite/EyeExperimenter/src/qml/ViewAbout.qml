import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects


VMDialogBase {

    property string vmLoadLanguage: "";

    id: viewAbout
    width: mainWindow.width*0.48
    height: mainWindow.height*0.812    

    MouseArea {
        anchors.fill: parent
        onClicked: {
            focus = true
        }
    }

    // The configure settings title
    Text {
        id: diagTitle
        font.pixelSize: 18*viewHome.vmScale
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: "EyeExperimenter  - " +  loader.getVersionNumber();
        //text: "EyeExperimenter  - 15.2.0";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.101
    }

    Row {
        id: mainRow;
        spacing: 0
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: diagTitle.bottom
        anchors.topMargin: parent.height*0.02

        Column{
            id: imageColumn
            spacing: 0

            Rectangle{
                id: imgFactoryRectangle
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: factory
                    source: "qrc:/images/labeling/manufacturer.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgRepresentative
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: representative
                    source: "qrc:/images/labeling/ec_rep.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgRefName
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: refName
                    source: "qrc:/images/labeling/reference.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgDateOfManufacture
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: dateOfManufacture
                    source: "qrc:/images/labeling/date_of_manufacture.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgSerialNumber
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: serialNumber
                    source: "qrc:/images/labeling/serial_number.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgInstUso
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1
                Image {
                    id: instUso
                    source: "qrc:/images/labeling/inst_uso.jpg"
                    width: parent.width*0.9
                    height: parent.height*0.9
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle{
                id: imgAuthorizedANMAT
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.1

                Rectangle{
                    id: imgAuthorizedSubRect
                    color: "#FFFFFF"
                    border.color: "#cfcfcf"
                    height: parent.height*0.7
                    width:  parent.width
                    Image {
                        id: authorizedANMAT
                        source: "qrc:/images/labeling/ce_mark.jpg"
                        width: parent.width*0.9
                        height: parent.height*0.9
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                    }
                }

                Rectangle{
                    id: imgAuthorizedTextSubRect
                    color: "#FFFFFF"
                    border.color: "#cfcfcf"
                    height: parent.height - imgAuthorizedSubRect.height
                    width:  parent.width
                    anchors.top: imgAuthorizedSubRect.bottom
                    anchors.left: parent.left
                    Text {
                        id: uniqueAuthorizationText
                        font.pixelSize: 10*viewHome.vmScale
                        font.family: viewHome.gothamR.name
                        anchors.centerIn: parent
                        text: loader.getUniqueAuthorizationNumber()
                    }


                }

            }

        }

        Column{
            id: textColumn

            Rectangle{
                id: textFactoryRectangle
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: factoryText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: {
                        var list = loader.getStringListForKey("labeling_company_info");
                        return list.join("\n");
                    }
                }
            }

            Rectangle{
                id: textRepresentative
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: representativeText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: {
                        var list = loader.getStringListForKey("labeling_representative");
                        return list.join("\n")
                    }
                }
            }

            Rectangle{
                id: textProductName
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: productNameText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: loader.getStringListForKey("labeling_name")
                }
            }

            Rectangle{
                id: textManufactureDate
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: manufactureDateText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: loader.getManufactureDate();
                }
            }

            Rectangle{
                id: textSerialNumber
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: serialNumberText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: loader.getSerialNumber();
                }
            }


            Rectangle{
                id: textReadInstructions
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: readInstructionsText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: loader.getStringListForKey("labeling_read_instructions")
                }
            }

            Rectangle{
                id: textAuthorizedByText
                color: "#FFFFFF"
                border.color: "#cfcfcf"
                height: viewAbout.height*0.1
                width:  viewAbout.width*0.7
                Text {
                    id: authorizedByText
                    font.pixelSize: 10*viewHome.vmScale
                    font.family: viewHome.gothamR.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: parent.width*0.05
                    text: {
                        var list = loader.getStringListForKey("labeling_authorized_use");
                        return list.join("\n");
                    }
                }
            }

        }


    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: mainWindow.height*0.032
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.width*0.02
        onClicked: {
            viewAbout.close()
        }
    }


}
