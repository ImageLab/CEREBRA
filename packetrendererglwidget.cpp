#include "packetrendererglwidget.h"
#include <limits>

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{

    leftPressed = false;

    projection.setToIdentity();
    modelView.setToIdentity();

    alpha = 25;
    beta = -25;
    distance = 2.5;

    //sample voxel
    //    packetToRender.vXYZ = new libsimple::Packet::Point3D[4];

    //    packetToRender.vXYZ[0].x = 0;
    //    packetToRender.vXYZ[0].y = 0;
    //    packetToRender.vXYZ[0].z = 0;

    //    packetToRender.vXYZ[1].x = 1;
    //    packetToRender.vXYZ[1].y = 1;
    //    packetToRender.vXYZ[1].z = 1;

    //    packetToRender.vXYZ[2].x = 2;
    //    packetToRender.vXYZ[2].y = 2;
    //    packetToRender.vXYZ[2].z = 2;

    //    packetToRender.vXYZ[3].x = 2;
    //    packetToRender.vXYZ[3].y = 5;
    //    packetToRender.vXYZ[3].z = 5;

    //    packetToRender.Intensities = new double*[1];
    //    packetToRender.Intensities[0] = new double[4];

    //    packetToRender.Intensities[0][0] = 0.77;
    //    packetToRender.Intensities[0][1] = 0.3;
    //    packetToRender.Intensities[0][2] = 0.1;
    //    packetToRender.Intensities[0][3] = 0.9;

    readVoxels();
}

PacketRendererGLWidget::~PacketRendererGLWidget()
{

}

QSize PacketRendererGLWidget::sizeHint() const{

    return QSize( 1024, 768);
}

void PacketRendererGLWidget::initializeGL(){

    projection.setToIdentity();
    modelView.setToIdentity();

    //gl attributes
    glEnable( GL_DEPTH_TEST);
    qglClearColor( QColor( Qt::black));

    //link and bind shader
    shaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/vertexshader.vsh");
    shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/fragmentshader.fsh");

    shaderProgram.link();
    shaderProgram.bind();

    updateVoxels();
    updateMatrices();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "vColor");
}

void PacketRendererGLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void PacketRendererGLWidget::resizeGL( int width, int height){

    if (height == 0) {
        height = 1;
    }

    projection.setToIdentity();
    projection.perspective( 60.0, (double)width/(double)height, 0.001, 1000);
    glViewport( 0, 0, width, height);

    updateMatrices();
}

void PacketRendererGLWidget::mousePressEvent( QMouseEvent *event){

    if( event->button() == Qt::LeftButton){
        lastMousePosition = event->pos();
        leftPressed = true;
    }

    event->accept();
}

void PacketRendererGLWidget::mouseReleaseEvent( QMouseEvent *event){

    if( event->button() == Qt::LeftButton)
        leftPressed = false;

    event->accept();
}

void PacketRendererGLWidget::mouseMoveEvent( QMouseEvent *event){

    if( event->buttons() == Qt::LeftButton && leftPressed){

        float dx = event->x() - lastMousePosition.x();
        float dy = event->y() - lastMousePosition.y();

        alpha -= dx;
        while (alpha < 0) alpha += 360;
        while (alpha >= 360) alpha -= 360;

        beta -= dy;
        while (beta < 0) beta += 360;
        while (beta >= 360) beta -= 360;

        lastMousePosition = event->pos();

        updateMatrices();
    }

    event->accept();
}

void PacketRendererGLWidget::wheelEvent( QWheelEvent *event){

    int delta = event->delta();
    if (event->orientation() == Qt::Vertical) {

        if (delta < 0) {
            distance *= 1.1;
        } else if (delta > 0) {
            distance *= 0.9;
        }

        updateMatrices();
    }
    event->accept();
}

void PacketRendererGLWidget::updateMatrices(){

    modelView.setToIdentity();
    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(alpha, 0, 1, 0);
    cameraTransformation.rotate(beta, 1, 0, 0);
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
    modelView.lookAt( cameraPosition, QVector3D(0,0,0), cameraUpDirection);

    shaderProgram.setUniformValue("projection", projection);
    shaderProgram.setUniformValue("modelView", modelView);
    updateGL();
}

