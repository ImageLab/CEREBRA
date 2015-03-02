#include "packetrendererglwidget.h"
#include <limits>

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{
    projection.setToIdentity();
    modelView.setToIdentity();

    alpha = 0;
    beta = 0;
    distance = 60;

    //QTimer *aTimer = new QTimer;
    //connect(aTimer,SIGNAL(timeout()),SLOT(animate()));
    //aTimer->start(100); //updating per this amount of milliseconds
}

PacketRendererGLWidget::~PacketRendererGLWidget(){

    delete packetToRender;

    //if working directory not specified, delete all the files
    //created while the program is running.
}

QSize PacketRendererGLWidget::sizeHint() const{

    return QSize( 1024, 768);
}

void PacketRendererGLWidget::initializeGL(){

    projection.setToIdentity();
    modelView.setToIdentity();
    textureOffset = QVector2D(0.0, 0.0);

    //gl attributes
    glEnable( GL_DEPTH_TEST);
    qglClearColor( QColor( Qt::gray));

    //-----------think those doing in setpacket
    //link and bind shader
    shaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/vertexshader.vsh");
    shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/fragmentshader.fsh");

    shaderProgram.link();
    shaderProgram.bind();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "vTextureCoordinate");
}

void PacketRendererGLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

//    shaderProgram.setAttributeArray( "vPosition", EdgePos.constData());
//    shaderProgram.setAttributeArray( "vColor", edgeColors.constData());

//    glDrawArrays(GL_LINES, 0, EdgePos.size());

//    shaderProgram.setAttributeArray( "vPosition", vertices.constData());
//    shaderProgram.setAttributeArray( "vTextureCoordinate", textureCoordinates.constData());
}

void PacketRendererGLWidget::resizeGL( int width, int height){

    if (height == 0)
        height = 1;

    projection.setToIdentity();
    projection.perspective( (float)60.0, (float)width/(float)height, (float)0.001, (float)1000.0);

    glViewport( 0, 0, width, height);

    updateMatrices();
}

void PacketRendererGLWidget::mousePressEvent( QMouseEvent *event){

    if( event->button() == Qt::LeftButton)
        lastMousePosition = event->pos();

    event->accept();
}

void PacketRendererGLWidget::mouseReleaseEvent( QMouseEvent *event){

    event->accept();
}

void PacketRendererGLWidget::mouseMoveEvent( QMouseEvent *event){

    if( event->buttons() & Qt::LeftButton){

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

        if (delta < 0)
            distance *= 1.1;
        else if (delta > 0)
            distance *= 0.9;

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

void PacketRendererGLWidget::updateAttributeArrays(){

    vertices.clear();
    textureCoordinates.clear();

    for( int currentVoxel = 0; currentVoxel < (int)packetToRender->vXYZ.size(); currentVoxel++){

        float x = packetToRender->vXYZ[currentVoxel].x;
        float y = packetToRender->vXYZ[currentVoxel].y;
        float z = packetToRender->vXYZ[currentVoxel].z;
        float yTextureOffset = (float)1/(float)(3*packetToRender->vXYZ.size());
        float yPos = ((float)currentVoxel/(float)packetToRender->vXYZ.size()) + yTextureOffset;

        vertices << QVector4D(x-0.35, y-0.35, z+0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z+0.35, 1.0) // Front
                << QVector4D(x+ 0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x-0.35, y-0.35, z+0.35, 1.0)
                << QVector4D(x+ 0.35, y-0.35, z-0.35, 1.0) << QVector4D(x-0.35, y-0.35, z-0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z-0.35, 1.0) // Back
                << QVector4D(x-0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z-0.35, 1.0)
                << QVector4D(x-0.35, y-0.35, z-0.35, 1.0) << QVector4D(x-0.35, y-0.35, z+0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z+0.35, 1.0) // Left
                << QVector4D(x-0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x-0.35, y-0.35, z-0.35, 1.0)
                << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z-0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z-0.35, 1.0) // Right
                << QVector4D(x+ 0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0)
                << QVector4D(x-0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z+0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z-0.35, 1.0) // Top
                << QVector4D(x+ 0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z-0.35, 1.0) << QVector4D(x-0.35,  y+0.35, z+0.35, 1.0)
                << QVector4D(x-0.35, y-0.35, z-0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z-0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0) // Bottom
                << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0) << QVector4D(x-0.35, y-0.35, z+0.35, 1.0) << QVector4D(x-0.35, y-0.35, z-0.35, 1.0);

        textureCoordinates << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Front
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos)
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Back
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos)
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Left
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos)
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Right
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos)
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Top
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos)
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Bottom
                          << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos);
    }

    shaderProgram.setAttributeArray( "vPosition", vertices.constData());
    shaderProgram.setAttributeArray( "vTextureCoordinate", textureCoordinates.constData());
}

