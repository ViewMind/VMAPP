#ifndef CONSOLEINPUTSCREEN_H
#define CONSOLEINPUTSCREEN_H

#include <QStringList>
#include <QVariant>
#include <QProcess>
#include <iostream>

class ConsoleInputScreen
{
public:

    ConsoleInputScreen();
    typedef enum {CIT_MENU, CIT_DATA_INPUT, CIT_INFORMATION, CIT_QUESTION} ConsoleInputType;
    typedef enum {CA_BACK, CA_SUBMIT, CA_NEXT} ConsoleAction;

    // Set options associated and data for a Menu Screen
    void setMenuTitle(const QString &title) {menuTitle = title;}
    void addMenuOption(const QString &option, const QVariant &data = QVariant());

    // Set prompts for data entry.
    void addDataEntryPrompt(const QString &prompt, const QString &dataID);

    // Set lines of texts for information show
    void setInformationScreen(const QStringList &information, bool isGoBackAnOption);

    // Set question for confirmation
    void setQuestion(const QString &question);

    // Show the screen acoording to the configuration.
    void show(bool noInput = false);

    // Clear all information.
    void clearAll();

    // Getting the information
    ConsoleAction getAction() const {return action;}
    QVariant getSelectedData() const {if (dataForMenu.size() > 0) return dataForMenu.at(selected); else return QVariant();}
    qint32 getSelected() const {return selected;}
    QStringList getInputedData() const {return inputData;}
    QStringList getMenuEntryIDs() const {return menuEntryID;}


private:

    // Maximum number of allowed options to be shown on one screen.
    const qint32 MAX_ALLOWED_OPTIONS = 30;

    ConsoleInputType screenType;
    ConsoleAction action;
    QString menuTitle;
    QStringList menuText;
    QStringList menuEntryID;
    QVariantList dataForMenu;
    QStringList inputData;
    qint32 selected;
    bool useGoBack;
    bool addedNumbers;

    void showMenu();
    void showQuestion();
    void showPrompts();
    void showInformation(bool noInput);
};

#endif // CONSOLEINPUTSCREEN_H
