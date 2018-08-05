#include "sslwrapper.h"

SSLWrapper::SSLWrapper(QObject *parent) : QObject(parent)
{

    someRandomStrings << "drown" << "muscle" << "hot" << "delirious" << "satisfying" << "subtract" << "harm" << "zealous" << "guitar" << "squeamish" << "jail"
                      << "trains" << "fear" << "list" << "homeless" << "tub" << "analyse" << "quarter" << "memorise" << "rifle" << "harmonious" << "wicked" << "church"
                      << "babies" << "innate" << "easy" << "literate" << "cherry" << "stretch" << "trick" << "force" << "tie" << "sail" << "passenger" << "guide"
                      << "loaf" << "highfalutin" << "sharp" << "ladybug" << "violent";

    someRandomNames << "Willia Griswold" << "Roberto Bagnell" << "Ignacio Deloach" << "Margarete Kearney" << "Charmaine Mcaninch" << "Ned Mendel" << "Joye Weinberg"
                    << "Sam Pankratz" << "Edgardo Dunneback" << "Jami Alpert" << "Maragret Magallanes" << "Arica Mcclaskey" << "Golda Elwell" << "Jeana Concha"
                    << "Shaina Stanback" << "Vivien Mccawley" << "Carly Wills" << "Buena Roehl" << "Felecia Peed" << "Onita Mcphee" << "Allena Rippey" << "Viviana Winchenbach"
                    << "Mirian Chiang" << "Phuong Bittinger" << "Tyrone Santucci" << "Oren Grindstaff" << "Vivan Gatling" << "Tierra Schaller" << "Corliss Hurd" << "Kathe Motyka"
                    << "Winnifred Yamasaki" << "Carmelita Nagata" << "Daria Almodovar" << "Kayce Plumb" << "Marylee Harkey" << "Otis Vanarsdale" << "Debrah Woods" << "Nenita Defilippo"
                    << "Eric Steimle" << "Jovita Yao" << "Eloisa Perrodin" << "Florentino Roop" << "Fay Machnik" << "Taryn Eberhardt" << "Cecila Lam" << "Carmine Hynes" << "Adela Basso"
                    << "Kyra Chute" << "Willodean Carmean" << "Estella Rickerson";

    // This is fixed due to this being a simple test.
    //config.addKeyValuePair(CONFIG_SERVER_ADDRESS,"localhost");
    config.addKeyValuePair(CONFIG_SERVER_ADDRESS,"18.220.30.34");

    // Used when compiling for test in the EC2 Server.
    //config.addKeyValuePair(CONFIG_SERVER_ADDRESS,"18.220.30.34");

    config.addKeyValuePair(CONFIG_CONNECTION_TIMEOUT,60);
    config.addKeyValuePair(CONFIG_WAIT_DBDATA_TIMEOUT,60);
    config.addKeyValuePair(CONFIG_WAIT_REPORT_TIMEOUT,60);
    config.addKeyValuePair(CONFIG_DATA_REQUEST_TIMEOUT,60);

    // Data for data processing. All fixed values for a quick testing
    config.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"/home/ariela/Workspace/Viewmind/test_patient/");
    config.addKeyValuePair(CONFIG_DOCTOR_NAME,"Pez Ecito");

    // For the Request Report to work properly, these NEED to be in the DB.
    config.addKeyValuePair(CONFIG_DOCTOR_UID,"AR31653499");
    config.addKeyValuePair(CONFIG_PATIENT_UID,"AR26844014");

    config.addKeyValuePair(CONFIG_DEMO_MODE,"false");

    clientDB = new SSLDBClient(this,&config);
    connect(clientDB,SIGNAL(transactionFinished(bool)),this,SLOT(onTransactionFinished(bool)));
    connect(clientDB,SIGNAL(diconnectionFinished()),this,SLOT(onDisconnectionDone()));

    clientDataProcessing = new SSLDataProcessingClient(this,&config);
    connect(clientDataProcessing,SIGNAL(transactionFinished(bool)),this,SLOT(onTransactionFinishedForDataProcessing(bool)));
    connect(clientDataProcessing,SIGNAL(diconnectionFinished()),this,SLOT(onDisconnectionDone()));

    doingProcessing = false;
    qsrand(static_cast<quint64>(QTime::currentTime().msecsSinceStartOfDay()));

}

void SSLWrapper::sendDataToProcess(){
    doingProcessing = true;
    clientDataProcessing->requestReport();
}

