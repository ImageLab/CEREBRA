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
    ui->edgeMinValueTextField->setValidator( new QDoubleValidator(INT_MIN, INT_MAX, 3, this));
    ui->edgeMaxValueTextField->setValidator( new QDoubleValidator(INT_MIN, INT_MAX, 3, this));

    ui->rangeSlider->setEnabled(false);
    ui->edgeRangeSlider->setEnabled(false);

    ui->thresholdSlider->setRange(0, 100);
    ui->edgeThresholdSlider->setRange(0, 100);
    ui->rangeSlider->setRange(0, 100);
    ui->edgeRangeSlider->setRange(0, 100);

    setMinValue(true);
    setMaxValue(true);
    setThreshold(true);
    setRange(true);

    setMinValue(false);
    setMaxValue(false);
    setThreshold(false);
    setRange(false);

    displayArcsStateChanged(0);
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

    setMinValue( true);
}

void MainWindow::maxValueTextEdited(QString text){

    setMaxValue( true);
}

void MainWindow::minEdgeValueTextEdited(QString text){

    setMinValue( false);
}

void MainWindow::maxEdgeValueTextEdited(QString text){

    setMaxValue( false);
}

void MainWindow::thresholdSliderValueChanged( int value){

    setThreshold( true);
}

void MainWindow::rangeSliderValueChanged(int value){

    setRange( true);
    ui->rangeValuePercent->setText(QString::number( value) + "%");
}

void MainWindow::edgeThresholdSliderValueChanged( int value){

    setThreshold( false);
}

void MainWindow::edgeRangeSliderValueChanged(int value){

    setRange( false);
    ui->edgeRangeValuePercent->setText(QString::number( value) + "%");
}

void MainWindow::setMinValue( bool isVoxel){

    QLineEdit *maxTextField = (isVoxel)?ui->maxValueTextField:ui->edgeMaxValueTextField;
    QLineEdit *minTextField = (isVoxel)?ui->minValueTextField:ui->edgeMinValueTextField;

    float minValue = INT_MIN;

    //if no text left
    if( minTextField->text().size() == 0)
        minValue = 0.0;
    else{
        float newMinValue = minTextField->text().toFloat();
        if( newMinValue > maxTextField->text().toFloat())
            minTextField->setText(QString::number(maxTextField->text().toFloat()));

        minValue = minTextField->text().toFloat();
    }

    if( isVoxel){
        setThreshold( true);
        ui->packetRendererGLWidget->setVoxelMinValue( minValue);
    }else{
        setThreshold( false);
        ui->packetRendererGLWidget->setPairsMinValue( minValue);
    }
}

void MainWindow::setMaxValue( bool isVoxel){

    QLineEdit *maxTextField = (isVoxel)?ui->maxValueTextField:ui->edgeMaxValueTextField;
    QLineEdit *minTextField = (isVoxel)?ui->minValueTextField:ui->edgeMinValueTextField;

    float maxValue = (float)INT_MAX;

    //if no text left
    if( maxTextField->text().size() == 0)
        maxValue = 1.0;
    else{
        float newMaxValue = maxTextField->text().toFloat();
        if( newMaxValue < minTextField->text().toFloat())
            maxTextField->setText(QString::number(minTextField->text().toFloat()));

        maxValue = maxTextField->text().toFloat();
    }

    if( isVoxel){
        setThreshold( true);
        ui->packetRendererGLWidget->setVoxelMaxValue( maxValue);
    }else{
        setThreshold( false);
        ui->packetRendererGLWidget->setPairsMaxValue( maxValue);
    }
}

