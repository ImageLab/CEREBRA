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

    Packet *readPacketFromDirectory(QString directory);
    void readMatFile(QString directory);
    void cvmxPrintMatrix(const mxArray* mxarr);

private:

    QVector<QVector3D> readVoxelLocations( QString directory);
    QVector< QVector<float>> readVoxelIntensities( QString directory);
    QVector<QVector2D> readEdgePairs( QString directory);
    QVector< QVector<float>> readEdgePairIntensities( QString directory);
};

#endif // PACKETFILEREADER_H
