#ifndef BINDINGPARSER_H
#define BINDINGPARSER_H

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include <QColor>
#include <QtMath>

/// TODO: Eliminate this when legacy parsing can be deleted.
// Strings for same and different in the CSV file.
#define   STR_DIFFERENT                                 "different"
#define   STR_SAME                                      "same"

class BindingParser
{
public:

    // Public structures
    struct BindingFlag{
        qint32 x;
        qint32 y;
        QColor back;
        QColor cross;

        // For debugging.
        QString toString() const{
            QString ans = "";
            ans = "(" + QString::number(x) + "," + QString::number(y) + ") ";
            ans = ans + "BACK: " + back.name() + " CROSS: " + cross.name();
            return ans;
        }
    };

    typedef QList<BindingFlag> BindingSlide;

    struct BindingTrial {
        QString name;
        BindingSlide show;
        BindingSlide test;
        bool isSame;

        // For debugging.
        QString toString() const {
            QString ans = name + ". ";
            ans = ans  + "SHOW -> ";
            for (qint32 i = 0; i < show.size(); i++){
                ans = ans + "TARGET " + QString::number(i) + ": " + show.at(i).toString() + "| ";
            }
            ans = ans  + "TEST -> ";
            for (qint32 i = 0; i < test.size(); i++){
                ans = ans + "TARGET " + QString::number(i) + ": " + test.at(i).toString() + "| ";
            }
            return ans;
        }

        // Used for CSV Generation.
        QString getCSVXYData(bool isTrial) const {
            QStringList ans;
            bool onlyTwo;
            if (isTrial){
                for (qint32 i = 0; i < test.size(); i++){
                    ans << QString::number(test.at(i).x);
                    ans << QString::number(test.at(i).y);
                }
                /// TODO: Change this to something less prone to error in furture changes.
                onlyTwo = (test.size() == 2);
            }
            else{
                for (qint32 i = 0; i < show.size(); i++){
                    ans << QString::number(show.at(i).x);
                    ans << QString::number(show.at(i).y);
                }
                /// TODO: Change this to something less prone to error in furture changes.
                onlyTwo = (show.size() == 2);
            }
            if (onlyTwo){
                ans << "0" << "0";
            }
            return ans.join(",");
        }

    };

    struct FlagDrawStructure {
        qreal FlagSideH;
        qreal FlagSideV;
        qreal HSBorder;
        qreal HLBorder;
        qreal VSBorder;
        qreal VLBorder;
        QList<qreal> xpos;
        QList<qreal> ypos;
    };


    // Main interface to use this class for the Binding manager or simply to get info on the experiments.
    BindingParser();
    bool parseBindingExperiment(const QString &contents, ConfigurationManager *config, qreal ScreenResolutionWidth, qreal ScreenResolutionHeight, qint32 numberToLeave);
    QString getError() const {return error;}
    FlagDrawStructure getDrawStructure() const {return drawStructure;}
    qint32 getNumberOfTargets() const {return numberOfTargets;}
    QString getVersionString() const {return versionString;}
    QList<BindingTrial> getTrials() const {return trials;}
    BindingTrial getTrialByName(const QString &id) const;
    QList<BindingTrial> getTrialList() const {return trials;}
    QList<QStringList> getExpectedIDs() const {return expectedIDs;}
    void demoModeList(qint32 numberToLeave);


private:

    // Selects the physical dimensions based on the targets.
    struct DrawValues{
        qreal side;
        qreal hs;
        qreal hl;
        qreal vs;
        qreal vl;
        qreal gx;
        qreal gy;
    };

    // Error message.
    QString error;

    // Version string
    QString versionString;

    // The trial structure
    QList<BindingTrial> trials;

    // The number of targets in each slide of the trial
    qint32 numberOfTargets;

    // Tells the system how to draw the flags.
    FlagDrawStructure drawStructure;

    // The expected IDs
    QList<QStringList> expectedIDs;

    // Aux functions for parsing.
    bool parseFlagPositions(const QString &line, BindingTrial *trial, bool show);
    bool parseColors(const QString &line, BindingTrial *trial, bool background, bool show);
    bool legacyParser(const QString &contents);
    DrawValues loadDrawStructure(bool targetsSmall);


};

#endif // BINDINGPARSER_H