void SSLWrapper::addSomeRandomDoctorData(qint32 n){

    isGET = false;
    doingProcessing = false;
    clientDB->setDBTransactionType(SQL_QUERY_TYPE_SET);

    for (qint32 i = 0; i < n; i++){

        QHash<QString,QString> data;

        qint32 dni = 20;
        dni = dni + (qrand() % 15);
        dni = dni * 1000000 + (qrand() % 999999);

        qint32 year = 2018 - (qrand() & 70);
        qint32 day = (qrand() % 29) + 1;
        qint32 month = (qrand() % 12) + 1;

        QString isodate = QString::number(year) + "-";
        QString mm = QString::number(month);
        if (month < 10) mm = "0" + mm;
        QString dd = QString::number(day);
        if (day < 10) dd = "0" + dd;
        isodate = isodate + mm + "-" + dd;


        data[TDOCTOR_COL_CITY] = getRandomStringFromList();
        data[TDOCTOR_COL_COUNTRYID] = "AR";
        data[TDOCTOR_COL_FIRSTNAME] = getRandomFirstName();
        data[TDOCTOR_COL_LASTNAME] =  getRandomLastName();
        data[TDOCTOR_COL_MEDICAL_INST] = getRandomStringFromList();
        data[TDOCTOR_COL_STATE] = getRandomStringFromList();
        data[TDOCTOR_COL_UID] = data[TDOCTOR_COL_COUNTRYID] + QString::number(dni);
        createdDRDNI << data[TDOCTOR_COL_UID];
        data[TDOCTOR_COL_EMAIL] = getRandomStringFromList() + "@" + getRandomStringFromList() + ".com";

        // Adding the transaction
        QStringList columns = data.keys();
        QStringList values;
        for (qint32 i = 0; i < columns.size(); i++){
            values << data.value(columns.at(i));
        }

        clientDB->appendSET(TABLE_DOCTORS,columns,values);
    }

    clientDB->runDBTransaction();

}

void SSLWrapper::sendSomeRandomPatientData(qint32 n, bool sendOpt){

    if (createdDRDNI.isEmpty()){
        qWarning() << "No Doctors created!!!!. Not doing anything";
        return;
    }

    enableOpt = sendOpt;
    isGET = false;
    doingProcessing = false;
    clientDB->setDBTransactionType(SQL_QUERY_TYPE_SET);
    for (qint32 i = 0; i < n; i++){
        addSETTransaction();
    }
    clientDB->runDBTransaction();

}

void SSLWrapper::requestPatientData(const QStringList &uid){

    QStringList cols;
    cols << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_LASTNAME << TPATREQ_COL_DOCTORID << TPATREQ_COL_COUNTRYID << TPATREQ_COL_BIRTHCOUNTRY << TPATREQ_COL_SEX << TPATREQ_COL_BIRTHDATE;
    QStringList colsOpt;
    colsOpt << TPATOPT_COL_KEYID << TPATOPT_COL_PATIENTID  << TPATOPT_COL_DATE_INS << TPATOPT_COL_CITY << TPATOPT_COL_MAINACTIVITY
            << TPATOPT_COL_HIGHESTDEGREE << TPATOPT_COL_PHYSICALACTIVITY << TPATOPT_COL_NOFSPOKENLANG << TPATOPT_COL_AGREESHAREDATA << TPATOPT_COL_FAMILYHISTORY
            << TPATOPT_COL_STATE << TPATOPT_COL_PATIENTHISTORY << TPATOPT_COL_REMEDIES << TPATOPT_COL_DIAGNOSTIC;

    isGET = true;
    clientDB->setDBTransactionType(SQL_QUERY_TYPE_GET);
    doingProcessing = false;

    for (qint32 i = 0; i < uid.size(); i++){
        QString condition = QString(TPATREQ_COL_UID) + " = '" + uid.at(i) + "'";
        clientDB->appendGET(TABLE_PATIENTS_REQ_DATA,cols,condition);
        condition = QString(TPATOPT_COL_PATIENTID) + " = '" + uid.at(i) + "'";
        clientDB->appendGET(TABLE_PATIENTS_OPT_DATA,colsOpt,condition);
    }

    clientDB->runDBTransaction();

}

void SSLWrapper::onTransactionFinishedForDataProcessing(bool isOk){
    if (isOk) qWarning() << "Data processing all done!";
    else qWarning() << "Data processing FAILED!";
}

