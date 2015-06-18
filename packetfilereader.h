#ifndef PACKETFILEREADER_H
#define PACKETFILEREADER_H

#include "Packet.h"
#include <QString>
#include <QVector>
#include <QtGui>
#include <iostream>
#include <cmath>

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

    void readMatVoxelLocations( QString directory, QString voxelPosVariable, Packet *packet);
    void readMatVoxelIntensities( QString directory, QString voxelIntensitiesVariable, Packet *packet);
    void readMatEdgePairs( QString directory, QString edgePairsVariable, Packet *packet);
    void readMatEdgePairIntensities( QString directory, QString edgePairsIntensitiesVariable, Packet *packet);

    void readVoxelLocations( QString directory, Packet *packet);
    void readVoxelIntensities( QString directory, Packet *packet);
    void readEdgePairs( QString directory, Packet *packet);
    void readEdgePairIntensities( QString directory, Packet *packet);
};

#endif // PACKETFILEREADER_H
