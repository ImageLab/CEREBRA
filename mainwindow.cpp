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

    ui->minValueTextField->setValidator( new QDoubleValidator(INT_MIN, INT_MAX, 3, this));
    ui->maxValueTextField->setValidator( new QDoubleValidator(INT_MIN, INT_MAX, 3, this));

    ui->thresholdSlider->setRange(0, 100);
    ui->rangeSlider->setRange(0, 100);

    setMinValue(ui->minValueTextField->text().toFloat());
    setMaxValue(ui->maxValueTextField->text().toFloat());
    setThreshold(ui->thresholdSlider->minimum());
    setRange(ui->rangeSlider->minimum());
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

void MainWindow::minValueTextEdited(QString text){

    int charAscii = text.toStdString().c_str()[text.length()-1];

    //if no text left
    if( charAscii == 4)
        setMinValue(0.0);
    else{
        float newMinValue = ui->minValueTextField->text().toFloat();
        if( newMinValue > maxValue)
            ui->minValueTextField->setText(QString::number(maxValue));

        setMinValue(ui->minValueTextField->text().toFloat());
    }

    updateThresholdSliderValue( ui->thresholdSlider->value());
}

void MainWindow::maxValueTextEdited(QString text){

    int charAscii = text.toStdString().c_str()[text.length()-1];

    //if no text left
    if( charAscii == 4)
        setMaxValue(0.0);
    else{

        float newMaxValue = ui->maxValueTextField->text().toFloat();
        if( newMaxValue < minValue)
            ui->maxValueTextField->setText(QString::number(minValue));

        setMaxValue(ui->maxValueTextField->text().toFloat());
    }

    updateThresholdSliderValue( ui->thresholdSlider->value());
}

void MainWindow::thresholdSliderValueChanged( int value){

    updateThresholdSliderValue( value);
}

void MainWindow::updateThresholdSliderValue( int value){

    setThreshold(minValue + ((float)(maxValue-minValue)*(float)value/ui->thresholdSlider->maximum()));
    ui->thresholdValue->setText(QString::number(threshold));
}

void MainWindow::rangeSliderValueChanged(int value){

    setRange(value);
    ui->rangeValuePercent->setText(QString::number(range) + "%");
}

void MainWindow::setThreshold( float threshold){

    this->threshold = threshold;
    ui->packetRendererGLWidget->setThreshold( threshold);
}

void MainWindow::setMinValue( float minValue){

    this->minValue = minValue;
    ui->packetRendererGLWidget->setMinValue( minValue);
}

void MainWindow::setMaxValue( float maxValue){

    this->maxValue = maxValue;
    ui->packetRendererGLWidget->setMaxValue( maxValue);
}

void MainWindow::setRange( float range){

    this->range = range;
    ui->packetRendererGLWidget->setRange( range);
}
