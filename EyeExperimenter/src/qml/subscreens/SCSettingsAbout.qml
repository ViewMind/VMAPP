import QtQuick
import "../"
import "../components"

Rectangle {

    id: settingsAbout
    anchors.fill: parent
    radius: VMGlobals.adjustWidth(10);
    color: VMGlobals.vmGrayToggleOff

    readonly property double vmDivisorMargin: VMGlobals.adjustHeight(15);
    readonly property string vmSYSTEM_RELEASE_DATE: "19-06-2024";
    readonly property string vmSYSTEM_VERSION: "27.1.A";
    readonly property string vmANVISA_REG_NUMBER: "81464750145";

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
        text: "ViewMind Atlas<sup>TM</sup> Congnition App"
        textFormat: Text.RichText
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

    //////////////////////////////////////////// SELECTOR

    Text {
        id: selectorText
        text: loader.getStringForKey("labeling_label_select")
        font.weight: 400
        font.pixelSize: VMGlobals.vmFontBaseSize
        height: VMGlobals.adjustHeight(15);
        verticalAlignment: Text.AlignVCenter
        anchors.top: div_app_name_bottom.bottom
        anchors.topMargin: vmDivisorMargin
        anchors.left: applicationName.left
    }

    VMComboBox {
        id: labelSelection
        width: VMGlobals.adjustWidth(130)
        anchors.top: parent.top
        anchors.topMargin: VMGlobals.adjustHeight(106);
        anchors.right: applicationVersion.right
        anchors.verticalCenter: selectorText.verticalCenter
        z: 100
        Component.onCompleted: {
            setModelList(loader.getStringListForKey("label_options"))
            setSelection(0);
        }

        onVmCurrentIndexChanged: {
            model.clear()
            if (vmCurrentText == "ANVISA"){
                labelLoadANVISA()
            }
            else if (vmCurrentText == "USA"){
                labelLoadUSA()
            }
            else if (vmCurrentText == "EU"){
                labelLoadEU()
            }
            else {
                labelLoadANMAT()
            }
        }

    }

    Rectangle {
        id: div_selector_bottom
        width: VMGlobals.adjustWidth(480)
        anchors.horizontalCenter: parent.horizontalCenter
        height: VMGlobals.adjustHeight(1)
        color: VMGlobals.vmGrayUnselectedBorder
        anchors.top: selectorText.bottom
        anchors.topMargin: vmDivisorMargin
    }


    ListModel {
        id: model
    }

    ListView {
        id: labelListView
        clip: true
        width: VMGlobals.adjustWidth(480)
        height: (parent.height - div_selector_bottom.y)*0.9;
        model: model
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: div_selector_bottom.bottom
        delegate: VMLabelItem {
            width: labelListView.width
        }
    }

    function labelLoadANMAT(){

        var element = {};
        element["vmIsLast"] = false;

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = vmSYSTEM_RELEASE_DATE
        element["vmImage"]  = "qrc:/images/labeling/industry_white.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.25;
        element["vmText"]   = "<b>VIEWMIND S.A</b><br>Av. 24 de Septiembre 1536, Oficina 12, Córdoba, Córdoba, Argentina.<br>DT: Romina Carranza, MP: 37665468.<br>\
<font color=\"" + VMGlobals.vmBlueSelected + "\"> info@viewmind.com</font> | <font color=\"" + VMGlobals.vmBlueSelected + "\">www.viewmind.com</font>"
        element["vmImage"]  = "qrc:/images/labeling/industry.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = "<b>VIEWMIND S.A.R.L.</b><br>House of Biohealth, 27 Rue Henri Koch, 4354 Esch-sur-Alzette, Luxemburgo"
        element["vmImage"]  = "qrc:/images/labeling/ec.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = "<b>ViewMind Atlas System for Cognition (Versión Global " + vmSYSTEM_VERSION + ")</b>"
        element["vmImage"]  = "qrc:/images/labeling/md.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.17;
        element["vmText"]   = "LEA LAS INSTRUCCIONES DE USO ANTES DE UTILIZAR"
        element["vmImage"]  = "qrc:/images/labeling/manual.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.13;
        element["vmText"]   = "AUTORIZADO POR ANMAT: <b>PM-2478-1.</b><br>Uso exclusivo para profesionales e instituciones sanitarias."
        element["vmImage"]  = ""
        element["vmIsLast"] = true;
        model.append(element);

    }

    function labelLoadANVISA(){

        var element = {};
        element["vmIsLast"] = false;

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = vmSYSTEM_RELEASE_DATE
        element["vmImage"]  = "qrc:/images/labeling/industry_white.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.22;
        element["vmText"]   = "<b>VIEWMIND S.A</b><br>Av. 24 de Septiembre 1536, Oficina 12, Córdoba, Córdoba, Argentina.<br>DT: Romina Carranza, MP: 37665468.<br>\
<font color=\"" + VMGlobals.vmBlueSelected + "\"> info@viewmind.com</font> | <font color=\"" + VMGlobals.vmBlueSelected + "\">www.viewmind.com</font>"
        element["vmImage"]  = "qrc:/images/labeling/industry.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = "<b>VIEWMIND S.A.R.L.</b><br>House of Biohealth, 27 Rue Henri Koch, 4354 Esch-sur-Alzette, Luxemburgo"
        element["vmImage"]  = "qrc:/images/labeling/ec.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = "<b>ViewMind Atlas System for Cognition (Versão global " + vmSYSTEM_VERSION + ")</b>"
        element["vmImage"]  = "qrc:/images/labeling/md.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = "LEIA AS INSTRUÇÕES DE OPERAÇÃO ANTES DE USAR"
        element["vmImage"]  = "qrc:/images/labeling/manual.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.21;
        element["vmText"]   = "Registro ANVISA N° " + vmANVISA_REG_NUMBER + "<br>\
Detentor do registro: Domo Salute Consultoria Regulatória Ltda.<br>\
Avenida Cristóvão Colombo, 2948 cj. 411 CEP 90560-002, <br>\
Porto Alegre/RS, Brasil<br>\
Responsável Técnico: Eng. Diego Louzada CREA/RS 162977"
        element["vmImage"]  = ""
        element["vmIsLast"] = true;
        model.append(element);


    }

    function labelLoadUSA(){

        var element = {};
        element["vmIsLast"] = false;

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = vmSYSTEM_RELEASE_DATE
        element["vmImage"]  = "qrc:/images/labeling/industry_white.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.22;
        element["vmText"]   = "<b>VIEWMIND S.A</b><br>Av. 24 de Septiembre 1536, Oficina 12, Córdoba, Córdoba, Argentina.<br>DT: Romina Carranza, MP: 37665468.<br>\
<font color=\"" + VMGlobals.vmBlueSelected + "\"> info@viewmind.com</font> | <font color=\"" + VMGlobals.vmBlueSelected + "\">www.viewmind.com</font>"
        element["vmImage"]  = "qrc:/images/labeling/industry.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = "<b>VIEWMIND INC</b><br>576 5th Ave #903, New York, NY 10036 USA."
        element["vmImage"]  = "qrc:/images/labeling/usagent.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.14;
        element["vmText"]   = "<b>ViewMind Atlas System for Cognition (Global Version " + vmSYSTEM_VERSION + ")</b>"
        element["vmImage"]  = "qrc:/images/labeling/md.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.17;
        element["vmText"]   = "READ THE OPERATING INSTRUCTIONS BEFORE USE"
        element["vmImage"]  = "qrc:/images/labeling/manual.png"
        model.append(element);


        element["vmHeight"] = labelListView.height*0.12;
        element["vmText"]   = "CAUTION: Federal law restricts this device to sale by or on the order<br>of a physician"
        element["vmImage"]  = "qrc:/images/labeling/rx-only.png"
        element["vmIsLast"] = true;
        model.append(element);

        element["vmHeight"] = labelListView.height*0.07;
        element["vmText"]   = "<b>CAUTION. Investigational device. Limited by United States law to<br>investigational use.</b>"
        element["vmImage"]  = ""
        element["vmIsLast"] = true;
        model.append(element);


    }

    function labelLoadEU(){

        var element = {};
        element["vmIsLast"] = false;

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = vmSYSTEM_RELEASE_DATE
        element["vmImage"]  = "qrc:/images/labeling/industry_white.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.25;
        element["vmText"]   = "<b>VIEWMIND S.A</b><br>Av. 24 de Septiembre 1536, Oficina 12, Córdoba, Córdoba, Argentina.<br>DT: Romina Carranza, MP: 37665468.<br>\
<font color=\"" + VMGlobals.vmBlueSelected + "\"> info@viewmind.com</font> | <font color=\"" + VMGlobals.vmBlueSelected + "\">www.viewmind.com</font>"
        element["vmImage"]  = "qrc:/images/labeling/industry.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = "<b>VIEWMIND S.A.R.L.</b><br>House of Biohealth, 27 Rue Henri Koch, 4354 Esch-sur-Alzette, Luxemburg"
        element["vmImage"]  = "qrc:/images/labeling/ec.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.15;
        element["vmText"]   = "<b>ViewMind Atlas System for Cognition (Global Version " + vmSYSTEM_VERSION + ")</b>"
        element["vmImage"]  = "qrc:/images/labeling/md.png"
        model.append(element);

        element["vmHeight"] = labelListView.height*0.17;
        element["vmText"]   = "READ THE OPERATING INSTRUCTIONS BEFORE USE"
        element["vmImage"]  = "qrc:/images/labeling/manual.png"
        model.append(element);


        element["vmHeight"] = labelListView.height*0.13;
        element["vmText"]   = "<b>The use of this device is exclusively for clinical research.</b>"
        element["vmImage"]  = ""
        element["vmIsLast"] = true;
        model.append(element);

    }



}


