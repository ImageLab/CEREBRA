#include "packetrendererglwidget.h"
#include <limits>

//for a cube we need 6 (face) * 6 (num of vertices
//used for each face) vertex connections
#define NUM_OF_CON_POINTS_FOR_EACH_MESH 36
#define NUM_OF_VERTICES 8
#define INTERPOLATION_LEVEL 12
#define UPDATE_FREQ_IN_MS 85
#define ANIMATION_BUFFER_SIZE 0

PacketRendererGLWidget::PacketRendererGLWidget( QWidget *parent) : QGLWidget( parent)
{
    projection.setToIdentity();
    modelView.setToIdentity();
    translationMatrix.setToIdentity();

    alpha = 0;
    beta = 0;
    distance = 60;
    moveX = 0;
    moveY = 0;

    aTimer = new QTimer;
    connect(aTimer,SIGNAL(timeout()),SLOT(animate()));
    shouldAnimate = true;

    voxelMinValue = 0.0;
    voxelMaxValue = 1.0;
    pairsMinValue = 0.0;
    pairsMaxValue = 1.0;
    displayArcs = false;
}

PacketRendererGLWidget::~PacketRendererGLWidget(){

    delete packetToRender;
    delete aTimer;
}

QSize PacketRendererGLWidget::sizeHint() const{

    return QSize( 1024, 768);
}

void PacketRendererGLWidget::initializeGL(){

    initializeOpenGLFunctions();

    projection.setToIdentity();
    modelView.setToIdentity();
    translationMatrix.setToIdentity();
    textureOffset = 0;

    //gl attributes
    qglClearColor( QColor( Qt::black));

    glEnable( GL_DEPTH_TEST);
    glEnable( GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initializeShader();
}

void PacketRendererGLWidget::initializeShader(){

    if( shaderProgram.isLinked()){
        shaderProgram.disableAttributeArray( "vPosition");
        shaderProgram.disableAttributeArray( "vIndex");
        shaderProgram.release();
        shaderProgram.removeAllShaders();
    }

    QString vertexShaderSource = "#version 430\n"

                                "in vec4 vPosition;\n"
                                "in float vIndex;\n"

                                "uniform int interpolationLevel;\n"
                                "uniform int timeLength;\n"
                                "uniform float minValue;\n"
                                "uniform float maxValue;\n"
                                "uniform float minThreshold;\n"
                                "uniform float maxThreshold;\n"
                                "uniform bool shouldDrawTransparent;\n"
                                "uniform bool drawsEdges;\n"
                                "uniform int textureOffset;\n"
                                "uniform int interpolationOffset;\n"
                                "uniform mat4 modelView;\n"
                                "uniform mat4 projection;\n"
                                "uniform mat4 translationMatrix;\n"
                                "uniform samplerBuffer u_tbo_tex;\n"

                                "out vec4 color;\n"

                                "void main(void){\n"
                                    "float curIntensity = texelFetch(u_tbo_tex, int(vIndex) + textureOffset).r;\n"
                                    "float nextIntensity = texelFetch(u_tbo_tex, int(vIndex + mod(textureOffset+1, timeLength))).r;\n"
                                    "float intensity = curIntensity + interpolationOffset * ((nextIntensity - curIntensity)/(interpolationLevel));\n"
                                    "float r = (intensity-minValue)/(maxValue - minValue);\n"
                                    "if(intensity <= maxThreshold && intensity >= minThreshold)\n"
                                        "if( !drawsEdges && shouldDrawTransparent)\n"
                                            "color = vec4(0.83, 0.83, 0.83, 0.05);\n"
                                        "else\n"
                                            "color = vec4(0,0,0,0);\n"
                                    "else\n"
                                        "if( !shouldDrawTransparent)\n"
                                            "color = vec4(r, 1-r, 0, 1.0);\n"
                                        "else\n"
                                            "color = vec4(0,0,0,0);\n"
                                    "gl_Position = projection * translationMatrix * modelView * vPosition;\n"
                                "}";

    QString fragmentShaderSource = "#version 430\n"

                                   "in vec4 color;\n"

                                   "out vec4 fColor;\n"
                                   "void main(void){\n"
                                        "if( color.x + color.y < 0.5)\n"
                                            "discard;\n"

                                        "fColor = color;\n"
                                   "}";

    shaderProgram.addShaderFromSourceCode(QGLShader::Vertex, vertexShaderSource);
    shaderProgram.addShaderFromSourceCode(QGLShader::Fragment, fragmentShaderSource);

    shaderProgram.link();
    shaderProgram.bind();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "vIndex");
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

    createVoxelTexture();

    textureOffset = 0;
    interpolationOffset = 0;
    updateAttributeArrays();
    updateMatrices();

    shaderProgram.setUniformValue( "textureOffset", textureOffset);
    shaderProgram.setUniformValue( "interpolationOffset", interpolationOffset);
    shaderProgram.setUniformValue( "interpolationLevel", INTERPOLATION_LEVEL);
    shaderProgram.setUniformValue( "timeLength", packetToRender->intensities[0].size());

    if( shouldAnimate)
        aTimer->start(UPDATE_FREQ_IN_MS); //updating per this amount of milliseconds
}

