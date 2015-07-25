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
/**
 * @brief MainWindow class handles user interaction within the system.
 *
 */
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
    /**
     * @brief loadButtonClicked called when
     * .txt file wanted to be select.
     */
    void loadButtonClicked();
    /**
     * @brief displayButtonClicked called after loadind data
     *
     */
    void displayButtonClicked();
    /**
     * @brief loadMatFileButtonClicked called when
     * .mat file wanted to be select.
     */
    void loadMatFileButtonClicked();
    /**
     * @brief loadClusteringMATFileButtonClicked called when
     * clustering variables wanted to be select as .mat format.
     */
    void loadClusteringMATFileButtonClicked();
    /**
     * @brief addButtonClicked called when
     * adding clustering variables to show.
     */
    void addButtonClicked();
    /**
     * @brief removeButtonClicked called when
     * removing clustering variables from the screen.
     */
    void removeButtonClicked();
    /**
     * @brief minValueTextEdited sets text to minimum value of voxel
     * @param text
     */
    void minValueTextEdited( QString text);
    /**
     * @brief maxValueTextEdited sets text to maximum value of voxel
     * @param text
     */
    void maxValueTextEdited( QString text);
    /**
     * @brief minEdgeValueTextEdited sets text to maximum value of edge
     * @param text
     */
    void minEdgeValueTextEdited( QString text);
    /**
     * @brief maxEdgeValueTextEdited sets text to maximum value of edge
     * @param text
     */
    void maxEdgeValueTextEdited( QString text);
    /**
     * @brief thresholdSliderValueChanged sets parameter value to slider value.
     * @param value
     */
    void thresholdSliderValueChanged( int value);
    /**
     * @brief rangeSliderValueChanged sets parameter value to range slider value.
     * @param value
     */
    void rangeSliderValueChanged(int value);
    /**
     * @brief setRangeStateChanged sets parameter state to range state value.
     * @param state
     */
    void setRangeStateChanged( int state);
    /**
     * @brief edgeThresholdSliderValueChanged sets parameter
     * value to edge threshold value.
     * @param value
     */
    void edgeThresholdSliderValueChanged( int value);
    /**
     * @brief edgeRangeSliderValueChanged sets parameter
     * value to edge range slider value.
     * @param value
     */
    void edgeRangeSliderValueChanged(int value);
    /**
     * @brief edgeSetRangeStateChanged sets parameter state
     * to edge range state value.
     * @param state
     */
    void edgeSetRangeStateChanged( int state);
    /**
     * @brief displayArcsStateChanged sets parameter state
     * to display arc state value.
     * @param state
     */
    void displayArcsStateChanged( int state);
    /**
     * @brief displayLabelsStateChanged sets parameter state
     * to display label state value.
     * @param state
     */
    void displayLabelsStateChanged( int state);
    /**
     * @brief clusterVariableChanged sets parameter index
     * to cluster variable value.
     * @param index
     */
    void clusterVariableChanged( int index);
private:    
    /**
     * @brief updateClusterLabelText updates
     * cluster label text.
     */
    void updateClusterLabelText();
    /**
     * @brief setThreshold sets threshoold,
     * it is done when isVoxel returns true.
     * @param isVoxel
     */
    void setThreshold( bool isVoxel);
    /**
     * @brief setMinValue sets minimum value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel
     */
    void setMinValue( bool isVoxel);
    /**
     * @brief setMaxValue sets maximum value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel
     */
    void setMaxValue( bool isVoxel);
    /**
     * @brief setRange sets range value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel
     */
    void setRange( bool isVoxel);
};

#endif // MAINWINDOW_H
