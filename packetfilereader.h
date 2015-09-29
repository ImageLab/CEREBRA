#ifndef PACKETFILEREADER_H
#define PACKETFILEREADER_H

#include "Packet.h"
#include <QString>
#include <QVector>
#include <QtGui>
#include <iostream>
#include <cmath>

/**
 * @brief PacketFileReader class simply reads data from the given input directory
 * and set it to the related variable.
 *
 */


class PacketFileReader
{
public:
    PacketFileReader();
    ~PacketFileReader();

    /**
     * @brief readPacketFromDirectory sets all .txt file which is
     * included in the given directory folder. It simply calls
     * reading functions to set all data to appropriate variables.
     * @param directory is the folder that contains file
     * which formatted as voxels.txt, intensities.txt, edgeConnections.txt, edgeIntensities.txt
     * @return packet
     */
    Packet *readPacketFromDirectory( QString directory);

    /**
     * @brief readPacketFromMatlab simply calls MatLab reading functions
     * to set all data variables to appropriate QT variables.
     * @param  fileLocation is the location of the matlab file
     * @param voxelPosVariable is a position of the voxels simply formatted as coordinates(X,Y,Z)
     * @param  voxelIntensitiesVariable is a location of the voxel intensities
     * @param  edgePairsVariable is a location of the edge pair variable
     * @param  edgePairsIntensitiesVariable is a location of the edge pair intensities variable.
     * @return packet
     */
    Packet *readPacketFromMatlab( QString fileLocation,
                                  QString voxelPosVariable,
                                  QString voxelIntensitiesVariable,
                                  QString edgePairsVariable,
                                  QString edgePairsIntensitiesVariable);

private:
    /**
     * @brief readMatVoxelLocations reads voxel locations variable from the matlab file.
     * voxelPosVariable name is given as parameter to read voxel position values from the matlab file.
     * @param  fileName is the directory of the matlab file
     * @param  voxelPosVariable is a position of the voxels simply formatted as coordinates(X,Y,Z)
     * @param  packet
     */
     void readMatVoxelLocations( QString fileName, QString voxelPosVariable, Packet *packet);
     /**
      * @brief readMatVoxelIntensities reads voxel intensities variable from the matlab file.
      * voxelIntensitiesVariable name is given as parameter to read voxel intensities from the matlab file.
      * @param  fileName is the directory of the matlab file
      * @param  voxelIntensitiesVariable is a location of the voxel intensities
      * @param  packet
      */
    void readMatVoxelIntensities( QString fileName, QString voxelIntensitiesVariable, Packet *packet);
    /**
     * @brief readMatEdgePairs reads edge pairs variable from the matlab file.
     * Edge pair variable name is given as parameter to read pairs variable from the matlab file.
     * @param  fileName is the directory of the matlab file
     * @param  edgePairsVariable is a location of the edge pair variable.
     * @param  packet
     */
    void readMatEdgePairs( QString fileName, QString edgePairsVariable, Packet *packet);
    /**
     * @brief readMatEdgePairIntensities reads edge pairs intensities variable from the matlab file.
     * Edge pair intensity variable name is given as parameter to read edge pair intensity values from the matlab file.
     * @param  fileName is the directory of the matlab file
     * @param  edgePairsIntensitiesVariable is a location of the edge pair intensities variable.
     * @param  packet
     */
    void readMatEdgePairIntensities( QString fileName, QString edgePairsIntensitiesVariable, Packet *packet);

    /**
     * @brief readVoxelLocations reads voxel locations from the .txt file.
     * It sets voxel location values to the voxel location data vector.
     * @param directory is the folder that contains file which formatted as voxels.txt
     */
    void readVoxelLocations( QString directory, Packet *packet);
    /**
     * @brief readVoxelIntensities reads voxel intensities from .txt file
     * @param directory is the folder that contains file which formatted as intensities.txt
     */
    void readVoxelIntensities( QString directory, Packet *packet);
    /**
     * @brief readEdgePairs reads edge pairs from .txt file
     * @param directory is the folder that contains file which formatted as edgeConnections.txt
     */
    void readEdgePairs( QString directory, Packet *packet);
    /**
     * @brief readEdgePairIntensities reads edge pair intensities from .txt file
     * @param directory is the folder that contains file which formatted as edgeIntensities.txt
     */
    void readEdgePairIntensities( QString directory, Packet *packet);
};

#endif // PACKETFILEREADER_H
