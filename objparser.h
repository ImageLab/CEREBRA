#ifndef OBJPARSER_H
#define OBJPARSER_H
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class OBJParser
{
public:

    static void parseOBJFileFromFile( char *fileLocation,
                               std::vector< std::pair<float, std::pair<float, float> > > &locations,
                               std::vector< std::pair<uint, std::pair<uint, uint> > > &faces){

        parseLocationsInOBJFileFromFile( fileLocation, locations);
        parseFacesInOBJFileFromFile( fileLocation, faces);
    }

    static void parseLocationsInOBJFileFromFile( char *fileLocation,
                                          std::vector< std::pair<float, std::pair<float, float> > > &locations){

        locations.clear();
        std::ifstream shortFile( fileLocation);

        std::string currentLine;
        bool vFound = false;
        int count = 0;
        while( getline( shortFile, currentLine)){

            if( vFound && currentLine.c_str()[0] != 'v')
                break;
            else if( !vFound && currentLine.c_str()[0] == 'v')
                vFound = true;

            if( vFound){

                locations.push_back(std::pair<float, std::pair<float, float> > ());
                std::pair<float, std::pair<float, float> > &curPos = locations[count++];
                sscanf(currentLine.c_str(), "%*s %f %f %f", &curPos.first, &curPos.second.first, &curPos.second.second);
            }
        }
    }

    static void parseFacesInOBJFileFromFile( char *fileLocation,
                                      std::vector< std::pair<uint, std::pair<uint, uint> > > &faces){

        faces.clear();
        std::ifstream shortFile( fileLocation);

        std::string currentLine;
        bool fFound = false;
        int count = 0;
        while( getline( shortFile, currentLine)){

            if( fFound && currentLine.c_str()[0] != 'f')
                break;
            else if( !fFound && currentLine.c_str()[0] == 'f')
                fFound = true;

            if( fFound){

                faces.push_back(std::pair<uint, std::pair<uint, uint> > ());
                std::pair<uint, std::pair<uint, uint> > &curFace = faces[count++];
                sscanf(currentLine.c_str(), "%*s %d %d %d", &curFace.first, &curFace.second.first, &curFace.second.second);
            }
        }
    }
};

#endif // OBJPARSER_H
