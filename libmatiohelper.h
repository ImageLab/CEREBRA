#ifndef LIBMATIOHELPER
#define LIBMATIOHELPER

#include <vector>
#include "matio.h"

using namespace std;
class LibMatioHelper{

public:

    static int getVariables( const char *fName, std::vector< char *> &variables){

        mat_t    *matfp;

        variables.clear();
        std::vector< matvar_t *> matVariables;

        matfp = Mat_Open( fName, MAT_ACC_RDONLY);

        if ( NULL == matfp ) {

            fprintf(stderr,"Error opening MAT file %s\n", fName);
            return EXIT_FAILURE;
        }

        matvar_t *matvar;
        while ( NULL != (matvar = Mat_VarReadNext( matfp))){

            variables.push_back( matvar->name);
            matVariables.push_back( matvar);
        }

        for( int var = 0; var < matVariables.size(); var++)
            Mat_VarFree( matVariables.at(var));

        Mat_Close( matfp);

        return EXIT_SUCCESS;
    }
};

#endif // LIBMATIOHELPER
