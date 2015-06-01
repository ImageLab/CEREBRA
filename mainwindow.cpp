#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <cmath>
#include "mat.h"
#include "matrix.h"

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

    QString directoryName = QFileDialog::getExistingDirectory();

    if( !directoryName.isNull()){
        ui->displayButton->setEnabled(false);
        ui->packetRendererGLWidget->setPacket( reader.readPacketFromDirectory( directoryName), directoryName);
    }
}

void MainWindow::loadMatFileButtonClicked(){

    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.mat*)"));
    if(fileName.length()== 0)
        return;

    ui->displayButton->setEnabled(true);

    QDir d = QFileInfo(fileName).absoluteDir();
    directory = d.absolutePath();

    MATFile *mFile = NULL;
    mFile = matOpen(fileName.toStdString().c_str(),"r");
    if(mFile == NULL){
        cout << "error opening MAT file: " << endl;
        ui->matlabTab->setEnabled(false);
        return;
    }

    int ndir;
    const char **variables;

    variables = (const char **)matGetDir(mFile, &ndir);
    if (variables == NULL)
      printf("Error reading directory of file");

    ui->comboBox->addItem("");
    ui->comboBox_2->addItem("");
    ui->comboBox_3->addItem("");
    ui->comboBox_4->addItem("");

    for(int k=0; k < ndir ; k++)
         ui->comboBox->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_2->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_3->addItem(QString(variables[k]));
    for(int k=0; k < ndir ; k++)
         ui->comboBox_4->addItem(QString(variables[k]));

    ui->matlabTab->setEnabled(true);
}

void MainWindow::displayButtonClicked()
{
    if( !fileName.isNull())
        ui->packetRendererGLWidget->setPacket(reader.readPacketFromMatlab( fileName,
                                                                           ui->comboBox->currentText(),
                                                                           ui->comboBox_2->currentText(),
                                                                           ui->comboBox_3->currentText(),
                                                                           ui->comboBox_4->currentText()), directory);
}

void MainWindow::minValueTextChanged(){


}
