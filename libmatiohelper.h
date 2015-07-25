#ifndef LIBMATIOHELPER
#define LIBMATIOHELPER

#include <vector>
#include "matio.h"

using namespace std;
class LibMatioHelper{

public:

    static int getVariables( const char *fName, std::vector< std::string> &variables){

        mat_t *matfp;

        variables.clear();

        matfp = Mat_Open( fName, MAT_ACC_RDONLY);

        if ( NULL == matfp ) {

            fprintf(stderr,"Error opening MAT file %s\n", fName);
            return EXIT_FAILURE;
        }

        matvar_t *matvar;
        while ( NULL != (matvar = Mat_VarReadNextInfo( matfp))){

            variables.push_back( std::string(matvar->name));
            Mat_VarFree( matvar);
            matvar = NULL;
        }

        Mat_Close( matfp);

        return EXIT_SUCCESS;
    }

    static int getIntegerValues( const char* fName, const char *varName, std::vector< int> &values){

        mat_t    *matfp;
        matvar_t *matvar;

        matfp = Mat_Open( fName, MAT_ACC_RDONLY);

        if ( NULL == matfp) {
            fprintf(stderr,"Error opening MAT file %s\n", fName);
            return EXIT_FAILURE;
        }

        matvar = Mat_VarReadInfo( matfp, varName);

        if ( NULL != matvar ){

            Mat_VarReadDataAll( matfp, matvar);

            double *data = (double*)matvar->data;

            values.clear();
            values.resize( matvar->dims[0]*matvar->dims[1]);
            for( int row = 0; row < (int)matvar->dims[0]; row++)
                for( int column = 0; column < (int)matvar->dims[1]; column++)
                    values[ matvar->dims[0] * column + row] = data[ matvar->dims[0] * column + row];

            Mat_VarFree( matvar);
        }

        Mat_Close(matfp);

        return EXIT_SUCCESS;
    }
};

#endif // LIBMATIOHELPER
