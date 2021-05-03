#include "reportimagespdf.h"

ReportImagesPDF::ReportImagesPDF()
{
}

QString ReportImagesPDF::getError() const{
    return error;
}

QString ReportImagesPDF::getStdError() const{
    return stdErr;
}

QString ReportImagesPDF::getStdOutput() const{
    return stdOut;
}

bool ReportImagesPDF::createPDF(const QString &inputDirectory, QString outputPDF){

    error = "";
    this->stdErr = "";
    this->stdOut = "";

    // First we check that the input exists and is a directory.
    QFileInfo input_info(inputDirectory);

    if (input_info.exists()){
        if (!input_info.isDir()){
            error =  "Input string " +  inputDirectory + " is not a directory";
            return false;
        }
    }
    else{
        error = "Input directory " + inputDirectory + " does not exist";
        return false;
    }

    // We are only interested in png files.
    QStringList nameFilters; nameFilters << "*.png";

    QStringList images = QDir(inputDirectory).entryList(nameFilters,QDir::Files,QDir::Name);

    // Adding the full path to the image name.
    QStringList arguments;
    for (qint32 i = 0; i < images.size(); i++){
        arguments << inputDirectory + "/" + images.at(i);
    }

    if (arguments.isEmpty()){
        error = "No PNG images found in the input directory of " + inputDirectory;
        return false;
    }

    // Checking that the output is a pdf file.
    QStringList output_file_name_parts = outputPDF.split(".",QString::SkipEmptyParts);
    if (output_file_name_parts.last().toLower() != "pdf"){
        outputPDF = outputPDF + ".pdf";
    }
    arguments << outputPDF;

    QProcess pdfmerge;
    pdfmerge.start(PDF_GEN_APPLICATION,arguments);
    bool all_good = pdfmerge.waitForFinished();

    this->stdErr = QString(pdfmerge.readAllStandardError());
    this->stdOut = QString(pdfmerge.readAllStandardOutput());

    if (!all_good){
        if (pdfmerge.error() == QProcess::Timedout){
            error = "Time out was reached";
        }
        else{
            // The only way to know the error is to print the output.
            error = "PDF creationg failed";
        }
        return false;
    }
    else if (pdfmerge.exitCode() != 0){
        error = "Non zero exit code of "  + QString::number(pdfmerge.exitCode());
        return false;
    }

    return true;

}
