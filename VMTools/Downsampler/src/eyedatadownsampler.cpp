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

    QIntValidator *validator = new QIntValidator(5,100,ui->leFreqStep);
    ui->leFreqStep->setValidator(validator);

    validator = new QIntValidator(10,100,ui->leStartFrequency);
    ui->leStartFrequency->setValidator(validator);

    validator = new QIntValidator(100,500,ui->leMaxMaxDisp);
    ui->leMaxMaxDisp->setValidator(validator);

    validator = new QIntValidator(10,100,ui->leMinMaxDisp);
    ui->leMinMaxDisp->setValidator(validator);

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

    //    if (mdSweeper.getError().isEmpty()){
    //        //qDebug() << "Sweep results " << mdSweeper.getResults();
    //        QHash<qint32, qint32> res = mdSweeper.getResults();
    //        QList<qint32> keys = res.keys();
    //        qint32 maxValue = res.value(keys.first());
    //        qint32 optimumWindow = keys.first();
    //        for (qint32 i = 1; i < keys.size(); i++){
    //            if (res.value(keys.at(i)) > maxValue){
    //                maxValue = res.value(keys.at(i));
    //                optimumWindow = keys.at(i);
    //            }
    //        }
    //        qDebug() << "Sweep Results. Optimum Value is" << optimumWindow << "which gives" << maxValue << "fixations" ;
    //    }
    //    else
    //        qDebug() << "Error" << mdSweeper.getError();
}


//void EyeDataDownSampler::on_pbMDSweep_clicked()
//{
//    //QString filePath = "C:/Users/Viewmind/Documents/viewmind_projects/VMTools/EyeDataAnalyzer/exe64/work/1369462188/0000000311/2019_06_13_22_22_53/reading_es_2_2019_06_13_19_21.dat";
//    QString filePath = "C:/Users/Viewmind/Documents/viewmind_projects/VMTools/EyeDataAnalyzer/exe64/work/1369462188/0000000311/2019_06_13_22_22_53/downsampled/reading_es_2_2019_06_13_19_21.dat";
//    //QString filePath = "C:/Users/Viewmind/Documents/viewmind_projects/VMTools/EyeDataAnalyzer/exe64/work/1369462188/0000000311/2019_06_13_22_22_53/downsampled-90/reading_es_2_2019_06_13_19_21.dat";
//    //QString filePath = "C:/Users/Viewmind/Documents/viewmind_projects/VMTools/EyeDataAnalyzer/exe64/work/1369462188/0000000311/2019_06_13_22_22_53/downsampled-120/reading_es_2_2019_06_13_19_21.dat";
//    mdSweeper.setSweepParameters(filePath,50,150);
//    mdSweeper.start();
//}

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

    sweeper.setSweepParameters(sweepParams);
    sweeper.start();
}
