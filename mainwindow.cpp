#include "mainwindow.h"
#include "ui_mainwindow.h"


/******************************************************
 ** Main Constructor. It setup the main windows' UI  **
 ** and encrypt and decrypt forms as well. It will   **
 ** then setup four connections of  four buttons in  **
 ** the encrypt and decrypt form to their slots.     **
 ******************************************************/
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

    initializeCharacterListHavingDanda();
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

/********************************************************
 ** This function initialize a list which contains all **
 ** the characters who have 'danda' in their glyph.    **
 ********************************************************/
void MainWindow::initializeCharacterListHavingDanda()
{
    QFile *file = new QFile("list.txt");
    file->open(QIODevice::ReadOnly);
    QString fileString = file->readLine();
    while(!fileString.isEmpty())
    {
        characterListWithDanda.append(fileString.toInt());
        fileString = file->readLine();
    }
}


/**************************************************************************************
 ** It is a slot and instantiated on pressing primary encoding button. This function **
 ** will read the cover text one by one word and check whether the word is present   **
 ** in dictionary or not. If the word is present in the dictionary then if the bit to**
 ** encode is "1" then sandhi viched is substituted, else the word is left as it is. **
 **************************************************************************************/
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
        {
            qDebug()<<"Cover Text is bigger, so clipping the cover text.";
            return;
        }

        i=0 , flag=0;
        while(++i)      //Note that it is infinite loop
        {

            QString fileString = file->readLine();
            if(fileString.isEmpty())
                break;
            fileString.chop(2);

            QStringList sandhiViched = fileString.split(" ",QString::SkipEmptyParts);

            if(sandhiViched.at(0) == searchString.toUtf8())
            {
                //Word is composite and present in database
                word_present=1;

                if(stringToEncode[cur_pos++] == '1')
                {
                    for(int i=1 ; i<sandhiViched.size() ; i++)
                    {
                        first_output += ( hindi->toUnicode(sandhiViched.at(i).toAscii() + " " ));
                        encodeForm.outputData->insertHtml(hindi->toUnicode \
                          ("<span style=\"text-decoration: underline;\">"+sandhiViched.at(i).toAscii())+" </span>");
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
    if(cur_pos < stringToEncode.size())
    {
        qDebug()<<"Cover Text is insufficient for encoding";
        qDebug()<<"Encoded"<<stringToEncode.left(cur_pos);
    }
}

/************************************************************************************
 ** It is also a slot and instantiated on pressing secondary encoding button. This **
 ** function will read the previous encoded text character by character and check  **
 ** whether the character is present in the list character list with danda. If the **
 ** character is present in the list and the bit to encode is "1" then danda in    **
 ** alphabet is cut using the slant font else character is left as it is .         **
 ************************************************************************************/
void MainWindow::on_sec_encode_clicked()
{
    int cur_pos = 0;
    for(int i=0 ; i<first_output.length()&&cur_pos < encodeForm.secData->text().length() ; i++)
    {
        //qDebug()<<first_output.at(i).unicode();
        if(first_output.at(i).unicode() == 32)
            encodeForm.finalOutput->insertHtml("&nbsp");

        else if(characterListWithDanda.contains \
              (first_output.at(i).unicode()) && encodeForm.secData->text().at(cur_pos++) == '1')

            encodeForm.finalOutput->insertHtml \
                 ("<span style=\"font-family:slant; font-size:24pt;\">" + QString(first_output.at(i)) + "</span>");

        else
            encodeForm.finalOutput->insertHtml \
                 ("<span style=\"font-family:Mangal; font-size:24pt;\">" + QString(first_output.at(i)) + "</span>");
    }
}


/*************************************************************************************
 ** This function does the reverse of on_sec_encode_clicked. This function find out **
 ** whether the character is displayed in slant font or normal font. This function  **
 ** and on_sec_encode_clicked performs the simulation of actually cutting the pixels**
 ** from the 'danda' and then on reciever side finding out which alphabet has its   **
 ** danda cut slant wise at bottom using image processing in MATLAB                 **
 *************************************************************************************/
void MainWindow::on_sec_decode_clicked()
{
    int pos = 0, index;
    QString encodedString = encodeForm.finalOutput->toHtml();
    QString substring = "span style";
    qDebug()<<encodedString;
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
                decodeForm.intermediateOutputData->insertPlainText(encodedString.at(index + 52 + ctr));
                //if the character has 'danda' then only output as '0' else character is unencodable
                if(characterListWithDanda.contains(encodedString.at(index + 52 + ctr++).unicode()))
                    decodeForm.secDecodedString->insert("0");
            }
        }

        pos = index + 1;
    }

    //decodeForm.intermediateOutputData->insertPlainText(encodedString);
}


/***************************************************************************
 ** This function performs all operations opposite to that of on_primary_ **
 ** encode_clicked(). It will check word by word whether it is present in **
 ** dictionary in composite word fields, if it is then it indicates that  **
 ** the word has been encoded with bit "0". Else it will check again the  **
 ** dictionary if the word and next consecutive word(s) are part of a     **
 ** sandhi viched. If yes then the bit encoded is "1" and the funtion will**
 ** output the composite word in cover text box and "1" as secret data    **
 ***************************************************************************/
void MainWindow::on_primary_decode_clicked()
{
    int i , flagToBreak , flagToContinue , tempIndex;

    QFile *file = new QFile("DB");
    QTextCodec *hindi = QTextCodec::codecForName("UTF-8");
    file->open(QIODevice::ReadWrite);

    QStringList searchStringList = decodeForm.intermediateOutputData->toPlainText().split(" ",QString::SkipEmptyParts);

    for(int index=0 ; index<searchStringList.length() ; index++ )
    {
        QString searchString = searchStringList.at(index);

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
                        //the word is not part of currently selected sandhi
                        //restore the value of index(which is index of main intermediate output text)
                        index = tempIndex;
                        //Flag to indicate that more search is needed as many composite words
                        //can have same first atomic word.
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


/*************************************************************
 ** Initiates the encoding process and displays encode form **
 *************************************************************/
void MainWindow::on_encrypt_clicked()
{
    encodeDialog->show();
}

/*************************************************************
 ** Initiates the decoding process and displays decode form **
 *************************************************************/
void MainWindow::on_decrypt_clicked()
{
    decodeForm.encodedData->insertHtml(encodeForm.finalOutput->toHtml());
    decodeDialog->show();
}
