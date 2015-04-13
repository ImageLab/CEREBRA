#include "packetrendererglwidget.h"
#include <limits>

//for a cube we need 6 (face) * 6 (num of vertices
//used for each face) vertex connections
#define NUM_OF_CON_POINTS_FOR_EACH_MESH 36
#define NUM_OF_VERTICES 8
#define INTERPOLATION_LEVEL 6
#define UPDATE_FREQ_IN_MS 85

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{
    projection.setToIdentity();
    modelView.setToIdentity();

    alpha = 0;
    beta = 0;
    distance = 60;

    aTimer = new QTimer;
    connect(aTimer,SIGNAL(timeout()),SLOT(animate()));
    shouldAnimate = true;
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

    initializeOpenGLFunctions();

    projection.setToIdentity();
    modelView.setToIdentity();
    textureOffset = 0;

    //gl attributes
    glEnable( GL_DEPTH_TEST);
    qglClearColor( QColor( Qt::gray));

    initializeShader();
}

void PacketRendererGLWidget::paintGL(){

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelIBO);
//    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements( GL_TRIANGLES, voxelIndices.size(), GL_UNSIGNED_INT, nullptr);

//    shaderProgram.setAttributeArray( "vPosition", edgeVertices.constData());
//    shaderProgram.setAttributeArray( "vColor", colors.constData());
//    shaderProgram.setAttributeArray( "vTextureCoordinate", edgeTextureCoordinates.constData());

//    glDrawArrays( GL_LINES, 0, edgeVertices.size());

//    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
//    shaderProgram.setAttributeArray( "vColor", colors.constData());
//    shaderProgram.setAttributeArray( "vTextureCoordinate", voxelTextureCoordinates.constData());
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
    voxelTextureIndex.clear();
    voxelIndices.clear();
    voxelIndices.resize( (int)packetToRender->vXYZ.size() * NUM_OF_CON_POINTS_FOR_EACH_MESH);
    int voxelIndexCount = 0;
    for( int currentVoxel = 0; currentVoxel < (int)packetToRender->vXYZ.size(); currentVoxel++){

        int curVoxelIndexBegin = NUM_OF_VERTICES * currentVoxel;
        int curVoxelTextureBegin = currentVoxel*packetToRender->intensities[currentVoxel].size()*INTERPOLATION_LEVEL;
        float x = packetToRender->vXYZ[currentVoxel].x;
        float y = packetToRender->vXYZ[currentVoxel].y;
        float z = packetToRender->vXYZ[currentVoxel].z;

        voxelVertices << QVector4D(x-0.35, y-0.35, z+0.35, 1.0) //0
                      << QVector4D(x+0.35, y-0.35, z+0.35, 1.0) //1
                      << QVector4D(x+0.35, y+0.35, z+0.35, 1.0) //2
                      << QVector4D(x-0.35, y+0.35, z+0.35, 1.0) //3
                      << QVector4D(x-0.35, y-0.35, z-0.35, 1.0) //4
                      << QVector4D(x+0.35, y-0.35, z-0.35, 1.0) //5
                      << QVector4D(x+0.35, y+0.35, z-0.35, 1.0) //6
                      << QVector4D(x-0.35, y+0.35, z-0.35, 1.0); //7

        voxelTextureIndex << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0)
                          << QVector2D(curVoxelTextureBegin, 0);

        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 0;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 1;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 2; //front
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 2;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 3;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 0;
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 7;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 6;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 5; //back
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 5;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 4;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 7;
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 4;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 0;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 3; //left
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 3;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 7;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 4;
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 1;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 5;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 6; // Right
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 6;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 2;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 1;
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 3;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 2;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 6; // Top
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 6;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 7;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 3;
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 4;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 5;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 1; // Bottom
        voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 1;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 0;  voxelIndices[voxelIndexCount++] = curVoxelIndexBegin + 4;
    }

    glGenBuffers (1, &voxelIBO);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, voxelIBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, voxelIndices.size()*sizeof(unsigned int), &(voxelIndices.front()), GL_STATIC_DRAW);

//    for(int pair=0; pair < packetToRender->edges.size(); pair++){
//        if(packetToRender->edgeIntensities[pair][0] > 0.5){

//            QVector3D origin(packetToRender->vXYZ[packetToRender->edges[pair].x].x, packetToRender->vXYZ[packetToRender->edges[pair].x].y, packetToRender->vXYZ[packetToRender->edges[pair].x].z);
//            QVector3D destination(packetToRender->vXYZ[packetToRender->edges[pair].y].x, packetToRender->vXYZ[packetToRender->edges[pair].y].y, packetToRender->vXYZ[packetToRender->edges[pair].y].z);

//            edgeVertices << QVector4D( origin,1.0) <<  QVector4D( destination,1.0);
//            edgeTextureCoordinates <<  QVector2D(0, (float)(pair)/(float)(packetToRender->edges.size())) << QVector2D(0, (float)(pair)/(float)(packetToRender->edges.size()));
//        }
//    }

    initializeShader();

    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
    shaderProgram.setAttributeArray( "voxelIndex", voxelTextureIndex.constData());
}

