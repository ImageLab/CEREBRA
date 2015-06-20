#include "packetfilereader.h"
#include "mainwindow.h"
#include "matio.h"
#include <QMainWindow>

PacketFileReader::PacketFileReader()
{

}

PacketFileReader::~PacketFileReader()
{

}

Packet *PacketFileReader::readPacketFromDirectory(QString directory){

    Packet *packet = new Packet();

    readVoxelLocations( directory, packet);
    readVoxelIntensities( directory, packet);
    readEdgePairs( directory, packet);
    readEdgePairIntensities( directory, packet);

    return packet;
}

Packet *PacketFileReader::readPacketFromMatlab(QString fileLocation,
                                               QString voxelPosVariable,
                                               QString voxelIntensitiesVariable,
                                               QString edgePairsVariable,
                                               QString edgePairsIntensitiesVariable){

    Packet *packet = new Packet();

    if( !voxelPosVariable.length())
        return packet;

    readMatVoxelLocations( fileLocation, voxelPosVariable, packet);
    readMatVoxelIntensities( fileLocation, voxelIntensitiesVariable, packet);
    readMatEdgePairs( fileLocation, edgePairsVariable, packet);
    readMatEdgePairIntensities( fileLocation, edgePairsIntensitiesVariable, packet);

    return packet;
}

