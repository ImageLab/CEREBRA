#ifndef PACKETRENDERERGLWIDGET_H
#define PACKETRENDERERGLWIDGET_H

#include <QtGui>
#include <QObject>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QVector>
#include <iostream>
#include "Packet.h"
#include <QOpenGLFunctions_3_3_Core>


using namespace std;
/**
 * @brief PacketRendererGLWidget class
 * draw voxels and edges if the necessary data
 * (positions, connections) provided.
 */
class PacketRendererGLWidget : public QGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit PacketRendererGLWidget( QWidget *parent = 0);
    ~PacketRendererGLWidget();

//OpenGL work
private:

    /**
     * @brief initializeGL called just before the display of this widget
     * initializes structure of this widget to make openGL work properly.
     */
    void initializeGL();

    /**
     * @brief paintGL Called when the screen needs update. You may
     * fire this function by calling updateGL()
     */
    void paintGL();

    /**
     * @brief resizeGL called when the size of the window has changed
     * It is suggested that perspective matrix should be calculated here.
     * This class does not provide extra variable to store as the
     * parameters of the perspective matrix.
     * @param width new width of the screen
     * @param height new height of the screen
     */
    void resizeGL( int width, int height);

//events
protected:

    /**
     * @brief mousePressEvent mouse events called when
     * Press, Release, and Move occurs. Move event called
     * even if the mouse is not in pressed state on the widget
     * @param event
     */
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void mouseMoveEvent( QMouseEvent *event);

    /**
     * @brief wheelEvent called when mouse's wheel
     * has changed its state.
     * @param event
     */
    void wheelEvent( QWheelEvent *event);

//services to outside
public:

    /**
     * @brief sizeHint
     * @return preferred size to display this widget
     */
    QSize sizeHint() const;

    /**
     * @brief setPacket updates the packet to be rendered
     * @param packet is the new packet which will be replaced with the current one, if any.
     * @param workingDirectory is the directory where it is asked
     */
    void setPacket( Packet *packet, QString workingDirectory = "");

    /**
     * @brief setWorkingDirectory makes PacketRendererGLWidget to
     * work on the specified directory. If anything specified,
     * it works on the location where the executable is located.
     * @param workingDir is the directory where it is asked
     * to work on.
     */
    void setWorkingDirectory( QString workingDir);
    /**
     * @brief setThresholdRange sets thresholds range for voxel intensities.
     * It arranges range with given threshold values to display voxels that is given
     * in that range.
     * @param minThreshold is the minimum threshold range for voxel intensity values
     * @param maxThreshold is the maximum threshold range for voxel intensity values
     */
    void setThresholdRange( float minThreshold, float maxThreshold);
    /**
     * @brief setPairsThresholdRange sets thresholds range for edge pair intensities.
     * It arranges range with given threshold values to display edpe pairs that is given
     * in that range.
     * @param minThreshold is the minimum threshold range for edge pair intensities
     * @param maxThreshold is the maximum threshold range for edge pair intensities
     */
    void setPairsThresholdRange( float minThreshold, float maxThreshold);
    /**
     * @brief setVoxelMinValue sets minimum value for the voxel intensities.
     * It uses setVoxelMinValue function to normalize voxel intensity values
     * to set minimum value with given voxelMinValue parameter.
     * @param voxelMinValue is the minimum value for the voxel intensities.
     */
    void setVoxelMinValue( float voxelMinValue);
    /**
     * @brief setVoxelMaxValue sets maximum value for the voxel intensities.
     * It uses setVoxelMaxValue function to normalize voxel intensity values
     * to set maximum value with given voxelMaxValue parameter.
     * @param voxelMaxValue is the maximum value for the voxel intensities.
     */
    void setVoxelMaxValue( float voxelMaxValue);
    /**
     * @brief setPairsMinValue sets minimum value for the edge pair intensities.
     * It uses setPairsMinValue function to normalize edge pair intensity values
     * to set minimum value with given pairsMinValue parameter.
     * @param pairsMinValue is the minimum value for the edge pair intensities
     */
    void setPairsMinValue( float pairsMinValue);
    /**
     * @brief setPairsMaxValue sets maximum value for the edge pair intensities.
     * It uses setPairsMaxValue function to normalize edge pair intensity values
     * to set maximum value with given pairsMaxValue parameter.
     * @param pairsMaxValue is the maximum value for the edge pair intensities
     */
    void setPairsMaxValue( float pairsMaxValue);
    /**
     * @brief shouldDisplayArcs decides to show arcs or not.
     * @param shouldDisplay is the boolean variable to decide display arcs.
     */
    void shouldDisplayArcs( bool shouldDisplay);
    /**
     * @brief labelEnabled enables label with given RGB color values.
     * @param label is the index of the label that is enabled
     * @param r is the red value to set label color
     * @param g is the green value to set label color
     * @param b is the blue value to set label color
     */
    void labelEnabled( int label, int r, int g, int b);
    /**
     * @brief labelEnabled enables label that has given index as paramater.
     * @param label is the index of the label that is enabled
     */
    void labelEnabled( int label);
    /**
     * @brief labelDisabled disable label that has given index as paramater.
     * @param label is the index of the label that is disabled.
     */
    void labelDisabled( int label);
    /**
     * @brief disableClusteringDisplay disables clustering variables for displaying on the screen.
     */
    void disableClusteringDisplay();
    /**
     * @brief setLabels set voxel labels when cluster variables has loaded.
     * @param voxelLabels
     */
    void setLabels( std::vector<int> &voxelLabels);
    /**
     * @brief getRGBOfALabel gets RGB of a voxel labels.
     * @param label is the index of the label to get RGB values from it.
     * @param r is the red value received from the label.
     * @param g is the green value received from the label.
     * @param b is the blue value received from the label.
     */
    void getRGBOfALabel( int label, int &r, int &g, int &b);
    /**
     * @brief setRGBOfALabel sets RGB of a voxel labels.
     * @param label is the index of the label to set given RGB values to it.
     * @param r is the red value to set label color
     * @param g is the green value to set label color
     * @param b is the blue value to set label color
     */
    void setRGBForALabel( int label, int r, int g, int b);

