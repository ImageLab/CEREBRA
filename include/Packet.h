#include <cstdlib>
#include <vector>

#ifndef PACKET_HEADER
#define PACKET_HEADER
/// <summary>
/// This class encapsulates brain data.
/// </summary>
class Packet
{

public:

	Packet();
	/// <summary>
	/// Array of Node intensities. Accessed as Intensities[time,node], both 0-based.
	/// </summary>
	std::vector< std::vector< float> > intensities;

	/// <summary>
	// pair of edges. edge.first is origin. edge.second is the destination
	/// </summary>
	std::vector< std::pair<int, int> > edges;

	//edgeIntensities[edge][time]
	std::vector< std::vector<float> > edgeIntensities;

	/// <summary>
	/// Positions in space.
	/// </summary>
	std::vector< std::pair< float, std::pair<float, float> > > voxel3DPositions;
};
#endif