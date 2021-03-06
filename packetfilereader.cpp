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

Packet *PacketFileReader::readPacketFromDirectory(QString directory)
{
    Packet *packet = new Packet();

    readVoxelLocations(directory, packet);
    readVoxelIntensities(directory, packet);
    readEdgePairs(directory, packet);
    readEdgePairIntensities(directory, packet);

    return packet;
}

Packet *PacketFileReader::readPacketFromMatlab(QString fileLocation,
                                               QString voxelPosVariable,
                                               QString voxelIntensitiesVariable,
                                               QString edgePairsVariable,
                                               QString edgePairsIntensitiesVariable){

    Packet *packet = new Packet();

    if (!voxelPosVariable.length())
        return packet;

    readMatVoxelLocations(fileLocation, voxelPosVariable, packet);
    readMatVoxelIntensities(fileLocation, voxelIntensitiesVariable, packet);
    readMatEdgePairs(fileLocation, edgePairsVariable, packet);
    readMatEdgePairIntensities(fileLocation, edgePairsIntensitiesVariable, packet);

    return packet;
}

void PacketFileReader::zeroMeanVoxelLocations(Packet *packet)
{
    float xCoord = 0;
    float yCoord = 0;
    float zCoord = 0;

    for (int row = 0; row < packet->voxel3DPositions.size(); row++)
    {
        xCoord += packet->voxel3DPositions[row].first;
        yCoord += packet->voxel3DPositions[row].second.first;
        zCoord += packet->voxel3DPositions[row].second.second;
    }

    xCoord = xCoord/packet->voxel3DPositions.size();
    yCoord = yCoord/packet->voxel3DPositions.size();
    zCoord = zCoord/packet->voxel3DPositions.size();

    for (int row = 0; row < packet->voxel3DPositions.size(); row++)
    {
        packet->voxel3DPositions[row].first -= xCoord;
        packet->voxel3DPositions[row].second.first -= yCoord;
        packet->voxel3DPositions[row].second.second -= zCoord;
    }

    return;
}

