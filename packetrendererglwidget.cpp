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
    displayLabels = false;
    pairsMinThreshold = INT_MAX;
    pairsMaxThreshold = INT_MAX;
    voxelMinThreshold = INT_MAX;
    voxelMaxThreshold = INT_MAX;

    textureOffset = 0;
    interpolationOffset = 0;
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
        shaderProgram.disableAttributeArray( "label");
        shaderProgram.release();
        shaderProgram.removeAllShaders();
    }

    QString vertexShaderSource = "#version 430\n"

                                "in vec4 vPosition;\n"
                                "in float vIndex;\n"
                                "in float label;\n"

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
                                "uniform bool isClusteringEnabled;\n"
                                "uniform vec4 curLabelColor;\n"
                                "uniform int curLabel;\n"
                                "uniform bool isCurLabelEnabled;\n"

                                "out vec4 color;\n"

                                "void main(void){\n"

                                    "if( isClusteringEnabled){\n"
                                        "if( int(label) == curLabel)\n"
                                            "if( isCurLabelEnabled)\n"
                                                "color = curLabelColor;\n"
                                            "else\n"
                                                "color = vec4(0.83, 0.83, 0.83, 0.05);\n"
                                        "else\n"
                                           "color = vec4(-3,0,0,0);\n"
                                    "}else{\n"

                                        "float curIntensity = texelFetch(u_tbo_tex, int(vIndex) + textureOffset).r;\n"
                                        "float nextIntensity = texelFetch(u_tbo_tex, int(vIndex + mod(textureOffset+1, timeLength))).r;\n"
                                        "float intensity = curIntensity + interpolationOffset * ((nextIntensity - curIntensity)/(interpolationLevel));\n"
                                        "float r = (intensity-minValue)/(maxValue - minValue);\n"

                                        "if( timeLength == 0)\n"
                                            "color = vec4(1,1,1,1);\n"
                                        "else if(intensity <= maxThreshold && intensity >= minThreshold)\n"
                                            "if( !drawsEdges && shouldDrawTransparent)\n"
                                                "color = vec4( 0.83, 0.83, 0.83, 0.05);\n"
                                            "else\n"
                                                "color = vec4( -3,0,0,0);\n"
                                        "else\n"
                                            "if( !shouldDrawTransparent)\n"
                                                "color = vec4(r, 1-r, 0, 1.0);\n"
                                            "else\n"
                                                "color = vec4( -3,0,0,0);\n"
                                    "}\n"
                                    "gl_Position = projection * translationMatrix * modelView * vPosition;\n"
                                "}";

    QString fragmentShaderSource = "#version 430\n"

                                   "in vec4 color;\n"

                                   "out vec4 fColor;\n"
                                   "void main(void){\n"
                                        "if( color.x < -1)\n"
                                            "discard;\n"

                                        "fColor = color;\n"
                                   "}";

    shaderProgram.addShaderFromSourceCode( QGLShader::Vertex, vertexShaderSource);
    shaderProgram.addShaderFromSourceCode( QGLShader::Fragment, fragmentShaderSource);

    shaderProgram.link();
    shaderProgram.bind();

    shaderProgram.enableAttributeArray( "vPosition");
    shaderProgram.enableAttributeArray( "vIndex");
    shaderProgram.enableAttributeArray( "label");
}

void PacketRendererGLWidget::setPacket( Packet *packet, QString workingDirectory){

    if( aTimer->isActive())
        aTimer->stop();

    if( packet->voxel3DPositions.size() < 1)
        return;

    packetToRender = packet;
    this->workingDirectory = workingDirectory;

    //if no intensity available, then create fake one with all intensities = 1;
    if( packetToRender->intensities.size() < 1){

        packetToRender->intensities.clear();
        packetToRender->intensities.resize( (size_t)packetToRender->voxel3DPositions.size());

        for( int voxel = 0; voxel < (int)packetToRender->voxel3DPositions.size(); voxel++)
            packetToRender->intensities[voxel].push_back((float)1.0);
    }

    createVoxelTexture();

    textureOffset = 0;
    interpolationOffset = 0;

    updateAttributeArrays();
    updateMatrices();

    if( shouldAnimate)
        aTimer->start( UPDATE_FREQ_IN_MS); //updating per this amount of milliseconds
}

