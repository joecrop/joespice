/* information used to describe a single instance */

typedef struct vcvs{
    char *name;  /* pointer to character string naming this vcvs */
    int pNode; /* number of positive node of vcvs */
    int nNode; /* number of negative node of vcvs */
    int pCNode; /* number of positive control node of vcvs */
    int nCNode; /* number of negative control node of vcvs */
    int branchNum ; /* number of branch */
    double gain;  /* value of gain */

    double *pb1n1;     /*pointer to sparse-matrix location (branchNum, pNode)*/
    double *pb1n2;     /*pointer to sparse-matrix location (branchNum, nNode)*/
    double *pb1n3;     /*pointer to sparse-matrix location (branchNum, pCNode)*/
    double *pb1n4;     /*pointer to sparse-matrix location (branchNum, nCNode)*/
    double *pn1b1;     /*pointer to sparse-matrix location (pNode, branchNum)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nNode, branchNum)*/
} vcvs ;

