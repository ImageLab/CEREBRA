#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cmath>
#include "mat.h"
#include "matrix.h"

#pragma comment(lib, "lib/libmx.lib")
#pragma comment(lib, "lib/libeng.lib")
#pragma comment(lib, "lib/libmat.lib")

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

    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.mat*)"));
    if(fileName.length()== 0)
        return;
    QDir d = QFileInfo(fileName).absoluteDir();
    directory = d.absolutePath();

    MATFile *mFile = NULL;
    mFile = matOpen(fileName.toStdString().c_str(),"r");
    if(mFile == NULL)
        cout << "error opening MAT file: " << endl;

    const mxArray *pa = NULL;
    int ndir;
    const char **variables;

    variables = (const char **)matGetDir(mFile, &ndir);
    if (variables == NULL)
      printf("Error reading directory of file");


    for(int k=0; k < ndir ; k++)
         ui->comboBox->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_2->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_3->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_4->addItem(QString(variables[k]));



}

void MainWindow::startButtonClicked()
{
    if( !fileName.isNull())
        ui->packetRendererGLWidget->setPacket(reader.readPacketFromDirectory( fileName), directory);
}