void PacketRendererGLWidget::setPacket(Packet *packet, QString workingDirectory){

    packetToRender = packet;
    this->workingDirectory = workingDirectory;

    createTexture();

    texture = bindTexture(QPixmap(getWorkingDirectory() + QDir::separator() + "texture.png"));
    shaderProgram.setUniformValue("textureOffset", textureOffset);
    shaderProgram.setUniformValue("texture", 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    updateAttributeArrays();
    updateMatrices();
}

void PacketRendererGLWidget::createTexture(){

    QImage texture;

    if( texture.load( getWorkingDirectory() + QDir::separator() + "texture.png"))
        return;

    //if no intensity available, then create fake one with all intensities = 1;
    if( packetToRender->intensities.size() < 1){

        packetToRender->intensities.resize( packetToRender->vXYZ.size());

        for( int voxel = 0; voxel < (int)packetToRender->vXYZ.size(); voxel++){
            packetToRender->intensities[voxel].resize((size_t)1);
            packetToRender->intensities[voxel][0] = 1.0;
        }
    }

    QImage img;
    if ( img.load(":/reference.png")) {

        texture = QImage(3*packetToRender->intensities.at(0).size(), 3*packetToRender->intensities.size(), QImage::Format_RGB32);

        for( int currentVoxel = 0; currentVoxel < (int)packetToRender->intensities.size(); currentVoxel++)
            for( int time = 0; time < (int)(packetToRender->intensities[currentVoxel]).size(); time++){

                int yPos = (1-packetToRender->intensities[currentVoxel][time])*img.height();
                if(yPos >= img.height()) yPos = img.height()-1;
                if(yPos <= 0) yPos = 0;
                QRgb intensityOfVoxelInTime = img.pixel(5, yPos);
                std::cout << yPos << std::endl;

                texture.setPixel( 3*time, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel( 3*time+1, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel( 3*time+2, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel( 3*time, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel( 3*time+1, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel( 3*time+2, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel( 3*time, 3*currentVoxel+2, intensityOfVoxelInTime);
                texture.setPixel( 3*time+1, 3*currentVoxel+2, intensityOfVoxelInTime);
                texture.setPixel( 3*time+2, 3*currentVoxel+2, intensityOfVoxelInTime);
            }

        texture.save( getWorkingDirectory() + QDir::separator() + "texture.png", "PNG");
    }
}

void PacketRendererGLWidget::animate(){

    if( !packetToRender || packetToRender->intensities.size() < 1)
        return;

    float offset = textureOffset.x() + ((float)1/(float)packetToRender->intensities.at(0).size());
    if( offset > 1)
        offset = 0;

    textureOffset = QVector2D( offset, 0);
    shaderProgram.setUniformValue("textureOffset", textureOffset);
    updateGL();
}

void PacketRendererGLWidget::setWorkingDirectory( QString workingDir){

    workingDirectory = workingDir;
}

QString PacketRendererGLWidget::getWorkingDirectory(){

    if( workingDirectory.size() < 1)
        workingDirectory = qApp->applicationDirPath();

    return workingDirectory;
}