void PacketRendererGLWidget::paintGL(){

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.setUniformValue( "projection", projection);
    shaderProgram.setUniformValue( "modelView", modelView);
    shaderProgram.setUniformValue( "translationMatrix", translationMatrix);
    shaderProgram.setUniformValue( "shouldDrawTransparent", false);
    shaderProgram.setUniformValue( "textureOffset", textureOffset);
    shaderProgram.setUniformValue( "interpolationOffset", interpolationOffset);
    shaderProgram.setUniformValue( "interpolationLevel", INTERPOLATION_LEVEL);
    shaderProgram.setUniformValue( "timeLength", (packetToRender && packetToRender->intensities.size() > 0)?(GLint)packetToRender->intensities[0].size():0);

    //major slow down occurs here. fps decreases because for each drawing,
    //there occurs a data transger (may be huge one). Therefore, to draw each
    //frame (both edge and voxels at the same time), gpu needs to first
    //transfer the edge data and display it, then transfer the voxel data
    //to the gpu again to display voxels.
    if( edgeVertices.size() > 0 && displayArcs){

        shaderProgram.setUniformValue( "drawsEdges", true);
        shaderProgram.setUniformValue( "isClusteringEnabled", false);
        shaderProgram.setUniformValue( "minThreshold", pairsMinThreshold);
        shaderProgram.setUniformValue( "maxThreshold", pairsMaxThreshold);
        shaderProgram.setUniformValue( "minValue", pairsMinValue);
        shaderProgram.setUniformValue( "maxValue", pairsMaxValue);

        shaderProgram.setAttributeArray( "vPosition", edgeVertices.constData());
        if( edgeTextureIndex.size() != edgeVertices.size())edgeTextureIndex.resize(edgeVertices.size());
        shaderProgram.setAttributeArray( "vIndex", edgeTextureIndex.constData(),1);
        if( edgeLabels.size() != edgeVertices.size()) edgeLabels.resize( edgeVertices.size());
        shaderProgram.setAttributeArray( "label", edgeLabels.constData(),1);

        glDrawArrays( GL_LINES, 0, edgeVertices.size());

        shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
        if( voxelTextureIndex.size() != voxelVertices.size()) voxelTextureIndex.resize( voxelVertices.size());
        shaderProgram.setAttributeArray( "vIndex", voxelTextureIndex.constData(),1);
        if( labels.size() != voxelVertices.size()) labels.resize( voxelVertices.size());
        shaderProgram.setAttributeArray( "label", labels.constData(),1);
    }

    if( voxelTextureIndex.size() != voxelVertices.size()) voxelTextureIndex.resize( voxelVertices.size());
    shaderProgram.setAttributeArray( "vIndex", voxelTextureIndex.constData(),1);
    if( labels.size() != voxelVertices.size()) labels.resize( voxelVertices.size());
    shaderProgram.setAttributeArray( "label", labels.constData(),1);

    shaderProgram.setUniformValue( "isClusteringEnabled", displayLabels);
    shaderProgram.setUniformValue( "drawsEdges", false);
    shaderProgram.setUniformValue( "minThreshold", voxelMinThreshold);
    shaderProgram.setUniformValue( "maxThreshold", voxelMaxThreshold);
    shaderProgram.setUniformValue( "minValue", voxelMinValue);
    shaderProgram.setUniformValue( "maxValue", voxelMaxValue);

    if( displayLabels){

        for( std::map< int, bool>::iterator iter = labelActivations.begin(); iter != labelActivations.end(); ++iter){
            if( labelActivations[iter->first]){

                shaderProgram.setUniformValue( "curLabel", iter->first);
                shaderProgram.setUniformValue( "isCurLabelEnabled", true);
                QVector4D color = QVector4D( ((float)colorsOfLabels[iter->first].first/255),
                                             ((float)colorsOfLabels[iter->first].second.first/255),
                                             ((float)colorsOfLabels[iter->first].second.second/255), 1.0);
                shaderProgram.setUniformValue( "curLabelColor", color);
                glDrawElements( GL_TRIANGLES, (GLsizei)voxelIndices.size(), GL_UNSIGNED_INT, nullptr);
            }
        }
    } else
        glDrawElements( GL_TRIANGLES, (GLsizei)voxelIndices.size(), GL_UNSIGNED_INT, nullptr);

    glDepthMask(GL_FALSE);
    shaderProgram.setUniformValue( "shouldDrawTransparent", true);

    if( displayLabels){
        for( std::map< int, bool>::iterator iter = labelActivations.begin(); iter != labelActivations.end(); ++iter){
            if( !labelActivations[iter->first]){
                shaderProgram.setUniformValue( "curLabel", iter->first);
                shaderProgram.setUniformValue( "isCurLabelEnabled", false);
                glDrawElements( GL_TRIANGLES, (GLsizei)voxelIndices.size(), GL_UNSIGNED_INT, nullptr);
            }
        }
    } else
        glDrawElements( GL_TRIANGLES, (GLsizei)voxelIndices.size(), GL_UNSIGNED_INT, nullptr);

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

    updateGL();
}

