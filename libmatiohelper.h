#ifndef LIBMATIOHELPER
#define LIBMATIOHELPER

#include <vector>
#include "matio.h"

using namespace std;
class LibMatioHelper{

public:

    static int getVariables( const char *fName, std::vector< char *> &variables){

        mat_t    *matfp;
        matvar_t *matvar;

        variables.clear();

        matfp = Mat_Open( fName, MAT_ACC_RDONLY);

        if ( NULL == matfp ) {

            fprintf(stderr,"Error opening MAT file %s\n", fName);
            return EXIT_FAILURE;
        }

        while ( NULL != (matvar = Mat_VarReadNext( matfp))){

            variables.push_back( matvar->name);
            Mat_VarFree( matvar);
        }

        Mat_Close( matfp);

        return EXIT_SUCCESS;
    }
};

#endif // LIBMATIOHELPER