void MainWindow::setThreshold( bool isVoxel){

    QLineEdit *maxTextField = (isVoxel)?ui->maxValueTextField:ui->edgeMaxValueTextField;
    QLineEdit *minTextField = (isVoxel)?ui->minValueTextField:ui->edgeMinValueTextField;
    QLabel *thresholdValue = (isVoxel)?ui->thresholdValue:ui->edgeThresholdValue;
    QSlider *thresholdSlider = (isVoxel)?ui->thresholdSlider:ui->edgeThresholdSlider;
    QSlider *rangeSlider = (isVoxel)?ui->rangeSlider:ui->edgeRangeSlider;
    QCheckBox *setRangeCheckBox = (isVoxel)?ui->setRangeCheckBox:ui->edgeSetRangeCheckBox;
    QLabel *minThresholdLabel = (isVoxel)?ui->minThresholdLabel:ui->edgeMinThresholdLabel;
    QLabel *maxThresholdLabel = (isVoxel)?ui->maxThresholdLabel:ui->edgeMaxThresholdLabel;

    float maxValue = maxTextField->text().toFloat();
    float minValue = minTextField->text().toFloat();
    float threshold = minValue + ((float)(maxValue-minValue)*(float)thresholdSlider->value()/thresholdSlider->maximum());
    float gap = ((float)(maxValue - minValue)/(float)2)*((float)rangeSlider->value()/(float)100);
    float minThreshold = (setRangeCheckBox->isChecked() && threshold - gap >= thresholdSlider->minimum())?(threshold-gap):thresholdSlider->minimum();
    float maxThreshold = (setRangeCheckBox->isChecked())?(threshold + gap > maxValue)?maxValue:(threshold+gap):threshold;

    if( isVoxel)
        ui->packetRendererGLWidget->setThresholdRange( minThreshold, maxThreshold);
    else
        ui->packetRendererGLWidget->setPairsThresholdRange( minThreshold, maxThreshold);

    minThresholdLabel->setText("Min. Value: " + QString::number(minThreshold));
    maxThresholdLabel->setText("Max. Value: " + QString::number(maxThreshold));
    thresholdValue->setText(QString::number(threshold));
}

void MainWindow::setRange( bool isVoxel){

    QLineEdit *maxTextField = (isVoxel)?ui->maxValueTextField:ui->edgeMaxValueTextField;
    QLineEdit *minTextField = (isVoxel)?ui->minValueTextField:ui->edgeMinValueTextField;
    QSlider *rangeSlider = (isVoxel)?ui->rangeSlider:ui->edgeRangeSlider;
    QSlider *thresholdSlider = (isVoxel)?ui->thresholdSlider:ui->edgeThresholdSlider;
    QLabel *minThresholdLabel = (isVoxel)?ui->minThresholdLabel:ui->edgeMinThresholdLabel;
    QLabel *maxThresholdLabel = (isVoxel)?ui->maxThresholdLabel:ui->edgeMaxThresholdLabel;

    float maxValue = maxTextField->text().toFloat();
    float minValue = minTextField->text().toFloat();
    float threshold = minValue + ((float)(maxValue-minValue)*(float)thresholdSlider->value()/thresholdSlider->maximum());
    float gap = ((float)(maxValue - minValue)/(float)2)*((float)rangeSlider->value()/(float)100);
    float minThreshold = (threshold - gap < 0)?0:(threshold-gap);
    float maxThreshold = (threshold + gap > maxValue)?maxValue:(threshold+gap);

    if( isVoxel)
        ui->packetRendererGLWidget->setThresholdRange( minThreshold, maxThreshold);
    else
        ui->packetRendererGLWidget->setPairsThresholdRange( minThreshold, threshold);

    minThresholdLabel->setText("Min. Value: " + QString::number(minThreshold));
    maxThresholdLabel->setText("Max. Value: " + QString::number(maxThreshold));
}

void MainWindow::setRangeStateChanged( int state){

    if( state == 0)
        ui->rangeSlider->setEnabled(false);
    else
        ui->rangeSlider->setEnabled(true);

    setThreshold( true);
}

void MainWindow::edgeSetRangeStateChanged( int state){

    if( state == 0)
        ui->edgeRangeSlider->setEnabled(false);
    else
        ui->edgeRangeSlider->setEnabled(true);

    setThreshold( false);
}

void MainWindow::displayArcsStateChanged( int state){

    if( state == 0){

        ui->edgeMinMaxValuesWidget->setEnabled(false);
        ui->edgeThresholdSlider->setEnabled(false);
        ui->edgeRangeSlider->setEnabled(false);
        ui->edgeSetRangeCheckBox->setEnabled(false);
        ui->packetRendererGLWidget->shouldDisplayArcs( false);
    }else{

        ui->edgeMinMaxValuesWidget->setEnabled(true);
        ui->edgeThresholdSlider->setEnabled(true);
        ui->edgeSetRangeCheckBox->setEnabled(true);
        if( ui->edgeSetRangeCheckBox->isChecked())
            ui->edgeRangeSlider->setEnabled(true);
        else
            ui->edgeRangeSlider->setEnabled(false);
        ui->packetRendererGLWidget->shouldDisplayArcs( true);
    }
}
