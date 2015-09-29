#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include "packetfilereader.h"


/**
 *
 * @section DESCRIPTION
 *
 *CEREBRA is developed using QT UI Development Framework and C/C++
 *is used as programming languages. OpenGL with GLSL is used to utilize the
 *power of GPU power in order to achieve high performance on visualization task.
 *This software can run under a 64-bit Windows environment. CEREBRA can
 *load and display .txt and .mat files. The software is tested successfully under
 *a 64-bit Windows 8, and 8.1 environments. There is no additional installation
 *is needed to run CEREBRA, it is a standalone toolbox.
 *
 */


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
     * @param text is minimum value of voxel text which is editted.
     */
    void minValueTextEdited( QString text);
    /**
     * @brief maxValueTextEdited sets text to maximum value of voxel
     * @param text is maximum value of voxel text which is editted.
     */
    void maxValueTextEdited( QString text);
    /**
     * @brief minEdgeValueTextEdited sets text to maximum value of edge
     * @param text is minimum value of edge text which is editted.
     */
    void minEdgeValueTextEdited( QString text);
    /**
     * @brief maxEdgeValueTextEdited sets text to maximum value of edge
     * @param text is maximum value of edge text which is editted.
     */
    void maxEdgeValueTextEdited( QString text);
    /**
     * @brief thresholdSliderValueChanged sets parameter value to slider value.
     * @param value is threshoold value which is editted.
     */
    void thresholdSliderValueChanged( int value);
    /**
     * @brief rangeSliderValueChanged sets parameter value to range slider value.
     * @param value is range slider value which is editted.
     */
    void rangeSliderValueChanged(int value);
    /**
     * @brief setRangeStateChanged sets parameter state to range state value.
     * @param state is the range state value.
     */
    void setRangeStateChanged( int state);
    /**
     * @brief edgeThresholdSliderValueChanged sets parameter
     * value to edge threshold value.
     * @param value is edge thresold slider value which is editted.
     */
    void edgeThresholdSliderValueChanged( int value);
    /**
     * @brief edgeRangeSliderValueChanged sets parameter
     * value to edge range slider value.
     * @param value is edge range slider value which is editted.
     */
    void edgeRangeSliderValueChanged(int value);
    /**
     * @brief edgeSetRangeStateChanged sets parameter state
     * to edge range state value.
     * @param state is the value of edges states.
     */
    void edgeSetRangeStateChanged( int state);
    /**
     * @brief displayArcsStateChanged sets parameter state
     * to display arc state value.
     * @param state is the value of the arc states.
     */
    void displayArcsStateChanged( int state);
    /**
     * @brief displayLabelsStateChanged sets parameter state
     * to display label state value.
     * @param state is the value of the diplaying label state.
     */
    void displayLabelsStateChanged( int state);
    /**
     * @brief clusterVariableChanged sets parameter index
     * to cluster variable value.
     * @param index is the index of the cluster to change.
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
     * @param isVoxel isVoxel the boolean value to set thresold value with respect to voxel or edge.
     */
    void setThreshold( bool isVoxel);
    /**
     * @brief setMinValue sets minimum value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel the boolean value to set minimum value with respect to voxel or edge.
     */
    void setMinValue( bool isVoxel);
    /**
     * @brief setMaxValue sets maximum value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel the boolean value to set maximum value with respect to voxel or edge.
     */
    void setMaxValue( bool isVoxel);
    /**
     * @brief setRange sets range value of the voxel,
     * it is done when isVoxel returns true.
     * @param isVoxel the boolean value to set range value with respect to voxel or edge.
     */
    void setRange( bool isVoxel);
};

#endif // MAINWINDOW_H
