#include "control.h"

Control::Control(QObject *parent):QObject(parent)
{
    connect(&eyeServer,&QProcess::errorOccurred,this,&Control::onErrorOcurred);
    connect(&eyeServer, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    connect(&eyeServer,&QProcess::readyReadStandardError,this,&Control::onReadyReadStandardError);
    connect(&eyeServer,&QProcess::readyReadStandardOutput,this,&Control::onReadyReadStandardOutput);
    logger.setLogFileLocation("eyeupdatemonitor.log");

    bool shouldExit = false;
    configuration = LoadLocalConfiguration(&logger,&shouldExit);
    if (shouldExit){
        exit(0);
    }

    if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
        logger.appendStandard("Configured for Production");
    }
    else{
        logger.appendStandard("Configured for Local Host");
    }


    sendingMail = false;
}


void Control::launchServer(){
    logger.appendStandard("Starting server");
    eyeServer.setProgram("./EyeUpdateServer");
    eyeServer.start();
}

void Control::onErrorOcurred(QProcess::ProcessError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    QString errorString = "There was a process error:  ";
    errorString = errorString + metaEnum.valueToKey(error);
    logger.appendError(errorString);
    sendMail(errorString);
}

void Control::onReadyReadStandardError(){
    QString output(eyeServer.readAllStandardError());
    std::cout << output.toStdString() << std::endl;
}

void Control::onReadyReadStandardOutput(){
    QString output(eyeServer.readAllStandardOutput());
    std::cout << output.toStdString() << std::endl;
}

void Control::onFinished(int exitCode){
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
    QString errorString = "The EyeUpdateServer Exited with code: " + QString::number(exitCode) + " and an exit status " + metaEnum.valueToKey(eyeServer.exitStatus());
    logger.appendError(errorString);
    sendMail(errorString);
    emit(exitRequested());
}


void Control::sendMail(const QString &err){
    if (sendingMail){
        logger.appendError("Attempting to send email when last attemp is not finished");
        return;
    }
    sendingMail = true;
    QString outPHPFilename = "mail.php";
    QFile phpFile(outPHPFilename);
    if (!phpFile.open(QFile::WriteOnly)){
        logger.appendError("Could not open the mail file for writing");
        return;
    }
    QTextStream writer(&phpFile);

    writer << "<?php\n";
    if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
        writer << "require '/home/ec2-user/composer/vendor/autoload.php';\n";
        writer << "require '/home/ec2-user/composer/vendor/phpmailer/phpmailer/PHPMailerAutoload.php';\n";
    }
    else{
        writer << "use PHPMailer\\PHPMailer\\PHPMailer;\n";
        writer << "require '/home/ariela/repos/viewmind_projects/Scripts/php/vendor/autoload.php';\n";
    }

    writer << "$mail = new PHPMailer;\n";
    writer << "$mail->isSMTP();\n";
    writer << "$mail->setFrom('check.viewmind@gmail.com', 'ViewMind Administration');\n";
    writer << "$mail->Username = 'AKIARDLQA5AHRSTLPCYS';\n";
    writer << "$mail->Password = 'BHGWozyNwZoHjvUAhL8d7H9FC/H4RBNfh564MnKZRKj/';\n";
    writer << "$mail->Host = 'email-smtp.us-east-1.amazonaws.com';\n";
    writer << "$mail->Subject = 'ViewMind EyeUpdateServer Stopped Working';\n";
    writer << "$mail->addAddress('ariel.arelovich@viewmind.com.ar', 'Ariel Arelovich');\n";
    if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
        writer << "$mail->addAddress('matias.shulz@viewmind.com.ar', 'Matias Shulz');\n";
    }
    // The HTML-formatted body of the email
    writer << "$mail->Body = '" + err + "';\n";
    writer << "$mail->SMTPAuth = true;\n";
    writer << "$mail->SMTPSecure = 'tls';\n";
    writer << "$mail->Port = 587;\n";
    writer << "$mail->isHTML(true);\n";
    writer << "if(!$mail->send()) {\n";
    writer << "    echo 'Email error is: ' , $mail->ErrorInfo , PHP_EOL;\n";
    writer << "}\n";
    writer << "?>\n";

    // Actually sending the email.
    phpFile.close();
    QProcess p;
    p.setProgram("php");
    QStringList args; args << outPHPFilename;
    p.setArguments(args);
    p.start();
    p.waitForFinished();
    QString output(p.readAllStandardOutput());
    if (!output.trimmed().isEmpty()){
        logger.appendError("EMAIL: " + output.trimmed());
    }
    sendingMail = false;
}
