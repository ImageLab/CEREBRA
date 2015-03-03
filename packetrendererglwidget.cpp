#include "packetrendererglwidget.h"
#include <limits>

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{
    projection.setToIdentity();
    modelView.setToIdentity();

    alpha = 0;
    beta = 0;
    distance = 60;

    aTimer = new QTimer;
    connect(aTimer,SIGNAL(timeout()),SLOT(animate()));
}

PacketRendererGLWidget::~PacketRendererGLWidget(){

    delete packetToRender;
    delete aTimer;

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

    glDrawArrays(GL_TRIANGLES, 0, voxelVertices.size());

    shaderProgram.setAttributeArray( "vPosition", edgeVertices.constData());
    shaderProgram.setAttributeArray( "vTextureCoordinate", edgeTextureCoordinates.constData());

    glDrawArrays(GL_LINES, 0, edgeVertices.size());

    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
    shaderProgram.setAttributeArray( "vTextureCoordinate", voxelTextureCoordinates.constData());
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

    voxelVertices.clear();
    voxelTextureCoordinates.clear();

    for( int currentVoxel = 0; currentVoxel < (int)packetToRender->vXYZ.size(); currentVoxel++){

        float x = packetToRender->vXYZ[currentVoxel].x;
        float y = packetToRender->vXYZ[currentVoxel].y;
        float z = packetToRender->vXYZ[currentVoxel].z;
        float yTextureOffset = (float)1/(float)(3*packetToRender->vXYZ.size());
        float yPos = ((float)currentVoxel/(float)packetToRender->vXYZ.size()) + yTextureOffset;

        voxelVertices << QVector4D(x-0.35, y-0.35, z+0.35, 1.0) << QVector4D(x+ 0.35, y-0.35, z+0.35, 1.0) << QVector4D(x+ 0.35,  y+0.35, z+0.35, 1.0) // Front
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

        voxelTextureCoordinates << QVector2D(0, yPos) << QVector2D(0, yPos) << QVector2D(0, yPos) // Front
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

    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
    shaderProgram.setAttributeArray( "vTextureCoordinate", voxelTextureCoordinates.constData());

    for(int pair=0; pair < packetToRender->edges.size(); pair++){
        if(packetToRender->edgeIntensities[pair][0] > 0.5){

            QVector3D origin(packetToRender->vXYZ[packetToRender->edges[pair].x].x, packetToRender->vXYZ[packetToRender->edges[pair].x].y, packetToRender->vXYZ[packetToRender->edges[pair].x].z);
            QVector3D destination(packetToRender->vXYZ[packetToRender->edges[pair].y].x, packetToRender->vXYZ[packetToRender->edges[pair].y].y, packetToRender->vXYZ[packetToRender->edges[pair].y].z);

            edgeVertices << QVector4D( origin,1.0) <<  QVector4D( destination,1.0);
            edgeTextureCoordinates <<  QVector2D(0, (float)(pair)/(float)(packetToRender->edges.size())) << QVector2D(0, (float)(pair)/(float)(packetToRender->edges.size()));
        }
    }
}

void PacketRendererGLWidget::setPacket(Packet *packet, QString workingDirectory){

    if( aTimer->isActive())
        aTimer->stop();

    packetToRender = packet;
    this->workingDirectory = workingDirectory;

    createVoxelTexture();

    if( packetToRender->edges.size() > 0)
        createEdgePairTexture();

    voxelTexture = bindTexture(QPixmap(getWorkingDirectory() + QDir::separator() + "voxelTexture.png"));
    shaderProgram.setUniformValue("textureOffset", textureOffset);
    shaderProgram.setUniformValue("texture", 0);
    glBindTexture(GL_TEXTURE_2D, voxelTexture);

    updateAttributeArrays();
    updateMatrices();

    aTimer->start(30); //updating per this amount of milliseconds
}

void PacketRendererGLWidget::createVoxelTexture(){

    QString voxelTextureName = "voxelTexture.png";

//    QImage reference(10,512,QImage::Format_RGB32);
//    for(int x=0;x< reference.width();x++)
//        for(int y=0;y < reference.height();y++){

//            reference.setPixel(x,y,QColor(255*(1-((float)y/(float)reference.height())), 255*((float)y/(float)reference.height()), 0).rgb());
//        }

//    reference.save("reference.png", "PNG");

    QImage texture;

    if( texture.load( getWorkingDirectory() + QDir::separator() + voxelTextureName))
       return;

    //if no intensity available, then create fake one with all intensities = 1;
    if( packetToRender->intensities.size() < 1){

        packetToRender->intensities.clear();
        packetToRender->intensities.resize( (size_t)packetToRender->vXYZ.size());

        for( int voxel = 0; voxel < (int)packetToRender->vXYZ.size(); voxel++)
            packetToRender->intensities[voxel].push_back((float)1.0);
    }

    createTexture( voxelTextureName, packetToRender->intensities);
}

void PacketRendererGLWidget::createEdgePairTexture(){

    QString edgeTextureName = "edgeTexture.png";
    QImage texture;

    if( texture.load( getWorkingDirectory() + QDir::separator() + edgeTextureName))
       return;

    //if no intensity available, then create fake one with all intensities = 1;
    if( packetToRender->edgeIntensities.size() < 1){

        packetToRender->edgeIntensities.clear();
        packetToRender->edgeIntensities.resize( (size_t)packetToRender->edges.size());

        for( int edgePair = 0; edgePair < (int)packetToRender->edges.size(); edgePair++)
            packetToRender->edgeIntensities[edgePair].push_back((float)1.0);
    }

    createTexture( edgeTextureName, packetToRender->edgeIntensities);
}

void PacketRendererGLWidget::createTexture(QString textureName, vector< vector<float> > &intensityValues){

    QImage texture;
    QImage img;
    if ( img.load(":/reference.png")) {

        texture = QImage(12*intensityValues.at(0).size(), 3*intensityValues.size(), QImage::Format_RGB32);

        for( int currentElement = 0; currentElement < (int)intensityValues.size(); currentElement++)
            for( int time = 0; time < (int)(intensityValues[currentElement]).size(); time++){

                int yPos = (1-intensityValues[currentElement][time])*img.height();
                if(yPos >= img.height()) yPos = img.height()-1;
                if(yPos <= 0) yPos = 0;

                int yNextPos = (1-intensityValues[currentElement][(time + 1) % (int)intensityValues[currentElement].size()])*img.height();
                if(yNextPos >= img.height()) yNextPos = img.height()-1;
                if(yNextPos <= 0) yNextPos = 0;

                //not /3, it is /4. (there are 3 virtual time between current and next one ->
                //(current -> virtual1 -> virtual2 -> virtual3 -> next) we do not want
                //virtual3 = next. if it was /3, then virtual3 = next
                float intpolecoeff = (float)(yNextPos - yPos)/(float)4;

                // set pixel kısmında 15 olduğunda gri bir çizgi oluşuyordu. Hesaplamalarıma göre 12 olması daha mantıklı geldi.
                // Her ne kadar o gri çizgiler sende görünmese de aslında varlar:)
                // Bi deneme yapmanı tavsiye ederim.
                for(int i=0; i < 4 ; i++){

                    QRgb intensityOfVoxelInTime = img.pixel(5, yPos+(i*intpolecoeff));

                    texture.setPixel( 12*time+(3*i), 3*currentElement, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+1, 3*currentElement, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+2, 3*currentElement, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i), 3*currentElement+1, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+1, 3*currentElement+1, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+2, 3*currentElement+1, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i), 3*currentElement+2, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+1, 3*currentElement+2, intensityOfVoxelInTime);
                    texture.setPixel( 12*time+(3*i)+2, 3*currentElement+2, intensityOfVoxelInTime);
               }
            }

        texture.save( getWorkingDirectory() + QDir::separator() + textureName, "PNG");
    }
}

void PacketRendererGLWidget::animate(){

    if( packetToRender == NULL || packetToRender->intensities.size() < 1)
        return;

    textureOffset.setX( textureOffset.x() + (float)1/(float)(12*packetToRender->intensities[0].size()));
    //textureOffset.setX( textureOffset.x() + (float)1/(float)(12*packetToRender->intensities[0].size()+300));

    if( textureOffset.x() > 1)
        textureOffset.setX(0);

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
