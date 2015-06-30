#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
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

    QString clusteringFileName;
    std::vector<int> labels;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PacketFileReader reader;

public slots:
    void loadButtonClicked();
    void displayButtonClicked();
    void loadMatFileButtonClicked();
    void loadClusteringMATFileButtonClicked();
    void addButtonClicked();
    void removeButtonClicked();

    void minValueTextEdited( QString text);
    void maxValueTextEdited( QString text);
    void minEdgeValueTextEdited( QString text);
    void maxEdgeValueTextEdited( QString text);

    void thresholdSliderValueChanged( int value);
    void rangeSliderValueChanged(int value);
    void setRangeStateChanged( int state);
    void edgeThresholdSliderValueChanged( int value);
    void edgeRangeSliderValueChanged(int value);
    void edgeSetRangeStateChanged( int state);

    void displayArcsStateChanged( int state);
    void displayLabelsStateChanged( int state);

    void clusterVariableChanged( int index);
private:

    void updateClusterLabelText();
    void setThreshold( bool isVoxel);
    void setMinValue( bool isVoxel);
    void setMaxValue( bool isVoxel);
    void setRange( bool isVoxel);
};

#endif // MAINWINDOW_H
