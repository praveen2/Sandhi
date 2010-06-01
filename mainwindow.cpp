#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encodeDialog = new QDialog(this);
    encodeForm.setupUi(encodeDialog);

    decodeDialog = new QDialog(this);
    decodeForm.setupUi(decodeDialog);

    connect(encodeForm.primary_encode_button,SIGNAL(clicked()),this,SLOT(on_primary_encode_clicked()));
    connect(encodeForm.sec_encode_button,SIGNAL(clicked()),this,SLOT(on_sec_encode_clicked()));
    connect(decodeForm.primary_decode_button,SIGNAL(clicked()),this,SLOT(on_primary_decode_clicked()));
    connect(decodeForm.sec_decode_button,SIGNAL(clicked()),this,SLOT(on_sec_decode_clicked()));
    encodeForm.stringToEncode->insert("1100010");
    encodeForm.secData->insert("001001010101010000000000001111111111111111111111111000000000000000000001111111111111111111111010101010010101010101000000000000001111111111111111111111111000000000000000000000000000000000111111111111111111111111111111111111111");
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


void MainWindow::on_primary_encode_clicked()
{
    int i , flag = 0 ,cur_pos = 0, word_present=0;
    QString stringToEncode = encodeForm.stringToEncode->text();
    if(stringToEncode.isNull())
        return;

    QFile *file = new QFile("DB");
    QTextCodec *hindi = QTextCodec::codecForName("UTF-8");
    file->open(QIODevice::ReadWrite);

    QStringList searchStringList = encodeForm.inputData->toPlainText().split(" ",QString::SkipEmptyParts);
    foreach (QString searchString,searchStringList )
    {
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
                    {
                        first_output += ( hindi->toUnicode(sandhiViched.at(i).toAscii() + " " ));
                        encodeForm.outputData->insertHtml(hindi->toUnicode("<span style=\"text-decoration: underline;\">"+sandhiViched.at(i).toAscii())+" </span>");
                    }
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
            first_output += (searchString +" ");
            if(word_present)
                encodeForm.outputData->insertHtml("<span style=\"font-weight: bold;\">" + searchString +" </span>");
            else
                encodeForm.outputData->insertHtml(searchString +" ");
        }
    }
}

void MainWindow::on_sec_encode_clicked()
{
    int cur_pos = 0;
    for(int i=0 ; i<first_output.length()&&cur_pos < encodeForm.secData->text().length() ; i++)
    {
        //qDebug()<<first_output.at(i).unicode();
        if(first_output.at(i).unicode() == 32)
            encodeForm.finalOutput->insertHtml("&nbsp");
        else if(encodeForm.secData->text().at(cur_pos++) == '1')
            encodeForm.finalOutput->insertHtml("<span style=\"font-family:slant; font-size:10pt;\">" + QString(first_output.at(i)) + "</span>");
        else
            encodeForm.finalOutput->insertHtml("<span style=\"font-family:Mangal; font-size:10pt;\">" + QString(first_output.at(i)) + "</span>");
    }
}

void MainWindow::on_sec_decode_clicked()
{
    int pos = 0, index;
    QString encodedString = encodeForm.finalOutput->toHtml();
    QString substring = "span style";
    //qDebug()<<encodedString;
    while(pos < encodedString.size())
    {
        index = encodedString.indexOf(substring,pos);
        //qDebug()<<index<<encodedString.at(index-2);

        if(index == -1)
            break;

        //qDebug()<<encodedString.at(index-2).unicode();

        if(encodedString.at(index-2).unicode() == 160)
            decodeForm.intermediateOutputData->insertPlainText(" ");

        if(encodedString.at(index+26) == 's')
        {
            int ctr = 0;
            while(encodedString.at(index + 51 + ctr) != '<')
            {
                decodeForm.intermediateOutputData->insertPlainText(encodedString.at(index + 51 + ctr++));
                decodeForm.secDecodedString->insert("1");
            }
        }
        else if(encodedString.at(index+26) == 'M')
        {
            int ctr = 0;
            while(encodedString.at(index + 52 + ctr) != '<')
            {
                decodeForm.intermediateOutputData->insertPlainText(encodedString.at(index + 52 + ctr++));
                decodeForm.secDecodedString->insert("0");
            }
        }

        pos = index + 1;
    }

    //decodeForm.intermediateOutputData->insertPlainText(encodedString);
}

void MainWindow::on_primary_decode_clicked()
{
    int i , flag = 0 , flagToBreak , flagToContinue , cur_pos = 0, word_present=0, tempIndex;

    QFile *file = new QFile("DB");
    QTextCodec *hindi = QTextCodec::codecForName("UTF-8");
    file->open(QIODevice::ReadWrite);

    QStringList searchStringList = decodeForm.intermediateOutputData->toPlainText().split(" ",QString::SkipEmptyParts);

    for(int index=0 ; index<searchStringList.length() ; index++ )
    {
        QString searchString = searchStringList.at(index);

        word_present = 0;
        file->seek(0);
        i=0; flagToBreak = 0;

        while(++i)      //Note that it is infinite loop
        {
            QString fileString = file->readLine();
            flagToContinue = 0;
            if(fileString.isEmpty())
                break;
            fileString.chop(2);
            QStringList sandhiViched = fileString.split(" ",QString::SkipEmptyParts);
            //qDebug()<<sandhiViched.at(0)<<"   "<<searchString.toUtf8();

            if(sandhiViched.at(0) == searchString.toUtf8())
            {
                //Word is composite and present in database and so bit encoded is 0
                //qDebug()<<"true"<<i;
                decodeForm.primaryDecodedString->insert("0");
                decodeForm.coverText->insertPlainText(searchString + " ");
                flagToBreak = 1;
            }

            else if(sandhiViched.at(1) == searchString.toUtf8())
            {
                //testing if this word is part of sandhi viched by matching next words
                tempIndex = index;
                for(int ctr=2; ctr<sandhiViched.length(); ctr++)
                {
                    if(sandhiViched[ctr] != searchStringList[++index].toUtf8())
                    {
//                        //the word is not part of any sandhi viched so write all the currently matched parts as it is to cover text
//                        for(int ctr2=1; ctr2 < ctr; ctr2++)
//                            decodeForm.coverText->insertPlainText(hindi->toUnicode(sandhiViched.at(ctr2).toAscii() + " "));
                        //restore the value of index(which is index of main intermediate output text)
                        index = tempIndex;    //1 because ctr is already ahead by 1(composite word at 0th location)
                        flagToContinue = 1;
                    }
                }

                if(flagToContinue)
                    continue;

                //if all words matched then this code will be executed
                //we have found the composition of a sandhi so output the original composite word and decode bit as 1
                decodeForm.primaryDecodedString->insert("1");
                decodeForm.coverText->insertPlainText(hindi->toUnicode(sandhiViched.at(0).toAscii() + " " ));
                flagToBreak = 1;
            }
            if(flagToBreak == 1)
                break;
        }
        if(flagToBreak == 0)
        {
            //its a normal word
            decodeForm.coverText->insertPlainText(searchString + " ");
        }
    }
}

void MainWindow::on_encrypt_clicked()
{
    encodeDialog->show();
}

void MainWindow::on_decrypt_clicked()
{
    decodeForm.encodedData->insertHtml(encodeForm.finalOutput->toHtml());
    decodeDialog->show();
}
