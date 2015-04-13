#ifndef PACKETFILEREADER_H
#define PACKETFILEREADER_H

#include "Packet.h"
#include <QString>
#include <QVector>
#include <QtGui>
#include <iostream>
#include <cmath>
#include "mat.h"
#include "matrix.h"

using namespace libsimple;

class PacketFileReader
{
public:
    PacketFileReader();
    ~PacketFileReader();

    Packet *readPacketFromDirectory( QString directory);

    Packet *readPacketFromMatlab( QString fileLocation,
                                  QString voxelPosVariable,
                                  QString voxelIntensitiesVariable,
                                  QString edgePairsVariable,
                                  QString edgePairsIntensitiesVariable);

private:

    QVector<QVector3D> readMatVoxelLocations( QString directory, QString voxelPosVariable);
    QVector< QVector<float>> readMatVoxelIntensities( QString directory, QString voxelIntensitiesVariable);
    QVector<QVector2D> readMatEdgePairs( QString directory, QString edgePairsVariable);
    QVector< QVector<float>> readMatEdgePairIntensities( QString directory, QString edgePairsIntensitiesVariable);

    QVector<QVector3D> readVoxelLocations( QString directory);
    QVector< QVector<float>> readVoxelIntensities( QString directory);
    QVector<QVector2D> readEdgePairs( QString directory);
    QVector< QVector<float>> readEdgePairIntensities( QString directory);
};

#endif // PACKETFILEREADER_H