void PacketRendererGLWidget::updateVoxels(){

    //length fixed for now (See constructor)
    for( int currentVoxel = 0; currentVoxel < fileVertexPos.length(); currentVoxel++){

        float x = packetToRender.vXYZ[currentVoxel].x;
        float y = packetToRender.vXYZ[currentVoxel].y;
        float z = packetToRender.vXYZ[currentVoxel].z;
        float intensity = packetToRender.Intensities[0][currentVoxel];

        vertices << QVector4D(x-0.5, y-0.5, z+0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z+0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z+0.5, 1.0) // Front
                << QVector4D(x+ 0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x-0.5, y-0.5, z+0.5, 1.0)
                << QVector4D(x+ 0.5, y-0.5, z-0.5, 1.0) << QVector4D(x-0.5, y-0.5, z-0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z-0.5, 1.0) // Back
                << QVector4D(x-0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z-0.5, 1.0)
                << QVector4D(x-0.5, y-0.5, z-0.5, 1.0) << QVector4D(x-0.5, y-0.5, z+0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z+0.5, 1.0) // Left
                << QVector4D(x-0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x-0.5, y-0.5, z-0.5, 1.0)
                << QVector4D(x+ 0.5, y-0.5, z+0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z-0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z-0.5, 1.0) // Right
                << QVector4D(x+ 0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z+0.5, 1.0)
                << QVector4D(x-0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z+0.5, 1.0) << QVector4D(x+ 0.5,  y+0.5, z-0.5, 1.0) // Top
                << QVector4D(x+ 0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z-0.5, 1.0) << QVector4D(x-0.5,  y+0.5, z+0.5, 1.0)
                << QVector4D(x-0.5, y-0.5, z-0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z-0.5, 1.0) << QVector4D(x+ 0.5, y-0.5, z+0.5, 1.0) // Bottom
                << QVector4D(x+ 0.5, y-0.5, z+0.5, 1.0) << QVector4D(x-0.5, y-0.5, z+0.5, 1.0) << QVector4D(x-0.5, y-0.5, z-0.5, 1.0);


        colors << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Front
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0)
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Back
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0)
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Left
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0)
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Right
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0)
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Top
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0)
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) // Bottom
               << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0) << QVector4D(intensity, 1-intensity, 0, 1.0);
    }

    shaderProgram.setAttributeArray( "vPosition", vertices.constData());
    shaderProgram.setAttributeArray( "vColor", colors.constData());
}

void PacketRendererGLWidget::setPacket( Packet packet){

    //copy constructor here
    packetToRender = packet;

    updateVoxels();
}

void PacketRendererGLWidget::readVoxels(){

    QFile file(":/voxels.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }

    QTextStream instream(&file);
    QString pos;

    while( (pos = instream.readLine()) != NULL){

        QString intensityString = instream.readLine();

        QRegExp rx("[ ]");
        QStringList list = pos.split(rx, QString::SkipEmptyParts);
        fileVertexPos << QVector3D( list.at(0).toFloat(), list.at(1).toFloat(), list.at(2).toFloat());
        intensities << intensityString.toFloat();
    }

    //normalize intensities
    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    //find min-max values
    for( int i = 0; i < intensities.length(); i++){

        if( intensities[i] < min)
            min = intensities[i];
        if( intensities[i] > max)
            max = intensities[i];
    }

    for( int i = 0; i < intensities.length(); i++) {
        intensities[i] = (intensities[i] - min)/(max - min);
        printf( "%f\n", intensities[i]);
    }

    packetToRender.vXYZ = new libsimple::Packet::Point3D[fileVertexPos.length()];
    packetToRender.Intensities = new double*[1];
    packetToRender.Intensities[0] = new double[intensities.length()];

    for( int i = 0; i < fileVertexPos.length(); i++){

        packetToRender.vXYZ[i].x = fileVertexPos[i].x();
        packetToRender.vXYZ[i].y = fileVertexPos[i].y();
        packetToRender.vXYZ[i].z = fileVertexPos[i].z();
        packetToRender.Intensities[0][i] = intensities[i];
    }
    file.close();
}