void PacketRendererGLWidget::initializeShader(){

    if( shaderProgram.isLinked()){
        shaderProgram.disableAttributeArray( "vPosition");
        shaderProgram.disableAttributeArray( "voxelIndex");
        //shaderProgram.disableAttributeArray( "edgePairIndex");
        shaderProgram.release();
        shaderProgram.removeAllShaders();
    }

    QString vertexShaderSource = "#version 430\n"

                                "in vec4 vPosition;\n"
                                "in vec2 voxelIndex;\n"
                                "//in int edgePairIndex;\n"

                                "uniform int textureOffset;\n"
                                "uniform mat4 modelView;\n"
                                "uniform mat4 projection;\n"
                                "//uniform bool shouldUseVoxel;\n"
                                "uniform samplerBuffer u_tbo_tex;\n"

                                "out vec4 color;\n"

                                "void main(void){\n"

                                    "float r = texelFetch(u_tbo_tex, int(voxelIndex.x) + textureOffset).r;\n"
                                    "gl_Position = projection * modelView * vPosition;\n"
                                    "color = vec4(r, 1-r, 0.0, 1.0);\n"
                                "}";

    QString fragmentShaderSource = "#version 430\n"

                                   "uniform bool isTextured;\n"

                                   "in vec4 color;\n"

                                   "out vec4 fColor;\n"
                                   "void main(void){\n"
                                       "fColor = color;\n"
                                   "}";

    shaderProgram.addShaderFromSourceCode(QGLShader::Vertex, vertexShaderSource);
    shaderProgram.addShaderFromSourceCode(QGLShader::Fragment, fragmentShaderSource);

    shaderProgram.link();
    shaderProgram.bind();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "voxelIndex");
    //shaderProgram.enableAttributeArray( "edgePairIndex");
}

void PacketRendererGLWidget::setPacket(Packet *packet, QString workingDirectory){

    if( aTimer->isActive())
        aTimer->stop();

    if( packet->vXYZ.size() < 1)
        return;

    packetToRender = packet;
    this->workingDirectory = workingDirectory;

    //if no intensity available, then create fake one with all intensities = 1;
    if( packetToRender->intensities.size() < 1){

        packetToRender->intensities.clear();
        packetToRender->intensities.resize( (size_t)packetToRender->vXYZ.size());

        for( int voxel = 0; voxel < (int)packetToRender->vXYZ.size(); voxel++)
            packetToRender->intensities[voxel].push_back((float)1.0);
    }

    createVoxelTexture( packetToRender->intensities.size(),
                        packetToRender->intensities[0].size(),
                        INTERPOLATION_LEVEL);

    textureOffset = 0;
    shaderProgram.setUniformValue( "textureOffset", textureOffset);

    updateAttributeArrays();
    updateMatrices();

    if( shouldAnimate)
        aTimer->start(UPDATE_FREQ_IN_MS); //updating per this amount of milliseconds
}

void PacketRendererGLWidget::createVoxelTexture(const int row, const int column, const int interpolationLevel){

    float *voxelBOData = new float[row*column*interpolationLevel];

    for( int curRow = 0; curRow < row; curRow++){
        for( int curColumn = 0; curColumn < column-1; curColumn++){

            float nextIntensity = packetToRender->intensities[curRow][curColumn+1];
            float curIntensity = packetToRender->intensities[curRow][curColumn];
            float interpolation = (float)(nextIntensity - curIntensity)/(float)interpolationLevel;

            for( int curInterp = 0; curInterp < interpolationLevel; curInterp++){
                voxelBOData[curRow*column*interpolationLevel + curColumn*interpolationLevel + curInterp] = curIntensity + curInterp * interpolation;
                cout << voxelBOData[curRow*column*interpolationLevel + curColumn*interpolationLevel + curInterp] << " ";
            }
        }
        cout << endl;

        //last column
        int curColumn = column-1;
        float nextIntensity = packetToRender->intensities[curRow][0];
        float curIntensity = packetToRender->intensities[curRow][curColumn];
        float interpolation = (float)(curIntensity - nextIntensity)/(float)interpolationLevel;

        for( int curInterp = 0; curInterp < interpolationLevel; curInterp++)
            voxelBOData[curRow*column*interpolationLevel + curColumn*interpolationLevel + curInterp] = curIntensity + curInterp * interpolation;
    }

    glGenBuffers( 1, &voxelBO);
    glBindBuffer( GL_TEXTURE_BUFFER, voxelBO);
    glBufferData( GL_TEXTURE_BUFFER, sizeof(float)*row*column*interpolationLevel, voxelBOData, GL_STATIC_DRAW);

    glGenTextures( 1, &voxelTBO);
    glBindTexture( GL_TEXTURE_BUFFER, voxelTBO);
    glTexBuffer( GL_TEXTURE_BUFFER, GL_R32F, voxelBO);
    shaderProgram.setUniformValue("u_tbo_tex", 0);
}

void PacketRendererGLWidget::createEdgePairTexture(){

    QString edgeTextureName = "edgeTexture.jpeg";
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

    //createTexture( edgeTextureName, packetToRender->edgeIntensities);
}

void PacketRendererGLWidget::animate(){

    if( packetToRender == NULL || packetToRender->intensities.size() < 1)
        return;

    textureOffset++;

    if( textureOffset >= INTERPOLATION_LEVEL*packetToRender->intensities[0].size())
        textureOffset = 0;

    shaderProgram.setUniformValue( "textureOffset", textureOffset);
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
