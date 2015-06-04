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
    QString comboText;
    QString comboText2;
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PacketFileReader reader;

    //settings for display
    float minValue;
    float maxValue;
    float threshold;
    float range;

public slots:
    void loadButtonClicked();
    void displayButtonClicked();
    void loadMatFileButtonClicked();
    void minValueTextEdited( QString text);
    void maxValueTextEdited( QString text);
    void thresholdSliderValueChanged( int value);
    void rangeSliderValueChanged(int value);
    void setRangeStateChanged( int state);

private:
    void updateThresholdSliderValue( int value);
    void setThreshold( float threshold);
    void setMinValue( float minValue);
    void setMaxValue( float maxValue);
    void setRange( float range);
};

#endif // MAINWINDOW_H
