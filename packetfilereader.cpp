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
    //QVector<QVector2D> edgePairs = readEdgePairs( directory);
    //QVector<QVector<float>> edgePairIntensities = readEdgePairIntensities( directory);

    Packet *packet = new Packet();
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
            (packet->intensities[voxel]).resize(voxelIntensities[voxel].length());
            for( int intensity = 0; intensity < voxelIntensities[voxel].length(); intensity++)
                packet->intensities[voxel][intensity] = voxelIntensities[voxel][intensity];
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
    QFile file(directory + QDir::separator() + "voxels.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return voxelLocations;
    }

    QTextStream instream(&file);
    QString pos;

    while( (pos = instream.readLine()) != NULL){

        QRegExp rx("[ ]");
        QStringList list = pos.split(rx, QString::SkipEmptyParts);
        voxelLocations << QVector3D( list.at(0).toFloat(), list.at(1).toFloat(), list.at(2).toFloat());
    }
    file.close();

    return voxelLocations;
}

QVector< QVector<float>> PacketFileReader::readVoxelIntensities( QString directory){

    QVector< QVector<float>> voxelIntensities;

    QFile intensityFile(directory + QDir::separator() + "intensities.txt");

    if(!intensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << intensityFile.error();
        return voxelIntensities;
    }

    QTextStream instream(&intensityFile);
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

    QVector<QVector2D> a;
    return a;
}

QVector< QVector<float>> PacketFileReader::readEdgePairIntensities( QString directory){

    QVector< QVector<float>> a;
    return a;
}
