#include "packetfilereader.h"
#include "mainwindow.h"
#include "mat.h"
#include "matrix.h"
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

void PacketFileReader::readMatVoxelLocations( QString directory, QString voxelPosVariable, Packet *packet){

    if( !voxelPosVariable.length())
        return;

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
    packet->vXYZ.clear();
    //can previously resize here!!!

    for( int row = 0; row < (int)dims[0]; row++)
        for( int column = 0; column < (int)dims[1]; column += 3)
            packet->vXYZ.push_back( libsimple::Packet::Point3D( mxData[dims[0] * column + row],
                                                                mxData[dims[0] * (column+1) + row],
                                                                mxData[dims[0] * (column+2) + row]));
}

void PacketFileReader::readMatVoxelIntensities( QString directory, QString voxelIntensitiesVariable, Packet *packet){

    if( !voxelIntensitiesVariable.length())
        return;

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
    packet->intensities.clear();

    for( int row = 0; row < (int)dims[0]; row++){

        packet->intensities.push_back( vector<float>());

        for( int column = 0; column < (int)dims[1]; column++)
            packet->intensities.at(packet->intensities.size()-1).push_back(mxData[dims[0] * column + row]);
    }
}

void PacketFileReader::readMatEdgePairs( QString directory, QString edgePairsVariable, Packet *packet){

    if( !edgePairsVariable.length())
        return;

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
    packet->edges.clear();

    for(size_t i=0; i < rowSize; i++){
         mxArray *cellArray = NULL;
         cellArray = mxGetCell_730(pa,i);
         size_t cellSize = mxGetN(cellArray);
         double *mxData = (double*)mxGetData( cellArray);
         for(int k = 0; k < (int)cellSize; k++)
            packet->edges.push_back( libsimple::Packet::Point2D((float)i, (float)mxData[k]-1));
    }
}

void PacketFileReader::readMatEdgePairIntensities( QString directory, QString edgePairsIntensitiesVariable, Packet *packet){

    if( !edgePairsIntensitiesVariable.length())
        return;

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
    packet->edgeIntensities.clear();

    for( int row = 0; row < (int)dims[0]; row++){

        packet->edgeIntensities.push_back( vector<float>());
        for( int column = 0; column < (int)dims[1]; column++)
            packet->edgeIntensities.at(packet->edgeIntensities.size()-1).push_back(mxData[dims[0] * column + row]);
    }
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
