#include "packetrendererglwidget.h"
#include <limits>

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{

    leftPressed = false;

    projection.setToIdentity();
    modelView.setToIdentity();

    alpha = 0;
    beta = 0;
    distance = 60;

    readVoxels();
    readImage();
    readEdges();
    readEdgeIntensities();
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

    updateAttributeArrays();
    updateMatrices();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "vColor");
}

void PacketRendererGLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    shaderProgram.setAttributeArray( "vPosition", EdgePos.constData());
    shaderProgram.setAttributeArray( "vColor", edgeColors.constData());

    glDrawArrays(GL_LINES, 0, EdgePos.size());

    shaderProgram.setAttributeArray( "vPosition", vertices.constData());
    shaderProgram.setAttributeArray( "vColor", colors.constData());
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

void PacketRendererGLWidget::updateAttributeArrays(){

    vertices.clear();
    colors.clear();
    textureCoordinates.clear();

    //length fixed for now (See constructor)
    for( int currentVoxel = 0; currentVoxel < fileVertexPos.length(); currentVoxel++){

        float x = packetToRender.vXYZ[currentVoxel].x;
        float y = packetToRender.vXYZ[currentVoxel].y;
        float z = packetToRender.vXYZ[currentVoxel].z;
        float intensity = (float)packetToRender.Intensities[0][currentVoxel];

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

    updateAttributeArrays();
}

void PacketRendererGLWidget::readVoxels(){

    //here we assume that the file contains
    //2 consecutive lines. in the first line
    //there is position information (x y z)
    //and in the following line there is the
    //intensity value of that voxel.
    //it is just a parser to train the rendering.
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

    for( int i = 0; i < intensities.length(); i++)
        intensities[i] = (intensities[i] - min)/(max - min);

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

    QFile contIntensityFile(":/intensities.txt");

    if(!contIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }

    QTextStream instream2(&contIntensityFile);
    QString intensitiesOfVoxel;

    max = std::numeric_limits<float>::min();
    min = std::numeric_limits<float>::max();
    while( (intensitiesOfVoxel = instream2.readLine()) != NULL){

        QRegExp rx("[,]");
        QStringList list = intensitiesOfVoxel.split(rx, QString::SkipEmptyParts);
        QVector<float> intensityVectorOfVoxel;

        for( int i = 0; i < list.length(); i++){

            intensityVectorOfVoxel << list.at(i).toFloat();

            if( list.at(i).toFloat() > max)
                max = list.at(i).toFloat();
            if( list.at(i).toFloat() < min)
                min = list.at(i).toFloat();
        }

        contIntensities << intensityVectorOfVoxel;
    }
}

void PacketRendererGLWidget::readImage(){

    QImage img;
    if ( img.load(":/reference.png")) {

        QImage texture(3*contIntensities.at(0).length(), 3*contIntensities.length(), QImage::Format_RGB32);

        for( int currentVoxel = 0; currentVoxel < contIntensities.length(); currentVoxel++)
            for( int time = 0; time < contIntensities.at(currentVoxel).length(); time++){

                int yPos = (1-contIntensities.at(currentVoxel).at(time))*img.height();
                if(yPos >= img.height()) yPos = img.height()-1;
                if(yPos <= 0) yPos = 0;
                QRgb intensityOfVoxelInTime = img.pixel(5, yPos);

                texture.setPixel(3*time, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel(3*time+1, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel(3*time+2, 3*currentVoxel, intensityOfVoxelInTime);
                texture.setPixel(3*time, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel(3*time+1, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel(3*time+2, 3*currentVoxel+1, intensityOfVoxelInTime);
                texture.setPixel(3*time, 3*currentVoxel+2, intensityOfVoxelInTime);
                texture.setPixel(3*time+1, 3*currentVoxel+2, intensityOfVoxelInTime);
                texture.setPixel(3*time+2, 3*currentVoxel+2, intensityOfVoxelInTime);
            }

        texture.save("texture.png", "PNG");
    }
}

void PacketRendererGLWidget::readEdges(){

    QFile file(":/neighbors.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }

    QTextStream instream(&file);
    QString voxelLine;
    int count = 0;
    while( (voxelLine = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = voxelLine.split(rx, QString::SkipEmptyParts);
       // std::cout << list.length()<< std::endl;
        for(int i=0; i< list.length();i++){

            pairs << QVector2D(count, list[i].toInt()-1);

        }
        count++;

    }

}
void PacketRendererGLWidget::readEdgeIntensities(){

    QFile file(":/arclengths.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }

    QTextStream instream(&file);
    QString arcLength;

    while( (arcLength = instream.readLine()) != NULL){

        edgeIntensities << arcLength.toFloat();

    }
    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    //find min-max values
    for( int i = 0; i < edgeIntensities.length(); i++){

        if( edgeIntensities[i] < min)
            min = edgeIntensities[i];
        if( edgeIntensities[i] > max)
            max = edgeIntensities[i];
    }
    std::cout << "max: " << max << std::endl;
    std::cout << "min: " << min << std::endl;

    //normalize Edge intensities
    for( int i = 0; i < edgeIntensities.length(); i++){

        edgeIntensities[i] = (edgeIntensities[i] - min)/(max - min);
    }

    for(int i=0; i< pairs.length(); i++){

        if(edgeIntensities[i] > 0.5){

            EdgePos << QVector4D(fileVertexPos.at(pairs.at(i).x()),1.0) <<  QVector4D(fileVertexPos.at(pairs.at(i).y()),1.0);

            edgeColors << QVector4D(edgeIntensities[i], 1-edgeIntensities[i], 0, 1.0) << QVector4D(edgeIntensities[i], 1-edgeIntensities[i], 0, 1.0);
        }
    }
}