void SSLWrapper::onTransactionFinished(bool isOk){

    if (doingProcessing){
        if (isOk) qWarning() << "Should have report info";
        else qWarning() << "Something went wrong";
        return;
    }

    if (!isGET){
        if (isOk){
            qWarning() << "SET Transaction success";
        }
        else{
            qWarning() << "SET transaction failed";
        }
    }
    else{
        if (isOk){

            QList<DBData> dbdata = clientDB->getDBData();

            for (qint32 i = 0; i < dbdata.size(); i++){
                qWarning() << "TRANSACTION" << i;
                qWarning() << "   Columns: " << dbdata.at(i).columns;
                qWarning() << "   Error: " << dbdata.at(i).error;
                for (qint32 j = 0; j < dbdata.at(i).rows.size(); j++){
                    qWarning() << "   ROW" << j << dbdata.at(i).rows.at(j);
                }
            }

        }
        else{
            qWarning() << "GET Transaction failed";
        }
    }

}

void SSLWrapper::onDisconnectionDone(){
    qWarning() << "Disconnection completed";
}

void SSLWrapper::addSETTransaction(){

    QHash<QString,QString> data;

    qint32 dni = 20;
    dni = dni + (qrand() % 15);
    dni = dni * 1000000 + (qrand() % 999999);

    qint32 year = 2018 - (qrand() & 70);
    qint32 day = (qrand() % 29) + 1;
    qint32 month = (qrand() % 12) + 1;

    QString isodate = QString::number(year) + "-";
    QString mm = QString::number(month);
    if (month < 10) mm = "0" + mm;
    QString dd = QString::number(day);
    if (day < 10) dd = "0" + dd;
    isodate = isodate + mm + "-" + dd;


    data[TPATREQ_COL_BIRTHCOUNTRY] = "AR";
    data[TPATREQ_COL_DOCTORID] = getRandomDrUID();
    data[TPATREQ_COL_FIRSTNAME] = getRandomFirstName();
    data[TPATREQ_COL_COUNTRYID] = QString::number(dni);
    data[TPATREQ_COL_BIRTHDATE] = isodate;
    data[TPATREQ_COL_LASTNAME] = getRandomLastName();
    data[TPATREQ_COL_SEX] = ((qrand() % 2) == 0) ? "M" : "F";
    data[TPATREQ_COL_UID] = data[TPATREQ_COL_BIRTHCOUNTRY] + data[TPATREQ_COL_COUNTRYID];

    // Adding the transaction
    QStringList columns = data.keys();
    QStringList values;
    for (qint32 i = 0; i < columns.size(); i++){
        values << data.value(columns.at(i));
    }

    clientDB->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);

    if (enableOpt){

        QString patID = data[TPATREQ_COL_UID];
        data.clear();

        data[TPATOPT_COL_AGREESHAREDATA] = QString::number((qrand() % 2));
        data[TPATOPT_COL_CITY] = getRandomStringFromList();
        data[TPATOPT_COL_DIAGNOSTIC] = getRandomStringFromList();
        data[TPATOPT_COL_FAMILYHISTORY] = getRandomStringFromList() + DB_LIST_IN_COL_SEP + getRandomStringFromList();
        data[TPATOPT_COL_HIGHESTDEGREE] = getRandomStringFromList();
        data[TPATOPT_COL_MAINACTIVITY] = getRandomStringFromList();
        data[TPATOPT_COL_NOFSPOKENLANG] = QString::number((qrand() % 4)+1);
        data[TPATOPT_COL_PATIENTID] = patID;
        data[TPATOPT_COL_PHYSICALACTIVITY] = getRandomStringFromList();
        data[TPATOPT_COL_REMEDIES] = getRandomStringFromList() + DB_LIST_IN_COL_SEP + getRandomStringFromList() + DB_LIST_IN_COL_SEP + getRandomStringFromList();
        data[TPATOPT_COL_PATIENTHISTORY] = getRandomStringFromList() + DB_LIST_IN_COL_SEP + getRandomStringFromList() + DB_LIST_IN_COL_SEP + getRandomStringFromList();
        data[TPATOPT_COL_DATE_INS] = "TIMESTAMP(NOW())";

        QStringList columns = data.keys();
        QStringList values;
        for (qint32 i = 0; i < columns.size(); i++){
            values << data.value(columns.at(i));
        }

        clientDB->appendSET(TABLE_PATIENTS_OPT_DATA,columns,values);
    }

}
