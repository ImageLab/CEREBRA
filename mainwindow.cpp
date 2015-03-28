#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

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

void MainWindow::loadButtonClicked(){

//  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                     "",
//                                                     tr("Files (*.*)"));
    QString directoryName = QFileDialog::getExistingDirectory();

    if( !directoryName.isNull())
        ui->packetRendererGLWidget->setPacket(reader.readPacketFromDirectory( directoryName), directoryName);
}
