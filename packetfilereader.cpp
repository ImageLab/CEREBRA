#include "packetfilereader.h"
#include "mainwindow.h"
#include "mat.h"
#include "matrix.h"
#include <QMainWindow>

#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libeng.lib")
#pragma comment(lib, "libmat.lib")


PacketFileReader::PacketFileReader()
{

}

PacketFileReader::~PacketFileReader()
{

}

Packet *PacketFileReader::readPacketFromDirectory(QString directory){

    QVector<QVector3D> voxelLocations = readVoxelLocations( directory);
    QVector<QVector<float>> voxelIntensities = readVoxelIntensities( directory);
    QVector<QVector2D> edgePairs = readEdgePairs( directory);
    QVector<QVector<float>> edgePairIntensities = readEdgePairIntensities( directory);

    Packet *packet = new Packet();

    //setting voxel locations and intensity values
    if( voxelLocations.length() > 0) {

        packet->vXYZ.clear();
        packet->vXYZ.resize( voxelLocations.length());
        packet->intensities.clear();
        if( voxelIntensities.length() > 0)
            packet->intensities.resize( voxelIntensities.length());

        for( int voxel = 0; voxel < voxelLocations.length(); voxel++){

            packet->vXYZ[voxel].x = voxelLocations[voxel].x();
            packet->vXYZ[voxel].y = voxelLocations[voxel].y();
            packet->vXYZ[voxel].z = voxelLocations[voxel].z();

            if( packet->intensities.size() > 0) {
                (packet->intensities[voxel]).clear();
                if( voxelIntensities[voxel].length() > 0)
                    (packet->intensities[voxel]).resize(voxelIntensities[voxel].length());

                for( int intensity = 0; intensity < voxelIntensities[voxel].length(); intensity++)
                    packet->intensities[voxel][intensity] = voxelIntensities[voxel][intensity];
            }
        }
    }

    //setting edge locations and intensity values
    if( edgePairs.length() > 0){

        packet->edges.clear();
        packet->edges.resize( (size_t)edgePairs.length());
        packet->edgeIntensities.clear();
        if( edgePairIntensities.length())
            packet->edgeIntensities.resize( (size_t)edgePairIntensities.length());

        for( int edgePair = 0; edgePair < edgePairs.length(); edgePair++){

            packet->edges[edgePair].x = edgePairs[edgePair].x();
            packet->edges[edgePair].y = edgePairs[edgePair].y();

            if( packet->edgeIntensities.size() > 0){

                (packet->edgeIntensities[edgePair]).clear();
                if( edgePairIntensities[edgePair].length() > 0)
                    (packet->edgeIntensities[edgePair]).resize( edgePairIntensities[edgePair].length());

                for( int edgePairIntensity = 0; edgePairIntensity < edgePairIntensities[edgePair].length(); edgePairIntensity++)
                    packet->edgeIntensities[edgePair][edgePairIntensity] = edgePairIntensities[edgePair][edgePairIntensity];
            }
        }
    }

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

    QVector<QVector3D> voxelLocations = readMatVoxelLocations( fileLocation, voxelPosVariable);
    QVector<QVector<float>> voxelIntensities = readMatVoxelIntensities( fileLocation, voxelIntensitiesVariable);
    QVector<QVector2D> edgePairs;
    QVector<QVector<float>> edgePairIntensities;

    if( edgePairsIntensitiesVariable.length()){
        edgePairs = readMatEdgePairs( fileLocation, edgePairsVariable);
        edgePairIntensities = readMatEdgePairIntensities( fileLocation, edgePairsIntensitiesVariable);
    }

    //setting voxel locations and intensity values
    if( voxelLocations.length() > 0) {

        packet->vXYZ.clear();
        packet->vXYZ.resize( voxelLocations.length());
        packet->intensities.clear();
        if( voxelIntensities.length() > 0)
            packet->intensities.resize( voxelIntensities.length());

        for( int voxel = 0; voxel < voxelLocations.length(); voxel++){

            packet->vXYZ[voxel].x = voxelLocations[voxel].x();
            packet->vXYZ[voxel].y = voxelLocations[voxel].y();
            packet->vXYZ[voxel].z = voxelLocations[voxel].z();

            if( packet->intensities.size() > 0) {
                (packet->intensities[voxel]).clear();
                if( voxelIntensities[voxel].length() > 0)
                    (packet->intensities[voxel]).resize(voxelIntensities[voxel].length());

                for( int intensity = 0; intensity < voxelIntensities[voxel].length(); intensity++){
                    packet->intensities[voxel][intensity] = voxelIntensities[voxel][intensity];
                }
            }
        }
    }

    //setting edge locations and intensity values
    if( edgePairs.length() > 0){

        packet->edges.clear();
        packet->edges.resize( (size_t)edgePairs.length());
        packet->edgeIntensities.clear();
        if( edgePairIntensities.length())
            packet->edgeIntensities.resize( (size_t)edgePairIntensities.length());

        for( int edgePair = 0; edgePair < edgePairs.length(); edgePair++){

            packet->edges[edgePair].x = edgePairs[edgePair].x();
            packet->edges[edgePair].y = edgePairs[edgePair].y();

            if( packet->edgeIntensities.size() > 0){

                (packet->edgeIntensities[edgePair]).clear();
                if( edgePairIntensities[edgePair].length() > 0)
                    (packet->edgeIntensities[edgePair]).resize( edgePairIntensities[edgePair].length());

                for( int edgePairIntensity = 0; edgePairIntensity < edgePairIntensities[edgePair].length(); edgePairIntensity++)
                    packet->edgeIntensities[edgePair][edgePairIntensity] = edgePairIntensities[edgePair][edgePairIntensity];
            }
        }
    }

    return packet;
}

