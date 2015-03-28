#include "packetfilereader.h"

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
