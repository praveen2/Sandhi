#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QObject>
#include <QtGui>
#include "ui_encode.h"
#include "ui_decode.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Ui_encode encodeForm;
    Ui_decode decodeForm;
    QDialog *encodeDialog,*decodeDialog;
    QString first_output;

private slots:
    void on_decrypt_clicked();
    void on_encrypt_clicked();
    void on_sec_encode_clicked();
    void on_primary_encode_clicked();
    void on_sec_decode_clicked();
    void on_primary_decode_clicked();
};

#endif // MAINWINDOW_H