QVector<QVector3D> PacketFileReader::readMatVoxelLocations( QString directory, QString voxelPosVariable){

    QVector<QVector3D> voxelLocations;
    //here we assume that the file contains
    //2 consecutive lines. in the first line
    //there is position information (x y z)
    //and in the following line there is the
    //intensity value of that voxel.
    //it is just a parser to train the rendering.

    QString voxelLocationFile = directory;

    MATFile *mFile = NULL;
    mFile = matOpen(voxelLocationFile.toStdString().c_str(),"r");
    if(mFile == NULL)
        cout << "error opening MAT file: " << endl;

    const mxArray *pa = NULL;
    pa = matGetVariable(mFile, voxelPosVariable.toStdString().c_str());

    if (pa == NULL)
        printf("Error reading existing matrix %s\n");

    const size_t *dims = mxGetDimensions_730( pa);
    double *mxData = (double*)mxGetData( pa);

    for( int row = 0; row < (int)dims[0]; row++)
        for( int column = 0; (int)column < dims[1]; column += 3)
            voxelLocations << QVector3D( mxData[dims[0] * column + row],
                                         mxData[dims[0] * (column+1) + row],
                                         mxData[dims[0] * (column+2) + row]);

    return voxelLocations;
}

QVector< QVector<float>> PacketFileReader::readMatVoxelIntensities( QString directory, QString voxelIntensitiesVariable){

    QVector< QVector<float>> voxelIntensities;

    QString voxelIntensityFile = directory;

    MATFile *mFile = NULL;
    mFile = matOpen(voxelIntensityFile.toStdString().c_str(),"r");
    if(mFile == NULL)
        cout << "error opening MAT file: " << endl;

    const mxArray *pa = NULL;
    pa = matGetVariable(mFile, voxelIntensitiesVariable.toStdString().c_str());

    if (pa == NULL)
        printf("Error reading existing matrix %s\n");

    const size_t *dims = mxGetDimensions_730( pa);
    double *mxData = (double*)mxGetData( pa);

    for( int row = 0; row < (int)dims[0]; row++){
        QVector<float> intensityVectorOfVoxel;
        for( int column = 0; (int)column < dims[1]; column++)
            intensityVectorOfVoxel << mxData[dims[0] * column + row];
        voxelIntensities << intensityVectorOfVoxel;
    }

    return voxelIntensities;
}

QVector<QVector2D> PacketFileReader::readMatEdgePairs( QString directory, QString edgePairsVariable){

    QVector<QVector2D> edgePairs;

    QString edgePairsFile = directory;


    MATFile *mFile = NULL;
    mFile = matOpen(edgePairsFile.toStdString().c_str(),"r");
    if(mFile == NULL)
        cout << "error opening MAT file: " << endl;

    const mxArray *pa = NULL;
    pa = matGetVariable(mFile, edgePairsVariable.toStdString().c_str());

    if (pa == NULL)
        printf("Error reading existing matrix %s\n");
    size_t rowSize = mxGetM(pa);

    for(size_t i=0; i < rowSize; i++){
         mxArray *cellArray = NULL;
         cellArray = mxGetCell_730(pa,i);
         size_t cellSize = mxGetN(cellArray);
         double *mxData = (double*)mxGetData( cellArray);
         for(int k = 0; k < cellSize; k++)
            edgePairs << QVector2D(i, mxData[k]-1);
    }


    return edgePairs;
}

