#ifndef PACKETRENDERERGLWIDGET_H
#define PACKETRENDERERGLWIDGET_H

#include <QtGui>
#include <QObject>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QVector>

#include "Packet.h"
#include <QOpenGLFunctions_3_3_Core>

/**
 * @brief The GLWidget class
 * This QGLWidget class handles OpenGL work of the project.
 * It is supposed to draw voxels if the necessary data
 * (positions, texture coordinates etc.) provided. Anyone
 * using this widget should not bother with OpenGL work.
 */
using namespace std;
using namespace libsimple;

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
     */
    void setPacket( libsimple::Packet *packet, QString workingDirectory = "");

    /**
     * @brief setWorkingDirectory makes PacketRendererGLWidget to
     * work on the specified directory. If anything specified,
     * it works on the location where the executable is located.
     * @param workingDirectory is the directory where it is asked
     * to work on.
     */
    void setWorkingDirectory( QString workingDir);

    void setThresholdRange( float minThreshold, float maxThreshold);
    void setPairsThresholdRange( float minThreshold, float maxThreshold);
    void setVoxelMinValue( float voxelMinValue);
    void setVoxelMaxValue( float voxelMaxValue);
    void setPairsMinValue( float pairsMinValue);
    void setPairsMaxValue( float pairsMaxValue);
    void shouldDisplayArcs( bool shouldDisplay);

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

    void initializeShader();

    void createVoxelTexture();
    void createTexture( QString textureName, vector< vector<float> > &intensityValues);

    QString getWorkingDirectory();

private:

    Packet *packetToRender;
    QString workingDirectory;

    //shader variables
    QGLShaderProgram shaderProgram;
    std::vector<unsigned int> voxelIndices;
    std::vector<unsigned int> edgeIndices;
    QVector<unsigned int> indicesQ;
    QVector<QVector4D> voxelVertices; //passed to gpu
    QVector<QVector2D> voxelTextureIndex; //passed to gpu
    QVector<QVector4D> colors; //passed to gpu
    QVector<QVector4D> edgeVertices; //passed to gpu
    QVector<QVector2D> edgeTextureIndex; //passed to gpu
    GLuint textureOffset;
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
