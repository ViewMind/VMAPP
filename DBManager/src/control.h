#ifndef CONTROL_H
#define CONTROL_H

#include <QThread>
#include "consoleinputscreen.h"
#include "DBQueries.h"

#define PROGRAM_NAME      "DBManager"
#define PROGRAM_VERSION   "2.0.0"

#define MENU_OPTION_NEW_INSTITUTION       0
#define MENU_OPTION_UPDATE_INSTITUTION    1
#define MENU_OPTION_NEW_PRODUCT           2
#define MENU_OPTION_UPDATE_PRODUCT        3
#define MENU_OPTION_SEARCH_PRODUCT        4
#define MENU_DELETE_TEST_ENTRIES          5
#define MENU_OPTION_RESET_PASSWD          6
#define MENU_OPTION_EXIT                  7


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
    DBQueries db;

    // Auxiliary functions
    QStringList getGreeting() const;

    // Menu action functions.
    void newInstitutions();
    void updateInstitution();
    void resetPasswInstitution();
    bool deleteTestEntries();
    void addPlacedProductForInstitution();
    void searchForProducts();
    void modifyProducts();

    DBQueries::StringMap institutionSelection(bool *isOk);
    DBQueries::StringMap productSelection(bool *isOk, const QString &instUID);
    DBQueries::StringMap inputInstitutionInfo(DBQueries::StringMap inst, bool *accepted);
    DBQueries::StringMap inputProductInfo(DBQueries::StringMap product, bool *accepted);
    void showInstitutionInfoScreen(bool showPassword, const QString &keyid);
    void showPlacedProductInfoScreen(const QString & productKeyid, QStringList *result = nullptr);

};

#endif // CONTROL_H