QVector< QVector<float>> PacketFileReader::readMatEdgePairIntensities( QString directory, QString edgePairsIntensitiesVariable){

    QVector< QVector<float>> edgePairIntensities;

    QString edgePairIntensitiesFile = directory;

    MATFile *mFile = NULL;
    mFile = matOpen(edgePairIntensitiesFile.toStdString().c_str(),"r");
    if(mFile == NULL)
        cout << "error opening MAT file: " << endl;

    const mxArray *pa = NULL;
    pa = matGetVariable(mFile, edgePairsIntensitiesVariable.toStdString().c_str());

    if (pa == NULL)
        printf("Error reading existing matrix %s\n");

    const size_t *dims = mxGetDimensions_730( pa);
    double *mxData = (double*)mxGetData( pa);

    for( int row = 0; row < (int)dims[0]; row++){
        QVector<float> intensityOfPairInTime;
        for( int column = 0; (int)column < dims[1]; column++)
            intensityOfPairInTime << mxData[dims[0] * column + row];
        edgePairIntensities << intensityOfPairInTime;
    }

    return edgePairIntensities;
}

QVector<QVector3D> PacketFileReader::readVoxelLocations( QString directory){

    QVector<QVector3D> voxelLocations;
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
        return voxelLocations;
    }

    QTextStream instream(&voxelLocationFile);
    QString pos;

    while( (pos = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = pos.split(rx, QString::SkipEmptyParts);
        voxelLocations << QVector3D( list.at(0).toFloat(), list.at(1).toFloat(), list.at(2).toFloat());
    }
    voxelLocationFile.close();

    return voxelLocations;
}

QVector< QVector<float>> PacketFileReader::readVoxelIntensities( QString directory){

    QVector< QVector<float>> voxelIntensities;

    QFile voxelIntensityFile(directory + QDir::separator() + "intensities.txt");

    if(!voxelIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << voxelIntensityFile.error();
        return voxelIntensities;
    }

    QTextStream instream(&voxelIntensityFile);
    QString intensitiesOfVoxel;

    while( (intensitiesOfVoxel = instream.readLine()) != NULL){

        QRegExp rx("[,]");
        QStringList list = intensitiesOfVoxel.split(rx, QString::SkipEmptyParts);
        QVector<float> intensityVectorOfVoxel;

        for( int i = 0; i < list.length(); i++)
            intensityVectorOfVoxel << list.at(i).toFloat();

        voxelIntensities << intensityVectorOfVoxel;
    }

    return voxelIntensities;
}

QVector<QVector2D> PacketFileReader::readEdgePairs( QString directory){

    QVector<QVector2D> edgePairs;

    QFile edgePairFile(directory + QDir::separator() + "edgeConnections.txt");

    if(!edgePairFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << edgePairFile.error();
        return edgePairs;
    }

    QTextStream instream(&edgePairFile);
    QString edgePairLine;
    int count = 0;

    while( (edgePairLine = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = edgePairLine.split(rx, QString::SkipEmptyParts);
        for(int i=0; i< list.length();i++)
            edgePairs << QVector2D(count, list[i].toInt()-1);

        count++;
    }

    edgePairFile.close();

    return edgePairs;
}

QVector< QVector<float>> PacketFileReader::readEdgePairIntensities( QString directory){

    QVector< QVector<float>> edgePairIntensities;

    QFile contEdgeIntensityFile( directory + QDir::separator() + "edgeIntensities.txt");

    if(!contEdgeIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << contEdgeIntensityFile.error();
        return edgePairIntensities;
    }

    QTextStream instream(&contEdgeIntensityFile);
    QString intensityOfPair;

    while( (intensityOfPair = instream.readLine()) != NULL){

        QRegExp rx("[,]");
        QStringList list = intensityOfPair.split(rx, QString::SkipEmptyParts);
        QVector<float> intensityOfPairInTime;

        for( int i = 0; i < list.length(); i++)
            intensityOfPairInTime << list.at(i).toFloat();

        edgePairIntensities << intensityOfPairInTime;
    }

    return edgePairIntensities;
}
