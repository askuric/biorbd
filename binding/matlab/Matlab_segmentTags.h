#ifndef BIORBD_MATLAB_SEGMENT_TAGS_H
#define BIORBD_MATLAB_SEGMENT_TAGS_H

#include <mex.h>
#include "BiorbdModel.h"
#include "class_handle.h"
#include "processArguments.h"

void Matlab_segmentTags( int, mxArray *plhs[],
                  int nrhs, const mxArray*prhs[] ){

    // Verifier les arguments d'entree
    checkNombreInputParametres(nrhs, 3, 5, "3 arguments are required (+2 optional) where the 2nd is the handler on the model, 3rd is the Q, 4th if you want to remove axes as specified in the model file [default = true] and 5th optional is a specific segment index");
    // Recevoir le model
    biorbd::Model * model = convertMat2Ptr<biorbd::Model>(prhs[1]);
    unsigned int nQ = model->nbQ(); // Get the number of DoF

    // Recevoir Q
    biorbd::rigidbody::GeneralizedCoordinates Q = *getParameterQ(prhs, 2, nQ).begin();

    bool removeAxes(true);
    if (nrhs>=4)
        removeAxes = getBool(prhs, 3);

    // Recevoir l'index
    int idx(-1);
    if (nrhs >= 5)
        idx = getInteger(prhs,4)-1; // -1 afin que le segment 1 soit le root


    if ( idx==-1){ // Si on a demande tous les segments
        // Trouver ou sont les marqueurs
        std::vector<std::vector<biorbd::rigidbody::NodeBone>> allTags;
        for (unsigned int i=0; i<model->nbBone(); ++i)    {
            std::vector<biorbd::rigidbody::NodeBone> Tags_tp = model->segmentTags(*model, Q, i, removeAxes);
            allTags.push_back(Tags_tp);
        }
        // Create a matrix for the return argument
        plhs[0] = mxCreateCellMatrix( allTags.size(), 1);
        for (unsigned int i_bone=0; i_bone<allTags.size(); ++i_bone){
            mxArray *tags_out_tp = mxCreateDoubleMatrix( 3, (*(allTags.begin()+i_bone)).size(), mxREAL);
            double *Tags = mxGetPr(tags_out_tp);

            // Remplir le output
            std::vector<biorbd::rigidbody::NodeBone>::iterator it=(*(allTags.begin()+i_bone)).begin();
            for (unsigned int i=0; (it+i)!=(*(allTags.begin()+i_bone)).end(); ++i){
                Tags[i*3] = (*(it+i))(0);
                Tags[i*3+1] = (*(it+i))(1);
                Tags[i*3+2] = (*(it+i))(2);
            }
            mxSetCell(plhs[0],i_bone,tags_out_tp);
        }
        return;

    }
    else{ // Si on a demande un segment precis
        std::vector<biorbd::rigidbody::NodeBone> Tags_tp = model->segmentTags(*model, Q, idx, removeAxes);

        // Create a matrix for the return argument
        plhs[0] = mxCreateDoubleMatrix(3, Tags_tp.size(), mxREAL);
        double *Tags = mxGetPr(plhs[0]);

        // Remplir le output
        std::vector<biorbd::rigidbody::NodeBone>::iterator it=Tags_tp.begin();
        for (unsigned int i=0; (it+i)!=Tags_tp.end(); ++i){
            Tags[i*3] = (*(it+i))(0);
            Tags[i*3+1] = (*(it+i))(1);
            Tags[i*3+2] = (*(it+i))(2);
        }
        return;
    }
}

#endif // BIORBD_MATLAB_SEGMENT_TAGS_H
