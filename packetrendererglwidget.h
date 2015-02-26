#ifndef PACKETRENDERERGLWIDGET_H
#define PACKETRENDERERGLWIDGET_H

#include <QtGui>
#include <QObject>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QVector>
#include <vector>
#include "Packet.h"

/**
 * @brief The GLWidget class
 * This QGLWidget class handles OpenGL work of the project.
 * It is supposed to draw voxels if the necessary data
 * (positions, texture coordinates etc.) provided. Anyone
 * using this widget should not bother with OpenGL work.
 */
using namespace std;
using namespace libsimple;

class PacketRendererGLWidget : public QGLWidget
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
    void setPacket( libsimple::Packet packet);

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
     * @brief readVoxels is just a testing function
     * It reads specifically designed txt file
     * to obtain voxel position, intensity etc.
     */
    void readVoxels();
    void readImage();
    void readEdges();

private:

    Packet packetToRender;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    QGLShaderProgram shaderProgram;
    QVector<QVector4D> vertices; //passed to gpu
    QVector<QVector4D> colors; //passed to gpu
    QVector<QVector2D> textureCoordinates; //passed to gpu
    QVector<QVector2D> pairs;
    QVector<float> edgeIntensities;

    /**
     * To handle rotation, zooming etc.
     */
    QPoint lastMousePosition;
    bool leftPressed;
    const GLfloat maxZoomIn = 0.2;
    const GLfloat maxZoomOut = 100;
    double alpha; //rotate around y axis
    double beta; //rotate around x axis
    double distance;

    //auxiliary variables. will be removed later
    QVector<QVector3D> fileVertexPos;
    QVector<float> intensities;
    QVector< QVector<float>> contIntensities;

};

#endif // PACKETRENDERERGLWIDGET_H
