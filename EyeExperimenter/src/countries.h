#ifndef COUNTRIES_H
#define COUNTRIES_H

#include <QStringList>

struct CountryStruct {

public:
    void fillCountryList(bool inEnglish = true) {
        if (COUNTRY_CODE.size() != 0) return;
        /// TODO: Get the country list in spanish.
        if (inEnglish) {
            COUNTRY_LIST <<  "Afghanistan" << "Åland Islands" << "Albania" << "Algeria" << "American Samoa" << "Andorra" << "Angola" << "Anguilla" << "Antarctica"
                          << "Antigua and Barbuda" << "Argentina" << "Armenia" << "Aruba" << "Australia" << "Austria" << "Azerbaijan" << "Bahamas" << "Bahrain"
                          << "Bangladesh" << "Barbados" << "Belarus" << "Belgium" << "Belize" << "Benin" << "Bermuda" << "Bhutan" << "Bolivia, Plurinational State of"
                          << "Bonaire, Sint Eustatius and Saba" << "Bosnia and Herzegovina" << "Botswana" << "Bouvet Island" << "Brazil" << "British Indian Ocean Territory"
                          << "Brunei Darussalam" << "Bulgaria" << "Burkina Faso" << "Burundi" << "Cambodia" << "Cameroon" << "Canada" << "Cape Verde" << "Cayman Islands"
                          << "Central African Republic" << "Chad" << "Chile" << "China" << "Christmas Island" << "Cocos (Keeling) Islands" << "Colombia" << "Comoros" << "Congo"
                          << "Congo, the Democratic Republic of the" << "Cook Islands" << "Costa Rica" << "Côte d'Ivoire" << "Croatia" << "Cuba" << "Curaçao" << "Cyprus"
                          << "Czech Republic" << "Denmark" << "Djibouti" << "Dominica" << "Dominican Republic" << "Ecuador" << "Egypt" << "El Salvador" << "Equatorial Guinea"
                          << "Eritrea" << "Estonia" << "Ethiopia" << "Falkland Islands (Malvinas)" << "Faroe Islands" << "Fiji" << "Finland" << "France" << "French Guiana"
                          << "French Polynesia" << "French Southern Territories" << "Gabon" << "Gambia" << "Georgia" << "Germany" << "Ghana" << "Gibraltar" << "Greece"
                          << "Greenland" << "Grenada" << "Guadeloupe" << "Guam" << "Guatemala" << "Guernsey" << "Guinea" << "Guinea-Bissau" << "Guyana" << "Haiti"
                          << "Heard Island and McDonald Islands" << "Holy See (Vatican City State)" << "Honduras" << "Hong Kong" << "Hungary" << "Iceland" << "India"
                          << "Indonesia" << "Iran, Islamic Republic of" << "Iraq" << "Ireland" << "Isle of Man" << "Israel" << "Italy" << "Jamaica" << "Japan" << "Jersey"
                          << "Jordan" << "Kazakhstan" << "Kenya" << "Kiribati" << "Korea, Democratic People's Republic of" << "Korea, Republic of" << "Kuwait" << "Kyrgyzstan"
                          << "Lao People's Democratic Republic" << "Latvia" << "Lebanon" << "Lesotho" << "Liberia" << "Libya" << "Liechtenstein" << "Lithuania" << "Luxembourg"
                          << "Macao" << "Macedonia, the former Yugoslav Republic of" << "Madagascar" << "Malawi" << "Malaysia" << "Maldives" << "Mali" << "Malta"
                          << "Marshall Islands" << "Martinique" << "Mauritania" << "Mauritius" << "Mayotte" << "Mexico" << "Micronesia, Federated States of"
                          << "Moldova, Republic of" << "Monaco" << "Mongolia" << "Montenegro" << "Montserrat" << "Morocco" << "Mozambique" << "Myanmar" << "Namibia" << "Nauru"
                          << "Nepal" << "Netherlands" << "New Caledonia" << "New Zealand" << "Nicaragua" << "Niger" << "Nigeria" << "Niue" << "Norfolk Island"
                          << "Northern Mariana Islands" << "Norway" << "Oman" << "Pakistan" << "Palau" << "Palestinian Territory, Occupied" << "Panama" << "Papua New Guinea"
                          << "Paraguay" << "Peru" << "Philippines" << "Pitcairn" << "Poland" << "Portugal" << "Puerto Rico" << "Qatar" << "Réunion" << "Romania"
                          << "Russian Federation" << "Rwanda" << "Saint Barthélemy" << "Saint Helena, Ascension and Tristan da Cunha" << "Saint Kitts and Nevis" << "Saint Lucia"
                          << "Saint Martin (French part)" << "Saint Pierre and Miquelon" << "Saint Vincent and the Grenadines" << "Samoa" << "San Marino" << "Sao Tome and Principe"
                          << "Saudi Arabia" << "Scotland" << "Senegal" << "Serbia" << "Seychelles" << "Sierra Leone" << "Singapore" << "Sint Maarten (Dutch part)" << "Slovakia" << "Slovenia"
                          << "Solomon Islands" << "Somalia" << "South Africa" << "South Georgia and the South Sandwich Islands" << "South Sudan" << "Spain" << "Sri Lanka" << "Sudan"
                          << "Suriname" << "Svalbard and Jan Mayen" << "Swaziland" << "Sweden" << "Switzerland" << "Syrian Arab Republic" << "Taiwan, Province of China"
                          << "Tajikistan" << "Tanzania, United Republic of" << "Thailand" << "Timor-Leste" << "Togo" << "Tokelau" << "Tonga" << "Trinidad and Tobago" << "Tunisia"
                          << "Turkey" << "Turkmenistan" << "Turks and Caicos Islands" << "Tuvalu" << "Uganda" << "Ukraine" << "United Arab Emirates" << "United Kingdom"
                          << "United States" << "United States Minor Outlying Islands" << "Uruguay" << "Uzbekistan" << "Vanuatu" << "Venezuela, Bolivarian Republic of" << "Viet Nam"
                          << "Virgin Islands, British" << "Virgin Islands, U.S." << "Wallis and Futuna" << "Western Sahara" << "Yemen" << "Zambia" << "Zimbabwe";
        }
        else {
            // Only other language is spanish
            COUNTRY_LIST << "Afganistán" << "Islas Aland" << "Albania" << "Argelia" << "Samoa Americana" << "Andorra" << "Angola" << "Anguila" << "Antártico"
                         << "Antigua y Barbuda" << "Argentina" << "Armenia" << "Aruba" << "Australia" << "Austria" << "Azerbaiyán" << "Bahamas" << "Bahréin"
                         << "Bangladesh" << "Barbados" << "Bielorrusia" << "Bélgica" << "Belice" << "Benín" << "Bermudas" << "Bután" << "Bolivia"
                         << "Bonaire, San Eustaquio y Saba" << "Bosnia y Herzegovina" << "Botsuana" << "Isla Bouvet" << "Brasil"
                         << "Territorio Británico en el Océano Índico" << "Brunéi Darussalam" << "Bulgaria" << "Burkina Faso" << "Burundi" << "Camboya" << "Camerún"
                         << "Canadá" << "Cabo Verde" << "Islas Caimán" << "República Centroafricana" << "Chad" << "Chile" << "China, República Popular"
                         << "Isla de Navidad" << "Islas Cocos" << "Colombia" << "Comoras" << "Congo" << "Congo, República del" << "Islas Cook" << "Costa Rica"
                         << "Costa de Marfil" << "Croacia" << "Cuba" << "Curasao" << "Chipre" << "República Checa" << "Dinamarca" << "Yibuti" << "Dominica"
                         << "República Dominicana" << "Ecuador" << "Egipto" << "El Salvador" << "Guinea Ecuatorial" << "Eritrea" << "Estonia" << "Etiopía"
                         << "Islas Malvinas" << "Islas Feroe" << "Fiyi" << "Finlandia" << "Francia" << "Guayana Francesa" << "Polinesia Francesa"
                         << "Territorios Franceses del Sur" << "Gabón" << "Gambia" << "Georgia" << "Alemania" << "Ghana" << "Gibraltar" << "Grecia" << "Groenlandia"
                         << "Granada" << "Guadalupe" << "Guam" << "Guatemala" << "Guernesey" << "Guinea" << "Guinea-Bissau" << "Guyana" << "Haití"
                         << "Islas Heard y McDonald" << "Estado de la Ciudad del Vaticano" << "Honduras" << "Hong Kong" << "Hungría" << "Islandia" << "India"
                         << "Indonesia" << "Irán" << "Iraq" << "Irlanda" << "Isla de Man" << "Israel" << "Italia" << "Jamaica" << "Japón" << "Isla de Jersey"
                         << "Jordania" << "Kazajistán" << "Kenia" << "Kiribati" << "República de Corea, Popular Democrática de" << "Corea, República de" << "Kuwait"
                         << "Kirguistán" << "República Democrática Popular Lao" << "Letonia" << "Líbano" << "Lesoto" << "Liberia" << "Libia" << "Liechtenstein"
                         << "Lituania" << "Luxemburgo" << "Macao" << "Macedonia, República de" << "Madagascar" << "Malaui" << "Malasia" << "Maldivas" << "Malí"
                         << "Malta" << "Islas Marshall" << "Martinica" << "Mauritania" << "Mauricio" << "Mayotte" << "México" << "Micronesia, estados federados de"
                         << "Moldavia, República de" << "Mónaco" << "Mongolia" << "Montenegro" << "Montserrat" << "Marruecos" << "Mozambique" << "Myanmar (Birmania)"
                         << "Namibia" << "Nauru" << "Nepal" << "Países Bajos" << "Nueva Caledonia" << "Nueva Zelanda" << "Nicaragua" << "Níger" << "Nigeria"
                         << "Niue" << "Isla Norfolk" << "Islas Marianas del Norte" << "Noruega" << "Omán" << "Pakistán" << "Palaos" << "Palestina" << "Panamá"
                         << "Papúa Nueva Guinea" << "Paraguay" << "Perú" << "Filipinas" << "Pitcairn" << "Polonia" << "Portugal" << "Puerto Rico" << "Qatar"
                         << "Reunión" << "Rumania" << "Rusia" << "Ruanda" << "San Bartolomé" << "Santa Helena, Ascensión y Tristán da Cunha" << "San Cristóbal y Nieves"
                         << "Santa Lucía" << "San Martin" << "San Pedro y Miquelón" << "San Vicente y las Granadinas" << "Samoa" << "San Marino"
                         << "Santo Tomé y Príncipe" << "Arabia Saudita" << "Escocia" << "Senegal" << "Serbia" << "Seychelles" << "Sierra Leona" << "Singapur"
                         << "Sint Maarten" << "Eslovaquia" << "Eslovenia" << "Islas Salomón" << "Somalia" << "Sudáfrica" << "Islas Georgias del Sur y Sandwich del Sur"
                         << "Sudán del Sur" << "España" << "Sri Lanka" << "Sudán" << "Surinam" << "Svalbard y Jan Mayen" << "Suazilandia" << "Suecia" << "Suiza"
                         << "República Árabe Siria" << "Taiwan, provincia de China" << "Tayikistán" << "Tanzania, República Unida de" << "Tailandia"
                         << "Timor-Leste" << "Togo" << "Tokelau" << "Tonga" << "Trinidad y Tobago" << "Túnez" << "Turquía" << "Turkmenistán" << "Islas Turcas y Caicos"
                         << "Tuvalu" << "Uganda" << "Ucrania" << "Emiratos Árabes Unidos" << "Reino Unido" << "Estados Unidos"
                         << "Islas menores alejadas de los Estados Unidos" << "Uruguay" << "Uzbekistán" << "Vanuatu" << "Venezuela, República Bolivariana de"
                         << "Viet Nam" << "Islas Vírgenes Británicas" << "Islas Vírgenes, EE.UU." << "Wallis y Futuna" << "Sahara Occidental" << "Yemen" << "Zambia"
                         << "Zimbabue";
        }

        COUNTRY_CODE <<	"AF" <<	"AX" <<	"AL" <<	"DZ" <<	"AS" <<	"AD" <<	"AO" <<	"AI" <<	"AQ" <<	"AG" <<	"AR" <<	"AM" <<	"AW" <<	"AU" <<	"AT" <<	"AZ"
                     <<	"BS" <<	"BH" <<	"BD" <<	"BB" <<	"BY" <<	"BE" <<	"BZ" <<	"BJ" <<	"BM" <<	"BT" <<	"BO" <<	"BQ" <<	"BA" <<	"BW" <<	"BV" <<	"BR"
                     <<	"IO" <<	"BN" <<	"BG" <<	"BF" <<	"BI" <<	"KH" <<	"CM" <<	"CA" <<	"CV" <<	"KY" <<	"CF" <<	"TD" <<	"CL" <<	"CN" <<	"CX" <<	"CC"
                     <<	"CO" <<	"KM" <<	"CG" <<	"CD" <<	"CK" <<	"CR" <<	"CI" <<	"HR" <<	"CU" <<	"CW" <<	"CY" <<	"CZ" <<	"DK" <<	"DJ" <<	"DM" <<	"DO"
                     <<	"EC" <<	"EG" <<	"SV" <<	"GQ" <<	"ER" <<	"EE" <<	"ET" <<	"FK" <<	"FO" <<	"FJ" <<	"FI" <<	"FR" <<	"GF" <<	"PF" <<	"TF" <<	"GA"
                     <<	"GM" <<	"GE" <<	"DE" <<	"GH" <<	"GI" <<	"GR" <<	"GL" <<	"GD" <<	"GP" <<	"GU" <<	"GT" <<	"GG" <<	"GN" <<	"GW" <<	"GY" <<	"HT"
                     <<	"HM" <<	"VA" <<	"HN" <<	"HK" <<	"HU" <<	"IS" <<	"IN" <<	"ID" <<	"IR" <<	"IQ" <<	"IE" <<	"IM" <<	"IL" <<	"IT" <<	"JM" <<	"JP"
                     <<	"JE" <<	"JO" <<	"KZ" <<	"KE" <<	"KI" <<	"KP" <<	"KR" <<	"KW" <<	"KG" <<	"LA" <<	"LV" <<	"LB" <<	"LS" <<	"LR" <<	"LY" <<	"LI"
                     <<	"LT" <<	"LU" <<	"MO" <<	"MK" <<	"MG" <<	"MW" <<	"MY" <<	"MV" <<	"ML" <<	"MT" <<	"MH" <<	"MQ" <<	"MR" <<	"MU" <<	"YT" <<	"MX"
                     <<	"FM" <<	"MD" <<	"MC" <<	"MN" <<	"ME" <<	"MS" <<	"MA" <<	"MZ" <<	"MM" <<	"NA" <<	"NR" <<	"NP" <<	"NL" <<	"NC" <<	"NZ" <<	"NI"
                     <<	"NE" <<	"NG" <<	"NU" <<	"NF" <<	"MP" <<	"NO" <<	"OM" <<	"PK" <<	"PW" <<	"PS" <<	"PA" <<	"PG" <<	"PY" <<	"PE" <<	"PH" <<	"PN"
                     <<	"PL" <<	"PT" <<	"PR" <<	"QA" <<	"RE" <<	"RO" <<	"RU" <<	"RW" <<	"BL" <<	"SH" <<	"KN" <<	"LC" <<	"MF" <<	"PM" <<	"VC" <<	"WS"
                     <<	"SM" <<	"ST" <<	"SA" << "SQ" <<	"SN" <<	"RS" <<	"SC" <<	"SL" <<	"SG" <<	"SX" <<	"SK" <<	"SI" <<	"SB" <<	"SO" <<	"ZA" <<	"GS"
                     <<	"SS" <<	"ES" <<	"LK" <<	"SD" <<	"SR" <<	"SJ" <<	"SZ" <<	"SE" <<	"CH" <<	"SY" <<	"TW" <<	"TJ" <<	"TZ" <<	"TH" <<	"TL" <<	"TG"
                     <<	"TK" <<	"TO" <<	"TT" <<	"TN" <<	"TR" <<	"TM" <<	"TC" <<	"TV" <<	"UG" <<	"UA" <<	"AE" <<	"GB" <<	"US" <<	"UM" <<	"UY" <<	"UZ"
                     <<	"VU" <<	"VE" <<	"VN" <<	"VG" <<	"VI" <<	"WF" <<	"EH" <<	"YE" <<	"ZM" <<	"ZW";

    }

    QString getCodeForCountry(const QString &country){
        int index = COUNTRY_LIST.indexOf(country);
        if (index == -1) return "";
        else return COUNTRY_CODE.at(index);
    }

    QString getCountrFromCode (const QString &code){
        int index = COUNTRY_CODE.indexOf(code);
        if (index == -1)  return "";
        else return COUNTRY_LIST.at(index);
    }

    qint32 getIndexFromCode(const QString &countryCode){
        return COUNTRY_CODE.indexOf(countryCode);
    }

    QString getCodeFromIndex (int index){
        if (index == -1)  return "";
        else return COUNTRY_CODE.at(index);
    }

    QStringList getCountryList() const {return COUNTRY_LIST;}
    QStringList getCodeList() const {return COUNTRY_CODE;}

private:
    QStringList COUNTRY_LIST;
    QStringList COUNTRY_CODE;


};

#endif // COUNTRIES_H
