/* information used to describe a single instance */

typedef struct linind{
    char *name;  /* pointer to character string naming this instance */
    int pNode; /* number of positive node of resistor */
    int nNode; /* number of negative node of resistor */
    int branchNum; /* number of branch */

    double size;  /* cap */
    double alpha;  /* cap */
    double beta;  /* cap */
    double *pb1n1;     /*pointer to sparse-matrix location (branchNum, pNode)*/
    double *pb1n2;     /*pointer to sparse-matrix location (branchNum, nNode)*/
    double *pn1b1;     /*pointer to sparse-matrix location (nNode, branchNum)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nNode, branchNum)*/
    double *pb1b1;     /*pointer to sparse-matrix location (branchNum, branchNum)*/
} linind ;
