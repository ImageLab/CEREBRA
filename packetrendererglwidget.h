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
    void setPacket( libsimple::Packet packet);

//auxiliary functions
private:

private:

    Packet packetToRender;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    QGLShaderProgram shaderProgram;
    QVector<QVector4D> vertices;
    QVector<QVector4D> colors;
    QVector<QVector2D> textureCoordinates;

    QPoint lastMousePosition;
    bool leftPressed;
    const GLfloat maxZoomIn = 0.2;
    const GLfloat maxZoomOut = 100;

    /**
     * Qt Tutorial variables
     */
    double alpha; //rotate around y axis
    double beta; //rotate around x axis
    double distance;

private:

    void updateMatrices();
    void updateVoxels();
};

#endif // PACKETRENDERERGLWIDGET_H
