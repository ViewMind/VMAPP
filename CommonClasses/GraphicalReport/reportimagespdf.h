#ifndef REPORTIMAGESPDF_H
#define REPORTIMAGESPDF_H

#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QMetaEnum>

/*****************************************************************
 * Attempts to use the convert command of imagemagick
 * to create a PDF out of all the images in the input directory
 * **************************************************************/

#ifdef Q_OS_WIN
#define  PDF_GEN_APPLICATION    "magick"
#else
#define  PDF_GEN_APPLICATION    "convert"
#endif

class ReportImagesPDF
{
public:
    ReportImagesPDF();

    bool createPDF(const QString &inputDirectory, QString outputPDF);

    QString getStdOutput() const;
    QString getStdError() const;
    QString getError() const;
    QString getCmd() const;

private:
    QString stdOut;
    QString stdErr;
    QString error;
    QString cmd;

};

#endif // REPORTIMAGESPDF_H
