#ifndef CONTROL_H
#define CONTROL_H

#include <QThread>
#include "consoleinputscreen.h"
#include "InstDBComm.h"

#define PROGRAM_NAME      "DBManager"
#define PROGRAM_VERSION   "0.0.1"

#define INPUT_NAME     0
#define INPUT_NEVAL    1
#define INPUT_BRAND    2
#define INPUT_MODEL    3
#define INPUT_ETSN     4

class Control : public QThread
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = 0);
    void run();

signals:
    void exitRequested();


private:

    // The database connection configuration.
    ConfigurationManager dbconf;

    // The menu screens
    ConsoleInputScreen loadScreen;
    ConsoleInputScreen mainMenu;

    // Interface to the database
    InstDBComm db;

    // Auxiliary functions
    QStringList getGreeting() const;
    bool initialization();
    void newInstitutions();
    void updateInstitution();
    void resetPasswInstitution();
    void printInstitutionInfo();

    // Comunications variables.
    bool commTransactionOk;
    qint32 commSelection;
    QString commKeyid;
    InstDBComm::Institution commInstitutionInfo;

    void institutionSelection();
    void inputInstitutionInfo(bool update);
    void showInfoScreen(bool showPassword = true);


};

#endif // CONTROL_H