//auxiliary functions
private:

    /**
     * @brief updateMatrices calculates new value of modelView matrix
     * by using the variables alpha, beta, and distance. Updates
     * modelview matrix and perspective matrix on the shader side, if any
     */
    void updateMatrices();

    /**
     * @brief updateAttributeArrays updates all necessary arrays
     * that are passed to gpu. It basically uses the information
     * obtain from packetToRender
     */
    void updateAttributeArrays();

    /**
     * @brief initializeShader initiliaze shader.
     */

    void initializeShader();
    /**
     * @brief createVoxelTexture creates voxel texture
     */

    void createVoxelTexture();
    /**
     * @brief createTexture creates voxel texture
     * according to given voxel positions and intensity values.
     * @param textureName
     * @param intensityValues
     */
    void createTexture( QString textureName, vector< vector<float> > &intensityValues);
    /**
     * @brief getWorkingDirectory gets working directory.
     */
    QString getWorkingDirectory();

private:

    Packet *packetToRender;
    QString workingDirectory;

    //shader variables
    QGLShaderProgram shaderProgram;
    std::vector<unsigned int> voxelIndices;
    std::vector<unsigned int> edgeIndices;
    std::vector<int> voxelLabels;
    std::map< int, std::pair<int, std::pair< int, int> > > colorsOfLabels;
    std::map< int, bool> labelActivations;
    QVector<unsigned int> indicesQ;
    QVector<QVector4D> voxelVertices; //passed to gpu
    QVector<GLfloat> voxelTextureIndex; //passed to gpu
    QVector<QVector4D> edgeVertices; //passed to gpu
    QVector<GLfloat> edgeTextureIndex; //passed to gpu
    QVector<GLfloat> edgeLabels;
    QVector<GLfloat> labels;
    GLuint textureOffset;
    GLuint interpolationOffset;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    QMatrix4x4 translationMatrix;

    GLuint voxelIBO;
    GLuint edgesIBO;

    GLuint voxelBO;
    GLuint voxelTBO;
    GLuint edgesBO;
    GLuint edgesTBO;

    float voxelMinValue;
    float voxelMaxValue;
    float pairsMinValue;
    float pairsMaxValue;
    float voxelMaxThreshold;
    float voxelMinThreshold;
    float pairsMaxThreshold;
    float pairsMinThreshold;
    bool displayArcs;
    bool displayLabels;

    /**
     * To handle rotation, zooming etc.
     */
    QPoint lastMousePosition;
    double alpha; //rotate around y axis
    double beta; //rotate around x axis

    float moveX; //move on y axis
    float moveY; //move on x axis

    double distance;

    QTimer *aTimer;

    bool shouldAnimate;

private slots:
     void animate();
};

#endif // PACKETRENDERERGLWIDGET_H
