#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <QVariantHash>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDebug>

/*****************************************************
 * Saves and load a Human readable configuration file
 * with a simple syntax to a Variant Hash, for easy
 * access.
 * ***************************************************/

class ConfigurationManager
{
public:

    typedef enum {VT_INT, VT_REAL, VT_BOOL, VT_STRING, VT_LIST} ValueType;

    // The simple parser uses only three delimiters and a comment character, which are configuraable
    // TODO: Make the configuration function set the delimiters and fail if any are the same or a space.
    struct Delimiters {
        Delimiters (){
            statement = ';';
            name      = '=';
            field     = '|';
            comment   = '#';
        }
        QChar statement;
        QChar name;
        QChar field;
        QChar comment;
    };

    // Command structure to verify the data.
    struct Command {
        bool optional = false;
        ValueType type;
        QList<ValueType> fields;
        void clear(){
            optional = false;
            fields.clear();
            type = VT_STRING;
        }
    };

    typedef QHash<QString,Command> CommandVerifications;

    ConfigurationManager();
    bool loadConfiguration(const QString &file, const QString readText = "");

    // Used in order to add data and unify as the only configuration structure in a program
    void addKeyValuePair(const QString &name, const QVariant &value);

    // Used to remove a given key
    void removeKey(const QString &key) {data.remove(key);}

    // Getting the error
    QString getError() const {return error;}

    // Verify the parsed data if required
    bool setupVerification(const CommandVerifications &cv);

    // Check if a keyword was defined.
    bool containsKeyword(const QString &kw) const { return data.contains(kw); }

    // Delete all data
    void clear() {data.clear(); verif.clear();}

    // List of getter functions. They all have the same structure. They get a value based on a name and
    // ok is set to true if the data of the type can be obtained from the entry with that name.
    QString getString(const QString &name, bool *ok = nullptr) const;
    qint32  getInt(const QString &name, bool *ok = nullptr) const;
    qreal   getReal(const QString &name, bool *ok = nullptr) const;
    bool    getBool(const QString &name, bool *ok = nullptr) const;
    QStringList getStringList(const QString &name) const;
    QVariant getVariant(const QString &name) const { return data.value(name); }

    // Gets all the keys
    QStringList getAllKeys() const {return data.keys();}

    // Merge with another configuration manager.
    void merge(const ConfigurationManager &configmng);

    // Wrapper function to save a value from the configuration to a file in disc.
    QString saveValueToFile(const QString &fileName, const QString &key);

    // Save complete configuration to file
    bool saveToFile(const QString &fileName, const Delimiters &delimiters = Delimiters());

    // Functions to get to and from map.
    QVariantMap getMap() const {QVariant temp; temp = data; return temp.toMap();}
    //void fromMap(const QVariantMap &map) {QVariant temp; temp = map; data = temp.toHash();}

    // Service function to replace a value in a configuration text file.
    static QString setValue(const QString &fileName,
                            const QString &cmd,
                            const QString &value,
                            ConfigurationManager *configToChange = nullptr,
                            const Delimiters &delimiters = Delimiters());

    // Helper function to export when configuration is jsut a list of strings.
    QString toCSVFile(const QString &filename);


private:

    // States used for the parser.
    typedef enum {PS_NAME_OR_COMMENT,PS_NAME, PS_FIELD, PS_END_COMMENT} ParseState;

    // The defined delimiters
    Delimiters delimiters;

    // The error message
    QString error;

    // The actual data
    QVariantHash data;

    // Verification structure and functions.
    CommandVerifications verif;
    bool verifyParsedCommands();
    QString checkValidtyQVariant(const QVariant &data, const QString &keyword, ValueType type);
};

#endif // CONFIGURATIONMANAGER_H
