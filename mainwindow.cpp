#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <cmath>
#include "libmatiohelper.h"

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
    ui->matlabTab->setEnabled( false);
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
    displayLabelsStateChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadButtonClicked(){

    clusteringFileName.clear();
    ui->displayArcsCheckBox->setChecked( false);
    ui->matlabTab->setEnabled( false);

    QString directoryName = QFileDialog::getExistingDirectory();

    if( !directoryName.isNull()){
        ui->matlabTab->setEnabled(false);
        ui->packetRendererGLWidget->setPacket( reader.readPacketFromDirectory( directoryName), directoryName);
    }
}

void MainWindow::loadMatFileButtonClicked(){

    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.mat*)"));

    if(fileName.length()== 0)
        return;

    directory = QFileInfo(fileName).absoluteDir().absolutePath();

    std::vector< std::string> variables;

    if( LibMatioHelper::getVariables( fileName.toStdString().c_str(), variables) == EXIT_FAILURE){

        QMessageBox::warning(this, tr("Error"),
                                   tr("There is an error while openning the file.\n"),
                                   QMessageBox::Ok);

        return;
    }

    ui->displayButton->setEnabled(true);

    ui->voxelPositionComboBox->clear();
    ui->voxelIntensitiesComboBox->clear();
    ui->edgePairsComboBox->clear();
    ui->edgeIntensitiesComboBox->clear();

    ui->voxelPositionComboBox->addItem("Select...");
    ui->voxelIntensitiesComboBox->addItem("Select...");
    ui->edgePairsComboBox->addItem("Select...");
    ui->edgeIntensitiesComboBox->addItem("Select...");

    for( int curVar = 0; curVar < (int)variables.size(); curVar++){

        ui->voxelPositionComboBox->addItem(QString(variables.at(curVar).c_str()));
        ui->voxelIntensitiesComboBox->addItem(QString(variables.at(curVar).c_str()));
        ui->edgePairsComboBox->addItem(QString(variables.at(curVar).c_str()));
        ui->edgeIntensitiesComboBox->addItem(QString(variables.at(curVar).c_str()));
    }

    ui->matlabTab->setEnabled(true);
}

void MainWindow::displayButtonClicked()
{
    if( !fileName.isNull())
        ui->packetRendererGLWidget->
                setPacket(reader.readPacketFromMatlab( fileName,
                                                       (ui->voxelPositionComboBox->currentIndex() == 0)?NULL:ui->voxelPositionComboBox->currentText(),
                                                       (ui->voxelIntensitiesComboBox->currentIndex() == 0)?NULL:ui->voxelIntensitiesComboBox->currentText(),
                                                       (ui->edgePairsComboBox->currentIndex() == 0)?NULL:ui->edgePairsComboBox->currentText(),
                                                       (ui->edgeIntensitiesComboBox->currentIndex() == 0)?NULL:ui->edgeIntensitiesComboBox->currentText()),
                                                       directory);
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
    float minThreshold = (setRangeCheckBox->isChecked() && threshold - gap >= minValue)?(threshold-gap):minValue;
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
    float minThreshold = (threshold - gap < minValue)?minValue:(threshold-gap);
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

void MainWindow::loadClusteringMATFileButtonClicked(){

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.mat*)"));

    if( fileName.length()== 0)
        return;

    clusteringFileName = fileName;

    std::vector< std::string> variables;

    if( LibMatioHelper::getVariables( fileName.toStdString().c_str(), variables) == EXIT_FAILURE){

        QMessageBox::warning(this, tr("Error"),
                                   tr("There is an error while openning the file.\n"),
                                   QMessageBox::Ok);

        return;
    }

    ui->clusterVariablesComboBox->clear();

    for( int curVar = 0; curVar < variables.size(); curVar++)
        ui->clusterVariablesComboBox->addItem( variables.at(curVar).c_str());
}

void MainWindow::updateClusterLabelText(){

    QString chosenLabelsString = "";
    for( int curSelectedLabel = 0; curSelectedLabel < ui->removeClusterLabelComboBox->count(); curSelectedLabel++){
        QString label = ui->removeClusterLabelComboBox->itemText(curSelectedLabel);
        int r,g,b;
        ui->packetRendererGLWidget->getRGBOfALabel( label.toInt(), r,g,b);
        chosenLabelsString = chosenLabelsString + " <p style=\"color:rgb(" + QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + ")\">" + label + "</p>";
    }

    ui->chosenLabelsWithColorsLabel->setText( chosenLabelsString);
}

void MainWindow::addButtonClicked(){

    int label = ui->clusterLabelComboBox->currentText().toInt();
    ui->packetRendererGLWidget->labelEnabled( label);
    ui->removeClusterLabelComboBox->addItem( QString::number(label));
    ui->clusterLabelComboBox->removeItem( ui->clusterLabelComboBox->currentIndex());

    int r,g,b;
    ui->packetRendererGLWidget->getRGBOfALabel( label, r, g, b);

    updateClusterLabelText();
}

void MainWindow::removeButtonClicked(){

    QString label = ui->removeClusterLabelComboBox->currentText();
    ui->packetRendererGLWidget->labelDisabled( label.toInt());
    ui->clusterLabelComboBox->addItem( label);
    ui->removeClusterLabelComboBox->removeItem( ui->removeClusterLabelComboBox->currentIndex());

    updateClusterLabelText();
}

void MainWindow::clusterVariableChanged( int index){

    labels.clear();

    if( clusteringFileName.length() > 0){
        LibMatioHelper::getIntegerValues( clusteringFileName.toStdString().c_str(),
                                           ui->clusterVariablesComboBox->itemText( index).toStdString().c_str(), labels);
        QVector<int> sortedLabels;

        for( int curVar = 0; curVar < labels.size(); curVar++)
            if( !sortedLabels.contains( labels[curVar]))
                sortedLabels << labels[curVar];

        qSort( sortedLabels);

        ui->clusterLabelComboBox->clear();
        ui->removeClusterLabelComboBox->clear();
        ui->chosenLabelsWithColorsLabel->clear();
        for( int curVar = 0; curVar < sortedLabels.size(); curVar++)
            ui->clusterLabelComboBox->addItem( QString::number(sortedLabels[curVar]));
    }

    ui->packetRendererGLWidget->setLabels( labels);
}

void MainWindow::displayLabelsStateChanged( int state){

    if( state == 0){

        ui->clusterVariablesComboBox->setEnabled(false);
        ui->clusterVariablesComboBox->clear();
        ui->clusterLabelComboBox->setEnabled( false);
        ui->clusterLabelComboBox->clear();
        ui->addClusterLabelPushButton->setEnabled( false);
        ui->removeClusterLabelComboBox->setEnabled( false);
        ui->removeClusterLabelComboBox->clear();
        ui->removeClusterLabelPushButton->setEnabled( false);
        ui->chosenLabelsWithColorsLabel->clear();
        ui->loadClusterMatButton->setEnabled( false);
        clusteringFileName.clear();
        ui->packetRendererGLWidget->disableClusteringDisplay();
    } else{
        ui->clusterVariablesComboBox->setEnabled( true);
        ui->clusterLabelComboBox->setEnabled( true);
        ui->addClusterLabelPushButton->setEnabled( true);
        ui->removeClusterLabelComboBox->setEnabled( true);
        ui->removeClusterLabelPushButton->setEnabled( true);
        ui->loadClusterMatButton->setEnabled( true);
        clusterVariableChanged( ui->clusterLabelComboBox->currentIndex());
    }
}
