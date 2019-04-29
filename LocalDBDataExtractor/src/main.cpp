#include <QCoreApplication>
#include <QVariantMap>
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "../../CommonClasses/server_defines.h"
#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //************************ Getting the path to the local DB file
    QStringList args = QCoreApplication::arguments();
    QString inputDB;
    if (args.size() == 2){
        inputDB = args.last();
    }
    else {
        if (QFile(LOCAL_DB).exists()){
            inputDB = LOCAL_DB;
        }
        else{
            qDebug() << "The path of the local DB file must be inputted or a local db must be present in the same directory as the exectuable";
            exit(1);
        }
    }

    //************************ Setting up the ouput file
    QFileInfo info(inputDB);
    QString outputFile = info.path() + "/patient_info";

    //************************ Loading the local DB
    LocalInformationManager lim;
    lim.setWorkingFile(inputDB);
    QHash<QString,QString> hashedIDMap = lim.getPatientHashedIDMap();

    QStringList hasheduids = hashedIDMap.keys();
    //    for (qint32 i = 0; i < hasheduids.size(); i++){
    //        qDebug() << hasheduids.at(i) << hashedIDMap.value(hasheduids.at(i));
    //    }

    //************************ Establishing a DB Connection
    DBInterface dbi;
    QString configurationFile = ":/CommonClasses/DBConfigurations/db_localhost_prod";

    ConfigurationManager dbconfigs;

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    // DB configuration is all strings.
    cmd.clear();
    cv[CONFIG_DBHOST] = cmd;
    cv[CONFIG_DBNAME] = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cv[CONFIG_ID_DBHOST] = cmd;
    cv[CONFIG_ID_DBNAME] = cmd;
    cv[CONFIG_ID_DBPASSWORD] = cmd;
    cv[CONFIG_ID_DBUSER] = cmd;

    cv[CONFIG_PATDATA_DBHOST] = cmd;
    cv[CONFIG_PATDATA_DBNAME] = cmd;
    cv[CONFIG_PATDATA_DBPASSWORD] = cmd;
    cv[CONFIG_PATDATA_DBUSER] = cmd;

    cv[CONFIG_S3_ADDRESS] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DBPORT] = cmd;
    cv[CONFIG_ID_DBPORT] = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;

    dbconfigs.setupVerification(cv);
    if (!dbconfigs.loadConfiguration(configurationFile,COMMON_TEXT_CODEC)){
        qDebug() << "DB Configuration file errors:"  << dbconfigs.getError();
        exit(1);
    }

    QString host   = dbconfigs.getString(CONFIG_ID_DBHOST);
    QString dbname = dbconfigs.getString(CONFIG_ID_DBNAME);
    QString user   = dbconfigs.getString(CONFIG_ID_DBUSER);
    QString passwd = dbconfigs.getString(CONFIG_ID_DBPASSWORD);
    quint16 port   = dbconfigs.getInt(CONFIG_ID_DBPORT);
    dbi.setupDB(DB_NAME_ID,host,dbname,user,passwd,port,"");

    if (!dbi.open()){
        qDebug() << "Could not open DB interface";
        exit(1);
    }

    //************************ Running a query to get the desired information
    QStringList columns; columns << TPATID_COL_KEYID << TPATID_COL_UID;
    QString query = "SELECT " + columns.join(",") + " FROM " + QString(TABLE_PATIENTD_IDS) + " WHERE " + QString(TPATID_COL_UID) + " IN ('" + hasheduids.join("','") + "')";

    //qDebug() << "RUNNING QUERY" << query;

    if (!dbi.specialQuery(query,columns)){
        qDebug() << "Could not execute query: " + query + ", Error: " + dbi.getError();
        exit(1);
    }

    DBData results = dbi.getLastResult();

    if (results.rows.isEmpty()){
        qDebug() << "No results for query" << query;
        exit(1);
    }

    for (qint32 i = 0; i < results.rows.size(); i++){

        if (results.rows.at(i).size() != columns.size()){
            qDebug() << "Number of columns at row" << i << "is" << results.rows.at(i).size();
            exit(1);
        }

        QString hash = results.rows.at(i).last();
        QString puid = results.rows.at(i).first();

        hashedIDMap[hash] = hashedIDMap.value(hash) + " -> " + puid;

    }

    //************************ Saving the extracted information to the output file
    QFile outfile(outputFile);
    if (!outfile.open(QFile::WriteOnly)){
        qDebug() << "Cannot open" << outputFile << "for writing";
        exit(1);
    }

    QTextStream writer(&outfile);
    hasheduids = hashedIDMap.keys();
    for (qint32 i = 0; i < hasheduids.size(); i++){
        writer << hashedIDMap.value(hasheduids.at(i)) << "\n";
    }
    outfile.close();

    qDebug() << "FINISHED. Generated output file" << outputFile;

    exit(0);
    return a.exec();
}
