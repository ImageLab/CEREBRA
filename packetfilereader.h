#ifndef PACKETFILEREADER_H
#define PACKETFILEREADER_H

#include "Packet.h"
#include <QString>
#include <QVector>
#include <QtGui>
#include <iostream>
#include <cmath>

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

    void readMatVoxelLocations( QString fileName, QString voxelPosVariable, Packet *packet);
    void readMatVoxelIntensities( QString fileName, QString voxelIntensitiesVariable, Packet *packet);
    void readMatEdgePairs( QString fileName, QString edgePairsVariable, Packet *packet);
    void readMatEdgePairIntensities( QString fileName, QString edgePairsIntensitiesVariable, Packet *packet);

    void readVoxelLocations( QString directory, Packet *packet);
    void readVoxelIntensities( QString directory, Packet *packet);
    void readEdgePairs( QString directory, Packet *packet);
    void readEdgePairIntensities( QString directory, Packet *packet);
};

#endif // PACKETFILEREADER_H