void PacketFileReader::readMatVoxelLocations( QString fileName, QString voxelPosVariable, Packet *packet){

    if( !voxelPosVariable.length())
        return;

    mat_t    *matfp;
    matvar_t *matvar;

    matfp = Mat_Open( fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if ( NULL == matfp) {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }

    matvar = Mat_VarRead( matfp, voxelPosVariable.toStdString().c_str());
    if ( NULL != matvar ){

        Mat_VarReadDataAll( matfp, matvar);

        double *data = (double*)matvar->data;

        packet->vXYZ.clear();

        packet->vXYZ.resize(matvar->dims[0]);

        for( int row = 0; row < (int)matvar->dims[0]; row++)
                packet->vXYZ[row] = libsimple::Packet::Point3D( data[row],
                                                                data[matvar->dims[0] + row],
                                                                data[matvar->dims[0] * 2 + row]);

        Mat_VarFree( matvar);
    }

    Mat_Close(matfp);
}

void PacketFileReader::readMatVoxelIntensities( QString fileName, QString voxelIntensitiesVariable, Packet *packet){

    std::cout << "voxel inten funcs" << std::endl;

    if( !voxelIntensitiesVariable.length())
        return;

    mat_t    *matfp;
    matvar_t *matvar;

    matfp = Mat_Open( fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if ( NULL == matfp) {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }

    matvar = Mat_VarReadInfo( matfp, voxelIntensitiesVariable.toStdString().c_str());

    if ( NULL != matvar ){

        Mat_VarReadDataAll( matfp, matvar);
        std::cout << "voxel inten" << std::endl;

        double *data = (double*)matvar->data;

        packet->intensities.clear();
        packet->intensities.resize(matvar->dims[0]);
        for( int row = 0; row < (int)matvar->dims[0]; row++){
            packet->intensities[row] = vector<float>();
            packet->intensities[row].resize(matvar->dims[1]);
            for( int column = 0; column < (int)matvar->dims[1]; column++)
                packet->intensities[row][column] = data[matvar->dims[0] * column + row];
        }

        Mat_VarFree( matvar);
    }

    Mat_Close(matfp);
}

void PacketFileReader::readMatEdgePairs( QString fileName, QString edgePairsVariable, Packet *packet){

    if( !edgePairsVariable.length())
        return;

    std::cout << "asdasd" << std::endl;

    return;

//    MATFile *mFile = NULL;
//    mFile = matOpen(edgePairsFile.toStdString().c_str(),"r");
//    if(mFile == NULL)
//        cout << "error opening MAT file: " << endl;

//    const mxArray *pa = NULL;
//    pa = matGetVariable(mFile, edgePairsVariable.toStdString().c_str());

//    if (pa == NULL)
//        printf("Error reading existing matrix %s\n");
//    size_t rowSize = mxGetM(pa);
//    packet->edges.clear();

//    for(size_t i=0; i < rowSize; i++){
//         mxArray *cellArray = NULL;
//         cellArray = mxGetCell_730(pa,i);
//         size_t cellSize = mxGetN(cellArray);
//         double *mxData = (double*)mxGetData( cellArray);
//         for(int k = 0; k < (int)cellSize; k++)
//            packet->edges.push_back( libsimple::Packet::Point2D((float)i, (float)mxData[k]-1));
//    }

//    mat_t    *matfp;
//    matvar_t *matvar;

//    matfp = Mat_Open( fileName.toStdString().c_str(), MAT_ACC_RDONLY);

//    if ( NULL == matfp) {
//        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
//        return;
//    }

//    //matvar = Mat_VarReadInfo( matfp, edgePairsVariable.toStdString().c_str());

//    if ( NULL != matvar ){

//        //Mat_VarReadDataAll( matfp, matvar);
////        Mat_VarRead
//        //std::cout << matvar->dims[0] << " " << matvar->dims[1] << std::endl;
//    }

//    Mat_Close(matfp);
}

void PacketFileReader::readMatEdgePairIntensities( QString fileName, QString edgePairsIntensitiesVariable, Packet *packet){

    if( !edgePairsIntensitiesVariable.length())
        return;

//    MATFile *mFile = NULL;
//    mFile = matOpen(edgePairIntensitiesFile.toStdString().c_str(),"r");
//    if(mFile == NULL)
//        cout << "error opening MAT file: " << endl;

//    const mxArray *pa = NULL;
//    pa = matGetVariable(mFile, edgePairsIntensitiesVariable.toStdString().c_str());

//    if (pa == NULL)
//        printf("Error reading existing matrix %s\n");

//    const size_t *dims = mxGetDimensions_730( pa);
//    double *mxData = (double*)mxGetData( pa);
//    packet->edgeIntensities.clear();

//    for( int row = 0; row < (int)dims[0]; row++){

//        packet->edgeIntensities.push_back( vector<float>());
//        for( int column = 0; column < (int)dims[1]; column++)
//            packet->edgeIntensities.at(packet->edgeIntensities.size()-1).push_back(mxData[dims[0] * column + row]);
//    }
}

void PacketFileReader::readVoxelLocations( QString directory, Packet *packet){

    //here we assume that the file contains
    //2 consecutive lines. in the first line
    //there is position information (x y z)
    //and in the following line there is the
    //intensity value of that voxel.
    //it is just a parser to train the rendering.
    QFile voxelLocationFile(directory + QDir::separator() + "voxels.txt");

    if(!voxelLocationFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << voxelLocationFile.error();
        return;
    }

    packet->vXYZ.clear();
    QTextStream instream(&voxelLocationFile);
    QString pos;

    while( (pos = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = pos.split(rx, QString::SkipEmptyParts);
        packet->vXYZ.push_back( libsimple::Packet::Point3D(list.at(0).toFloat(), list.at(1).toFloat(), list.at(2).toFloat()));
    }

    voxelLocationFile.close();
}

void PacketFileReader::readVoxelIntensities( QString directory, Packet *packet){

    QFile voxelIntensityFile(directory + QDir::separator() + "intensities.txt");

    if(!voxelIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << voxelIntensityFile.error();
        return;
    }

    QTextStream instream(&voxelIntensityFile);
    QString intensitiesOfVoxel;

    packet->intensities.clear();

    while( (intensitiesOfVoxel = instream.readLine()) != NULL){

        QRegExp rx("[,]");
        QStringList list = intensitiesOfVoxel.split(rx, QString::SkipEmptyParts);
        packet->intensities.push_back( vector<float>());

        for( int i = 0; i < list.length(); i++)
            packet->intensities.at(packet->intensities.size()-1).push_back(list.at(i).toFloat());
    }
}

void PacketFileReader::readEdgePairs( QString directory, Packet *packet){

    QFile edgePairFile(directory + QDir::separator() + "edgeConnections.txt");

    if(!edgePairFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << edgePairFile.error();
        return;
    }

    packet->edges.clear();
    QTextStream instream(&edgePairFile);
    QString edgePairLine;
    int count = 0;

    while( (edgePairLine = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = edgePairLine.split(rx, QString::SkipEmptyParts);
        for(int i=0; i< list.length();i++)
            packet->edges.push_back( libsimple::Packet::Point2D((float)count, (float)list[i].toInt()-1));

        count++;
    }

    edgePairFile.close();
}

void PacketFileReader::readEdgePairIntensities( QString directory, Packet *packet){

    QFile contEdgeIntensityFile( directory + QDir::separator() + "edgeIntensities.txt");

    if(!contEdgeIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << contEdgeIntensityFile.error();
        return;
    }

    packet->edgeIntensities.clear();
    QTextStream instream(&contEdgeIntensityFile);
    QString intensityOfPair;

    while( (intensityOfPair = instream.readLine()) != NULL){

        QRegExp rx("[,]");
        QStringList list = intensityOfPair.split(rx, QString::SkipEmptyParts);

        packet->edgeIntensities.push_back( vector<float>());
        for( int i = 0; i < list.length(); i++)
            packet->edgeIntensities.at(packet->edgeIntensities.size()-1).push_back(list.at(i).toFloat());
    }
}