void PacketRendererGLWidget::updateAttributeArrays(){

    labels.clear();
    edgeLabels.clear();
    voxelVertices.clear();
    voxelTextureIndex.clear();
    voxelIndices.clear();
    edgeVertices.clear();
    edgeTextureIndex.clear();

    voxelIndices.resize( (int)packetToRender->voxel3DPositions.size() * NUM_OF_CON_POINTS_FOR_EACH_MESH);
    int voxelIndexCount = 0;
    for( int currentVoxel = 0; currentVoxel < (int)packetToRender->voxel3DPositions.size(); currentVoxel++){

        int curVoxelIndexBegin = NUM_OF_VERTICES * currentVoxel;
        int curVoxelTextureBegin = currentVoxel*(int)packetToRender->intensities[currentVoxel].size();
        float x = packetToRender->voxel3DPositions[currentVoxel].first;
        float y = packetToRender->voxel3DPositions[currentVoxel].second.first;
        float z = packetToRender->voxel3DPositions[currentVoxel].second.second;

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

    glGenBuffers( 1, &voxelIBO);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, voxelIBO);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, voxelIndices.size()*sizeof(unsigned int), &(voxelIndices.front()), GL_STATIC_DRAW);

    int edgeTextureBegin = (int)packetToRender->voxel3DPositions.size() * (int)packetToRender->intensities[0].size();

    for( int pair = 0; pair < (int)packetToRender->edges.size(); pair++){

        int curEdgeTextureBegin = edgeTextureBegin + pair*(int)packetToRender->edgeIntensities[pair].size();

        QVector3D origin(packetToRender->voxel3DPositions[packetToRender->edges[pair].first].first,
                packetToRender->voxel3DPositions[packetToRender->edges[pair].first].second.first,
                packetToRender->voxel3DPositions[packetToRender->edges[pair].first].second.second);

        QVector3D destination(packetToRender->voxel3DPositions[packetToRender->edges[pair].second].first,
                packetToRender->voxel3DPositions[packetToRender->edges[pair].second].second.first,
                packetToRender->voxel3DPositions[packetToRender->edges[pair].second].second.second);

        edgeVertices << QVector4D( origin, 1.0) //0
                     << QVector4D( destination, 1.0); //1

        edgeTextureIndex << curEdgeTextureBegin
                         << curEdgeTextureBegin;
    }

    initializeShader();

    shaderProgram.setAttributeArray( "vPosition", voxelVertices.constData());
    if( voxelTextureIndex.size() != voxelVertices.size()) voxelTextureIndex.resize( voxelVertices.size());
    shaderProgram.setAttributeArray( "vIndex", voxelTextureIndex.constData(),1);
    if( labels.size() != voxelVertices.size()) labels.resize( voxelVertices.size());
    shaderProgram.setAttributeArray( "label", labels.constData(),1);
}

void PacketRendererGLWidget::createVoxelTexture(){

    const int totalVoxel = (int)packetToRender->intensities.size();
    const int totalTime = (int)packetToRender->intensities[0].size();
    const int totalPairs = (int)packetToRender->edgeIntensities.size();
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
    shaderProgram.setUniformValue( "u_tbo_tex", 0);

    delete[] voxelBOData;
}

void PacketRendererGLWidget::animate(){

    if( packetToRender == NULL || packetToRender->intensities.size() < 1)
        return;

    if( displayLabels){

        updateGL();
        return;
    }

    interpolationOffset = (interpolationOffset+1)%INTERPOLATION_LEVEL;
    if( interpolationOffset == 0){

        textureOffset = (textureOffset+1)%packetToRender->intensities[0].size();
    }

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

void PacketRendererGLWidget::labelEnabled( int label, int r, int g, int b){

    if( labelActivations.find(label) != labelActivations.end()){

        labelActivations[label] = true;
        setRGBForALabel( label, r, g, b);
    }
}

void PacketRendererGLWidget::labelEnabled( int label){

    if( labelActivations.find(label) != labelActivations.end())
        labelActivations[label] = true;
}

void PacketRendererGLWidget::labelDisabled( int label){

    if( labelActivations.find(label) != labelActivations.end())
        labelActivations[label] = false;
}

void PacketRendererGLWidget::setLabels( std::vector<int> &voxelLabels){

    labelActivations.clear();
    labels.clear();
    colorsOfLabels.clear();

    for( int curLabel = 0; curLabel < voxelLabels.size(); curLabel++){

        displayLabels = true;
        labelActivations[voxelLabels[curLabel]] = false;

        labels << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel]
               << (GLfloat)voxelLabels[curLabel];

        if( colorsOfLabels.find(voxelLabels[curLabel]) == colorsOfLabels.end()){

            colorsOfLabels[voxelLabels[curLabel]].first = rand() % 256;
            colorsOfLabels[voxelLabels[curLabel]].second.first = rand() % 256;
            colorsOfLabels[voxelLabels[curLabel]].second.second = rand() % 256;
        }
    }
}

void PacketRendererGLWidget::disableClusteringDisplay(){

    displayLabels = false;

    labelActivations.clear();
    colorsOfLabels.clear();
}

void PacketRendererGLWidget::getRGBOfALabel( int label, int &r, int &g, int &b){

    if( colorsOfLabels.find(label) != colorsOfLabels.end()){

        r = colorsOfLabels[label].first;
        g = colorsOfLabels[label].second.first;
        b = colorsOfLabels[label].second.second;
    }
}

void PacketRendererGLWidget::setRGBForALabel( int label, int r, int g, int b){

    if( colorsOfLabels.find(label) != colorsOfLabels.end()){

        colorsOfLabels[label].first = r;
        colorsOfLabels[label].second.first = g;
        colorsOfLabels[label].second.second = b;
    }
}
