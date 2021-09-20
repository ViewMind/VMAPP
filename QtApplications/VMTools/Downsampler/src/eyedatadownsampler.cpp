#include "eyedatadownsampler.h"
#include "ui_eyedatadownsampler.h"

EyeDataDownSampler::EyeDataDownSampler(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EyeDataDownSampler)
{
    ui->setupUi(this);
    QString filePath = "C:/Users/Viewmind/Documents/viewmind_projects/VMTools/EyeDataAnalyzer/exe64/work/1369462188/0000000311/2019_06_13_22_22_53/reading_es_2_2019_06_13_19_21.dat";
    setAcceptDrops(true);
    connect(&sweeper,SIGNAL(finished()),this,SLOT(on_mdSweeper_finished()));
    connect(&sweeper,SIGNAL(updateProgress(qint32)),ui->pbarProgressBar,SLOT(setValue(qint32)));
    connect(&ascExtractor,SIGNAL(updateProgress(qint32)),ui->pbarProgressBar,SLOT(setValue(qint32)));
    connect(&ascExtractor,SIGNAL(finished()),this,SLOT(on_ascExtractor_finished()));

    QIntValidator *validator = new QIntValidator(5,100,ui->leFreqStep);
    ui->leFreqStep->setValidator(validator);

    validator = new QIntValidator(10,100,ui->leStartFrequency);
    ui->leStartFrequency->setValidator(validator);

    validator = new QIntValidator(100,500,ui->leMaxMaxDisp);
    ui->leMaxMaxDisp->setValidator(validator);

    validator = new QIntValidator(10,100,ui->leMinMaxDisp);
    ui->leMinMaxDisp->setValidator(validator);

    validator = new QIntValidator(1,10000,ui->leResolutionScaling);
    ui->leResolutionScaling->setValidator(validator);

}

EyeDataDownSampler::~EyeDataDownSampler()
{
    delete ui;
}

void EyeDataDownSampler::on_mdSweeper_finished(){
    if (sweeper.getError().isEmpty()){
        QMessageBox::information(this,"Sweep Done","Finished with no errors",QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this,"Sweep Failed",sweeper.getError(),QMessageBox::Ok);
    }

}

void EyeDataDownSampler::on_ascExtractor_finished(){
    if (ascExtractor.getError().isEmpty()){
        QMessageBox::information(this,"ASC Sweep Done","Finished with no errors",QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this,"ASC Sweep Failed",ascExtractor.getError(),QMessageBox::Ok);
    }
}


void EyeDataDownSampler::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void EyeDataDownSampler::dropEvent(QDropEvent *event){
    if (event->mimeData()->hasUrls()){
        // Using only the first one
        sweepParams.fileName = event->mimeData()->urls().first().toLocalFile();
        ui->labFileToProcess->setText(sweepParams.fileName);
        //        QList<QUrl> urls = event->mimeData()->urls();
        //        for (qint32 i = 0; i < urls.size(); i++){
        //            qDebug() << urls.at(i).toLocalFile();
        //        }
    }
    event->acceptProposedAction();
}



void EyeDataDownSampler::on_pbMDAndFrequencySweep_clicked()
{
    sweepParams.frequencyStep           = ui->leFreqStep->text().toInt();
    sweepParams.startFrequency          = ui->leStartFrequency->text().toInt();
    sweepParams.startMaxDispersionValue = ui->leMinMaxDisp->text().toInt();
    sweepParams.endMaxDispersionValue   = ui->leMaxMaxDisp->text().toInt();
    sweepParams.resolutionScaling       = ui->leResolutionScaling->text().toInt();

    sweeper.setSweepParameters(sweepParams);
    sweeper.start();
}

void EyeDataDownSampler::on_actionTester_Action_triggered()
{

    QString file = "C:/Users/Viewmind/Documents/OtherStuff/EyeLink/Paolini.asc";

    EyeTribeASCExtractor::MDSweepParameters params;

    // Normalized setup
    params.startMD = 5;
    params.endMD   = 300;
    params.fileName = file;
    params.resX = 1280;
    params.resY = 800;
    params.resScale = 1000;

    // NON Normalized Setup
    //    params.startMD = 1;
    //    params.endMD   = 200;
    //    params.fileName = file;
    //    params.resX = 0;
    //    params.resY = 0;
    //    params.resScale = 1;

    ascExtractor.setupSweep(params);

    //    ascExtractor.start();

    //  Function that generates the CSV witht the fixations found in the ASC file.
    //if (!ascExtractor.generateFixationFileFromASC(file,0,0)){            // NON Normalized Setup
    if (!ascExtractor.generateFixationFileFromASC(file,1280,800)){       // Normalized setup.
        QMessageBox::information(this,"ASC Processing",ascExtractor.getError(),QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this,"ASC Processing","Finished",QMessageBox::Ok);
    }
}

void EyeDataDownSampler::on_actionASC_CSV_Match_Up_triggered()
{
    QString file = "C:/Users/Viewmind/Documents/OtherStuff/EyeLink/Paolini.asc";
    if (!ascExtractor.genrateCSVFixFileBySentences(file,108)){
        QMessageBox::information(this,"ASC Processing",ascExtractor.getError(),QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this,"ASC Processing","Finished",QMessageBox::Ok);
    }

}
