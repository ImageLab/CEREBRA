#ifndef LIBMATIOHELPER
#define LIBMATIOHELPER

#include <vector>
#include "matio.h"

using namespace std;
/**
 * @brief LibMatioHelper class handles MatLab operations with LibMatio library.
 * It is called when matlab file is loaded to the system.
 * Mainly, it reads matlab file variables.
 */
class LibMatioHelper{

public:
    /**
     * @brief getVariables gets variables from the given matlab file.
     * It uses Mat_VArReadNext function to read each variable from that file.
     * Display variables into the matlab variables part at the top-right of the screen.
     * @param fName is the matlab file name given as paramater to get variables
     * @param variables is read variables that comes from the given matlab file.     *
     */
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
    /**
     * @brief getIntegerValues gets values of the given variable name.
     * It simply reads the integer values with the given column-row index alignment.
     * @param fName is the matlab file name given as paramater to get variables
     * @param variables is read variables that comes from the given matlab file.     *
     */
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
