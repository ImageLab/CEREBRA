#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "packetfilereader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString fileName;
    QString directory;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PacketFileReader reader;

public slots:
    void loadButtonClicked();
    void startButtonClicked();
};

#endif // MAINWINDOW_H
