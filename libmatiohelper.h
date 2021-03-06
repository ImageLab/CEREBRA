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
class LibMatioHelper
{

    public:

        /**
         * @brief getVariables gets variables from the given matlab file.
         * It uses Mat_VArReadNext function to read each variable from that file.
         * Display variables into the matlab variables part at the top-right of the screen.
         * @param fName is the matlab file name given as paramater to get variables
         * @param variables is read variables that comes from the given matlab file.
         */
        static int getVariables(const char *fName, std::vector< std::string> &variables)
        {
            mat_t *matfp;
            variables.clear();
            matfp = Mat_Open(fName, MAT_ACC_RDONLY);

            if (matfp == NULL)
            {
                fprintf(stderr,"Error opening MAT file %s\n", fName);
                return EXIT_FAILURE;
            }

            matvar_t *matvar;

            while ((matvar = Mat_VarReadNextInfo( matfp)) != NULL)
            {
                variables.push_back(std::string(matvar->name));
                Mat_VarFree(matvar);
                matvar = NULL;
            }
            Mat_Close(matfp);

            return EXIT_SUCCESS;
        }

        /**
         * @brief getIntegerValues gets values of the given variable name.
         * It simply reads the integer values with the given column-row index alignment.
         * @param fName is the matlab file name given as paramater to get variables
         * @param varName is the name of variavle to get values from it.
         * @param values is the values of that is read from the given variable name parameter.
         */
        static int getIntegerValues(const char* fName, const char *varName, std::vector< int> &values)
        {
            mat_t    *matfp;
            matvar_t *matvar;
            matfp = Mat_Open(fName, MAT_ACC_RDONLY);

            if (matfp == NULL)
            {
                fprintf(stderr,"Error opening MAT file %s\n", fName);
                return EXIT_FAILURE;
            }

            matvar = Mat_VarReadInfo(matfp, varName);

            if (matvar != NULL)
            {
                Mat_VarReadDataAll(matfp, matvar);
                double *data = (double*)matvar->data;
                values.clear();
                values.resize(matvar->dims[0]*matvar->dims[1]);

                for(int row = 0; row < (int)matvar->dims[0]; row++)
                {
                    for(int column = 0; column < (int)matvar->dims[1]; column++)
                    {
                        values[matvar->dims[0] * column + row] = data[matvar->dims[0] * column + row];
                    }
                }
                Mat_VarFree(matvar);
            }
            Mat_Close(matfp);

            return EXIT_SUCCESS;
        }
};
#endif // LIBMATIOHELPER
