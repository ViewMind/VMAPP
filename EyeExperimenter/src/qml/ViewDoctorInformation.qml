import QtQuick 2.6
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1

Dialog {

    readonly property string keybase: "viewdrinfo_"

    id: viewDoctorInformation
    modal: true
    width: 654
    height: 686
    closePolicy: Popup.CloseOnEscape

    contentItem: Rectangle {
        id: rectDialog
        anchors.fill: parent
        layer.enabled: true
        layer.effect: DropShadow{
            radius: 5
        }
    }

    VMDefines{
        id: vmDefines
    }

    Connections{
        target: loader
        onNewDoctorAdded:{
            loader.getUserDoctorInfoFromDB();
            viewDoctorInformation.close();
        }
    }

    function clearAllFields(){
        labelAddress.clear();
        labelCity.clear();
        docTypes.currentIndex = 0;
        labelLastName.clear();
        labelDocument_number.clear();
        labelInstitution.clear();
        labelMail.clear();
        labelName.clear();
        labelPhone.clear();
        labelProvince.clear();
    }

    // Creating the close button
    VMDialogCloseButton {
        id: btnClose
        anchors.top: parent.top
        anchors.topMargin: 22
        anchors.right: parent.right
        anchors.rightMargin: 25
        onClicked: {
            viewDoctorInformation.close()
        }
    }

    // The Doctor Information Title and subtitle
    Text {
        id: diagTitle
        font.pixelSize: 18
        font.family: viewHome.gothamB.name
        color: "#297FCA"
        text: loader.getStringForKey(keybase+"title");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 38
    }

    Text {
        id: diagSubTitle
        font.pixelSize: 11
        font.family: viewHome.gothamR.name
        color: "#cfcfcf"
        text: loader.getStringForKey(keybase+"subtitle");
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: diagTitle.bottom
        anchors.topMargin: 10
    }

    // The form fields

    // Name and institution
    VMTextDataInput{
        id: labelName
        width: 164
        anchors.left: parent.left
        anchors.leftMargin: 149
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: 31
        vmPlaceHolder: loader.getStringForKey(keybase+"labelName");
    }

    VMTextDataInput{
        id: labelLastName
        width: 164
        anchors.left: labelName.right
        anchors.leftMargin: 15
        anchors.top: diagSubTitle.bottom
        anchors.topMargin: 31
        vmPlaceHolder: loader.getStringForKey(keybase+"labelLastName");
    }

    VMTextDataInput{
        id: labelInstitution
        width: 343
        anchors.left: labelName.left
        anchors.top: labelLastName.bottom
        anchors.topMargin: 22
        vmPlaceHolder: loader.getStringForKey(keybase+"labelInstitution");
    }

    // Document type and number.
    VMComboBox{
        id: docTypes
        width: 150
        vmModel:  loader.getStringListForKey(keybase+"docTypes")
        font.family: viewHome.robotoR.name
        anchors.top: labelInstitution.bottom
        anchors.topMargin: 22
        anchors.left: labelName.left
    }

    VMTextDataInput{
        id: labelDocument_number
        width: 178
        anchors.left: docTypes.right
        anchors.leftMargin: 15
        anchors.bottom: docTypes.bottom
        vmPlaceHolder: loader.getStringForKey(keybase+"labelDocument_number");
    }

    // Country, province and city
    VMComboBox{
        id: labelCountry
        vmModel: {
            var data = loader.getCountryList();
            data.unshift(loader.getStringForKey(keybase+"labelCountry"));
            return data;
        }
        currentIndex:  loader.getDefaultCountry();
        width: 343
        font.family: viewHome.robotoR.name
        font.pixelSize: 13
        anchors.top: labelDocument_number.bottom
        anchors.topMargin: 22
        anchors.left: labelName.left
    }

    VMTextDataInput{
        id: labelProvince
        width: 164
        anchors.left: labelName.left
        anchors.top: labelCountry.bottom
        anchors.topMargin: 20
        vmPlaceHolder: loader.getStringForKey(keybase+"labelProvince");
    }

    VMTextDataInput{
        id: labelCity
        width: 164
        anchors.left: labelProvince.right
        anchors.leftMargin: 15
        anchors.top: labelCountry.bottom
        anchors.topMargin: 20
        vmPlaceHolder: loader.getStringForKey(keybase+"labelCity");
    }

    // Phone, address and email
    VMTextDataInput{
        id: labelPhone
        width: 343
        anchors.left: labelName.left
        anchors.top: labelProvince.bottom
        anchors.topMargin: 15
        vmPlaceHolder: loader.getStringForKey(keybase+"labelPhone");
    }

    VMTextDataInput{
        id: labelAddress
        width: 343
        anchors.left: labelName.left
        anchors.top: labelPhone.bottom
        anchors.topMargin: 15
        vmPlaceHolder: loader.getStringForKey(keybase+"labelAddress");
    }

    VMTextDataInput{
        id: labelMail
        width: 343
        anchors.left: labelName.left
        anchors.top: labelAddress.bottom
        anchors.topMargin: 15
        vmPlaceHolder: loader.getStringForKey(keybase+"labelMail");
    }

    // Buttons
    Row{

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: labelMail.bottom
        anchors.topMargin: 43
        spacing: 15

        VMButton{
            id: btnBack
            height: 50
            vmText: loader.getStringForKey(keybase+"btnBack");
            vmFont: viewHome.gothamM.name
            vmInvertColors: true
            onClicked: {
                viewDoctorInformation.close()
            }
        }

        VMButton{
            id: btnSave
            vmText: loader.getStringForKey(keybase+"btnSave");
            vmFont: viewHome.gothamM.name
            onClicked: {

                // THIS IS THE TABLE DATA.
                var dbData = {
                    uid: labelDocument_number.vmEnteredText,
                    idtype: docTypes.currentText,
                    firstname: labelName.vmEnteredText,
                    lastname: labelLastName.vmEnteredText,
                    countryid: labelCountry.currentText,
                    state: labelProvince.vmEnteredText,
                    city: labelCity.vmEnteredText,
                    medicalinstitution: labelInstitution.vmEnteredText,
                    telephone: labelPhone.vmEnteredText,
                    email: labelMail.vmEnteredText,
                    address: labelAddress.vmEnteredText
                };

                // The absolute must values are the document, the country, the name and the last name.
                if (labelCountry.currentIndex === 0){
                    labelCountry.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
                    return;
                }

                if (dbData.uid === ""){
                    labelDocument_number.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
                    return;
                }

                if (dbData.firstname === ""){
                    labelName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
                    return;
                }

                if (dbData.lastname === ""){
                    labelLastName.vmErrorMsg = loader.getStringForKey(keybase + "errorEmpty");
                    return;
                }

                loader.addNewDoctorToDB(dbData);
                viewDoctorInformation.close();

            }
        }
    }

}
