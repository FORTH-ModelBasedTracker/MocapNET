
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string.h>

#include "csv.hpp"
#include "../MocapNETLib/mocapnet.hpp"
#include "../MocapNETLib/jsonMocapNETHelpers.hpp"

int writeCSVHeader(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
    std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);

    FILE * fp = fopen(filename,"w");
    if (fp!=0)
        {
            for (int i=0; i<inputValues.size(); i++)
                {
                    fprintf(fp,"%s",MocapNETInputUncompressedArrayNames[i]);
                    if (i<inputValues.size()-1)
                        {
                            fprintf(fp,",");
                        }
                }
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}

int writeCSVBody(const char * filename,struct skeletonCOCO * skeleton,unsigned int width,unsigned int height)
{
    FILE * fp = fopen(filename,"a");
    if (fp!=0)
        {
            std::vector<float> inputValues = flattenskeletonCOCOToVector(skeleton,width,height);
            if (inputValues.size()==0)
                {
                    fprintf(stderr,"Failed to read from JSON file..\n");
                }

            for (int i=0; i<inputValues.size(); i++)
                {
                    if (i%3==2)
                        {
                            fprintf(fp,"%0.1f",inputValues[i]);
                        }
                    else
                        {
                            fprintf(fp,"%f",inputValues[i]);
                        }

                    if (i<inputValues.size()-1)
                        {
                            fprintf(fp,",");
                        }
                }
            fprintf(fp,"\n");
            fclose(fp);
            return 1;
        }
    return 0;
}

