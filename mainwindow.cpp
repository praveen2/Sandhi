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
    int i , flag = 0 ,cur_pos = 0, word_present=0;
    QString stringToEncode = ui->stringToEncode->text();
    if(stringToEncode.isNull())
        return;

    QFile *file = new QFile("DB");
    QTextCodec *hindi = QTextCodec::codecForName("UTF-8");
    file->open(QIODevice::ReadWrite);

    QStringList searchStringList = ui->inputData->toPlainText().split(" ",QString::SkipEmptyParts);
    foreach (QString searchString,searchStringList )
    {
        qDebug()<<"1";
        word_present = 0;
        file->seek(0);
        //If Cover Text is bigger than what is needed
        if(cur_pos >= stringToEncode.size())
            return;

        i=0 , flag=0;
        while(++i)      //Note that it is infinite loop
        {

            QString fileString = file->readLine();
            if(fileString.isEmpty())
                break;
            fileString.chop(2);
            QStringList sandhiViched = fileString.split(" ",QString::SkipEmptyParts);
            //qDebug()<<sandhiViched.at(0)<<"   "<<searchString.toUtf8();

            if(sandhiViched.at(0) == searchString.toUtf8())
            {
                //Word is composite and present in database
                qDebug()<<"true"<<i;
                word_present=1;

                if(stringToEncode[cur_pos++] == '1')
                {
                    for(int i=1 ; i<sandhiViched.size() ; i++)
                        ui->outputData->insertHtml(hindi->toUnicode("<span style=\"text-decoration: underline;\">"+sandhiViched.at(i).toAscii())+" </span>");
                    //We have outputted the viched of composite word so flag this word
                    flag = 1;
                    break;
                }
            }
            if(flag)
                break;
        }
        if(!flag)
        {
            if(word_present)
                ui->outputData->insertHtml("<span style=\"font-weight: bold;\">" + searchString +" </span>");
            else
                ui->outputData->insertHtml(searchString +" ");
        }
    }
}
