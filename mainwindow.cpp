#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::on_submit_clicked()
{
    int i = 0 , flag = 0;
    QFile *file = new QFile("DB");
    QTextCodec *hindi = QTextCodec::codecForName("UTF-8");
    file->open(QIODevice::ReadWrite);

    QString searchString = ui->inputData->text().toUtf8();

    while(++i)      //Not that it is infinite loop
    {
        QString fileString = file->readLine();
        if(fileString.isEmpty())
            break;
        fileString.chop(2);
        qDebug()<<fileString<<"   "<<searchString;

        if(fileString == searchString)
        {
            qDebug()<<"true";
            flag = 1;
            QMessageBox::information(this,tr("Results").toUtf8(),tr("Found at %1").arg(i));
            break;
        }
    }
    if(!flag)
        QMessageBox::information(this,tr("Results"),tr("Not Found"));
}