void PacketFileReader::readMatVoxelLocations(QString fileName, QString voxelPosVariable, Packet *packet)
{
    if (voxelPosVariable == NULL || !voxelPosVariable.length())
        return;

    mat_t *matfp;
    matvar_t *matvar;

    matfp = Mat_Open(fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if (matfp ==NULL)
    {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }

    matvar = Mat_VarReadInfo(matfp, voxelPosVariable.toStdString().c_str());

    if (matvar != NULL)
    {
        Mat_VarReadDataAll(matfp, matvar);
        double *data = (double*)matvar->data;
        packet->voxel3DPositions.clear();
        packet->voxel3DPositions.resize( matvar->dims[0]);

        for (int row = 0; row < (int)matvar->dims[0]; row++)
        {
            packet->voxel3DPositions[row].first = data[row];
            packet->voxel3DPositions[row].second.first = data[matvar->dims[0] + row];
            packet->voxel3DPositions[row].second.second = data[matvar->dims[0] * 2 + row];
        }

        zeroMeanVoxelLocations(packet);
        Mat_VarFree(matvar);
        matvar = NULL;
    }
    Mat_Close(matfp);
}

void PacketFileReader::readMatVoxelIntensities(QString fileName, QString voxelIntensitiesVariable, Packet *packet)
{

    if (voxelIntensitiesVariable == NULL || !voxelIntensitiesVariable.length())
        return;

    mat_t *matfp;
    matvar_t *matvar;
    matfp = Mat_Open(fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if (matfp == NULL)
    {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }

    matvar = Mat_VarReadInfo( matfp, voxelIntensitiesVariable.toStdString().c_str());

    if (matvar != NULL)
    {
        Mat_VarReadDataAll( matfp, matvar);
        double *data = (double*)matvar->data;
        packet->intensities.clear();
        packet->intensities.resize( matvar->dims[0]);
        packet->minMaxIntensity.clear();
        packet->minMaxIntensity.resize(1);
        packet->minMaxIntensity[0] = std::vector<float>();
        packet->minMaxIntensity[0].resize(2);
        packet->minMaxIntensity[0][0] = 32767;
        packet->minMaxIntensity[0][1] = -32767;

        for (int row = 0; row < (int)matvar->dims[0]; row++)
        {
            packet->intensities[row] = std::vector<float>();
            packet->intensities[row].resize( matvar->dims[1]);

            for (int column = 0; column < (int)matvar->dims[1]; column++)
            {
                packet->intensities[row][column] = data[ matvar->dims[0] * column + row];

                if(packet->intensities[row][column] > packet->minMaxIntensity[0][1])
                    packet->minMaxIntensity[0][1] = packet->intensities[row][column];

                if(packet->intensities[row][column] < packet->minMaxIntensity[0][0])
                    packet->minMaxIntensity[0][0] = packet->intensities[row][column];
            }
        }

        Mat_VarFree(matvar);
        matvar = NULL;
    }
    Mat_Close(matfp);
}

void PacketFileReader::readMatEdgePairs(QString fileName, QString edgePairsVariable, Packet *packet)
{
    if (edgePairsVariable == NULL || !edgePairsVariable.length())
        return;

    mat_t *matfp;
    matvar_t *matvar;
    matfp = Mat_Open( fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if (matfp == NULL)
    {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }

    matvar = Mat_VarReadInfo(matfp, edgePairsVariable.toStdString().c_str());
    packet->edges.clear();

    if (matvar != NULL)
    {
        Mat_VarReadDataAll(matfp, matvar);
        packet->edges.clear();

        for (int curCell = 0; curCell < matvar->dims[0]; curCell++)
        {
            matvar_t *cell = Mat_VarGetCell(matvar, curCell);
            double *data = (double *)(cell->data);
            size_t numberOfPairs = (cell->dims[0] == 1)?cell->dims[1]:cell->dims[0];

            for (int curValue = 0; curValue < numberOfPairs; curValue++)
                packet->edges.push_back( std::pair<int, int>(curCell, (int)(data[curValue]-1)));
        }

        Mat_VarFree(matvar);
        matvar = NULL;
    }
    Mat_Close(matfp);
}

void PacketFileReader::readMatEdgePairIntensities(QString fileName, QString edgePairsIntensitiesVariable, Packet *packet)
{
    if(edgePairsIntensitiesVariable == NULL || !edgePairsIntensitiesVariable.length())
        return;

    mat_t *matfp;
    matvar_t *matvar;
    matfp = Mat_Open(fileName.toStdString().c_str(), MAT_ACC_RDONLY);

    if (matfp == NULL)
    {
        fprintf(stderr,"Error opening MAT file %s\n", fileName.toStdString().c_str());
        return;
    }
    matvar = Mat_VarReadInfo(matfp, edgePairsIntensitiesVariable.toStdString().c_str());

    if (matvar != NULL)
    {
        Mat_VarReadDataAll(matfp, matvar);
        double *data = (double*)matvar->data;

        packet->edgeIntensities.clear();
        packet->edgeIntensities.resize(matvar->dims[0]);

        for (int row = 0; row < packet->edgeIntensities.size(); row++)
        {
            packet->edgeIntensities[row] = std::vector<float>();
            packet->edgeIntensities[row].resize( matvar->dims[1]);

            for (int column = 0; column < (int)matvar->dims[1]; column++)
                packet->edgeIntensities[row][column] = data[matvar->dims[0] * column + row];
        }

        Mat_VarFree(matvar);
        matvar = NULL;
    }
    Mat_Close(matfp);
}

void PacketFileReader::readVoxelLocations(QString directory, Packet *packet)
{
    QFile voxelLocationFile(directory + QDir::separator() + "voxels.txt");

    if(!voxelLocationFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << voxelLocationFile.error();
        return;
    }

    packet->voxel3DPositions.clear();
    QTextStream instream(&voxelLocationFile);
    QString pos;

    while ((pos = instream.readLine()) != NULL)
    {
        QRegExp rx("[ ]");
        QStringList list = pos.split(rx, QString::SkipEmptyParts);
        packet->voxel3DPositions.push_back(std::pair< float, std::pair< float, float> > (list.at( 0).toFloat(),
                                                                                          std::pair<float, float>(list.at( 1).toFloat(),
                                                                                                                  list.at( 2).toFloat())));
    }
    voxelLocationFile.close();
}

void PacketFileReader::readVoxelIntensities(QString directory, Packet *packet)
{
    QFile voxelIntensityFile(directory + QDir::separator() + "intensities.txt");

    if (!voxelIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << voxelIntensityFile.error();
        return;
    }

    QTextStream instream(&voxelIntensityFile);
    QString intensitiesOfVoxel;
    packet->intensities.clear();

    while ((intensitiesOfVoxel = instream.readLine()) != NULL)
    {
        QRegExp rx("[,]");
        QStringList list = intensitiesOfVoxel.split(rx, QString::SkipEmptyParts);
        packet->intensities.push_back(std::vector<float>());

        for (int i = 0; i < list.length(); i++)
            packet->intensities.at(packet->intensities.size()-1).push_back(list.at(i).toFloat());
    }
}

void PacketFileReader::readEdgePairs(QString directory, Packet *packet)
{
    QFile edgePairFile(directory + QDir::separator() + "edgeConnections.txt");

    if (!edgePairFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << edgePairFile.error();
        return;
    }

    packet->edges.clear();
    QTextStream instream(&edgePairFile);
    QString edgePairLine;
    int count = 0;

    while ((edgePairLine = instream.readLine()) != NULL)
    {
        QRegExp rx("[ ]");
        QStringList list = edgePairLine.split(rx, QString::SkipEmptyParts);
        for(int i=0; i< list.length();i++)
            packet->edges.push_back(std::pair<int, int> (count, (int)list[i].toInt()-1));

        count++;
    }
    edgePairFile.close();
}

void PacketFileReader::readEdgePairIntensities(QString directory, Packet *packet)
{
    QFile contEdgeIntensityFile(directory + QDir::separator() + "edgeIntensities.txt");

    if (!contEdgeIntensityFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << contEdgeIntensityFile.error();
        return;
    }

    packet->edgeIntensities.clear();
    QTextStream instream(&contEdgeIntensityFile);
    QString intensityOfPair;

    while ((intensityOfPair = instream.readLine()) != NULL)
    {
        QRegExp rx("[,]");
        QStringList list = intensityOfPair.split(rx, QString::SkipEmptyParts);

        packet->edgeIntensities.push_back( std::vector<float>());
        for (int i = 0; i < list.length(); i++)
            packet->edgeIntensities.at(packet->edgeIntensities.size()-1).push_back(list.at(i).toFloat());
    }
}