void PacketRendererGLWidget::paintGL(){

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.setUniformValue( "shouldDrawTransparent", false);
    shaderProgram.setUniformValue( "drawsEdges", true);

    if( edgeVertices.size() > 0 && displayArcs){
        shaderProgram.setUniformValue( "minThreshold", pairsMinThreshold);
        shaderProgram.setUniformValue( "maxThreshold", pairsMaxThreshold);
        shaderProgram.setUniformValue( "minValue", pairsMinValue);
        shaderProgram.setUniformValue( "maxValue", pairsMaxValue);
        shaderProgram.setAttributeArray( "vPosition", edgeVertices.constData());
        shaderProgram.setAttributeArray( "vIndex", edgeTextureIndex.constData(), 1);
        glDrawArrays(GL_LINES, 0, edgeVertices.size());
    }

    shaderProgram.setUniformValue( "drawsEdges", false);
    shaderProgram.setUniformValue( "minThreshold", voxelMinThreshold);
    shaderProgram.setUniformValue( "maxThreshold", voxelMaxThreshold);
    shaderProgram.setUniformValue( "minValue", voxelMinValue);
    shaderProgram.setUniformValue( "maxValue", voxelMaxValue);

    if( edgeVertices.size() > 0 && displayArcs){
        shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
        shaderProgram.setAttributeArray( "vIndex", voxelTextureIndex.constData(), 1);
    }

    glDrawElements( GL_TRIANGLES, voxelIndices.size(), GL_UNSIGNED_INT, nullptr);

    glDepthMask(GL_FALSE);
    shaderProgram.setUniformValue( "shouldDrawTransparent", true);
    glDrawElements( GL_TRIANGLES, voxelIndices.size(), GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
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

    if( event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton)
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
    }else if( event->buttons() & Qt::MiddleButton){

        moveX += 0.3*(event->x() - lastMousePosition.x());
        moveY += 0.3*(event->y() - lastMousePosition.y());

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
    translationMatrix.setToIdentity();

    translationMatrix.translate(moveX, -1*moveY);

    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(alpha, 0, 0, 1);
    cameraTransformation.rotate(beta, 1, 0, 0);
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
    modelView.lookAt( cameraPosition, QVector3D(0,0,0), cameraUpDirection);

    shaderProgram.setUniformValue("projection", projection);
    shaderProgram.setUniformValue("modelView", modelView);
    shaderProgram.setUniformValue("translationMatrix", translationMatrix);
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
        int curVoxelTextureBegin = currentVoxel*packetToRender->intensities[currentVoxel].size();
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

        voxelTextureIndex << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin
                          << curVoxelTextureBegin;

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

    edgeVertices.clear();
    edgeTextureIndex.clear();

    int edgeTextureBegin = (int)packetToRender->vXYZ.size() * packetToRender->intensities[0].size();
    for( int pair = 0; pair < (int)packetToRender->edges.size(); pair++){

        int curEdgeTextureBegin = edgeTextureBegin + pair*packetToRender->edgeIntensities[pair].size();
        QVector3D origin(packetToRender->vXYZ[packetToRender->edges[pair].x].x, packetToRender->vXYZ[packetToRender->edges[pair].x].y, packetToRender->vXYZ[packetToRender->edges[pair].x].z);
        QVector3D destination(packetToRender->vXYZ[packetToRender->edges[pair].y].x, packetToRender->vXYZ[packetToRender->edges[pair].y].y, packetToRender->vXYZ[packetToRender->edges[pair].y].z);

        edgeVertices << QVector4D( origin, 1.0) //0
                     << QVector4D( destination, 1.0); //1

        edgeTextureIndex << curEdgeTextureBegin
                         << curEdgeTextureBegin;
    }

    initializeShader();

    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
    shaderProgram.setAttributeArray( "vIndex", voxelTextureIndex.constData(), 1);
}

void PacketRendererGLWidget::createVoxelTexture(){

    const int totalVoxel = packetToRender->intensities.size();
    const int totalTime = packetToRender->intensities[0].size();
    const int totalPairs = packetToRender->edgeIntensities.size();
    const int totalVoxelIntensityValues = totalVoxel*totalTime;
    const int totalPairIntensityValues = totalPairs*totalTime;

    float *voxelBOData = new float[totalVoxelIntensityValues + totalPairIntensityValues];

    for( int curVoxel = 0; curVoxel < totalVoxel; curVoxel++)
        for( int curTime = 0; curTime < totalTime; curTime++)
            voxelBOData[curVoxel*totalTime + curTime] = packetToRender->intensities[curVoxel][curTime];

    for( int curPair = 0; curPair < totalPairs; curPair++)
        for( int curTime = 0; curTime < totalTime; curTime++)
            voxelBOData[totalVoxelIntensityValues + curPair*totalTime + curTime] = packetToRender->edgeIntensities[curPair][curTime];

    glGenBuffers( 1, &voxelBO);
    glBindBuffer( GL_TEXTURE_BUFFER, voxelBO);
    glBufferData( GL_TEXTURE_BUFFER, sizeof(float)*(totalVoxelIntensityValues+totalPairIntensityValues), voxelBOData, GL_STATIC_DRAW);

    glGenTextures( 1, &voxelTBO);
    glBindTexture( GL_TEXTURE_BUFFER, voxelTBO);
    glTexBuffer( GL_TEXTURE_BUFFER, GL_R32F, voxelBO);
    shaderProgram.setUniformValue("u_tbo_tex", 0);
}

void PacketRendererGLWidget::animate(){

    if( packetToRender == NULL || packetToRender->intensities.size() < 1)
        return;

    interpolationOffset = (interpolationOffset+1)%INTERPOLATION_LEVEL;
    if( interpolationOffset == 0){

        textureOffset = (textureOffset+1)%packetToRender->intensities[0].size();
        shaderProgram.setUniformValue( "textureOffset", textureOffset);
    }

    shaderProgram.setUniformValue( "interpolationOffset", interpolationOffset);

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

void PacketRendererGLWidget::setThresholdRange( float minThreshold, float maxThreshold){

    this->voxelMinThreshold = minThreshold;
    this->voxelMaxThreshold = maxThreshold;
}

void PacketRendererGLWidget::setPairsThresholdRange( float minThreshold, float maxThreshold){

    this->pairsMinThreshold = minThreshold;
    this->pairsMaxThreshold = maxThreshold;
}

void PacketRendererGLWidget::setVoxelMinValue( float minValue){

    this->voxelMinValue = minValue;
}

void PacketRendererGLWidget::setVoxelMaxValue( float maxValue){

    this->voxelMaxValue = maxValue;
}

void PacketRendererGLWidget::setPairsMinValue( float minValue){

    this->pairsMinValue = minValue;
}

void PacketRendererGLWidget::setPairsMaxValue( float maxValue){

    this->pairsMaxValue = maxValue;
}

void PacketRendererGLWidget::shouldDisplayArcs( bool shouldDisplay){

    this->displayArcs = shouldDisplay;
}
