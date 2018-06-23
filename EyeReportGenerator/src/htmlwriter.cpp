#include "htmlwriter.h"

HTMLWriter::HTMLWriter()
{
    htmlData = "";
}


void HTMLWriter::appendMessage(const QString &msg, const QString &color, bool bold, bool plainHTML){
    if (!plainHTML){
        QString html = "<p><span style=\"font-family: 'Courier New'; color: " + color;
        if (bold){
            html = html + "; font-weight: bold\">";
        }
        else{
            html = html + "; font-weight: normal\">";
        }
        html = html + msg + "</span></p>";
        htmlData = htmlData + html;
    }
    else{
        htmlData + htmlData + plainHTML;
    }
}

void HTMLWriter::write(const QString &filename, const QString &title){
    QString html = "<!DOCTYPE html>\n";
    html = html + "<html>\n\n";
    html = html + "<head>\n";
    html = html + "<style>\n";
    html = html + "body: #DFDFDF;\n";
    html = html + "</style>\n";
    html = html + "</head>\n\n";
    html = html + "<body>";
    html = html + "<h3>" + title +  "</h3>";
    html = html + htmlData;
    html = html + "</body></html>";

    QFile htmlfile(filename);
    if (!htmlfile.open(QFile::WriteOnly)) return;
    QTextStream writer(&htmlfile);
    writer << html;
    htmlfile.close();
}
