#ifndef REPORTIMAGESPDF_H
#define REPORTIMAGESPDF_H

#include <QProcess>
#include <QDir>
#include <QFileInfo>

/*****************************************************************
 * Attempts to use the convert command of imagemagick
 * to create a PDF out of all the images in the input directory
 * **************************************************************/

#define  PDF_GEN_APPLICATION    "convert"

class ReportImagesPDF
{
public:
    ReportImagesPDF();

    bool createPDF(const QString &inputDirectory, QString outputPDF);

    QString getStdOutput() const;
    QString getStdError() const;
    QString getError() const;

private:
    QString stdOut;
    QString stdErr;
    QString error;

};

#endif // REPORTIMAGESPDF_H
