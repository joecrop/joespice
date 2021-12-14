/* information used to describe a single instance */

typedef struct ccvs{
    char *name;  /* pointer to character string naming this ccvs */
    int pNode; /* number of positive node of ccvs */
    int nNode; /* number of negative node of ccvs */
    int cbranchNum ; /* number of control branch */
    int branchNum ; /* number of ccvs branch */
    double Rt;  /* value of trans-resistance */

    double *pb1n1;     /*pointer to sparse-matrix location (pNode, pNode)*/
    double *pb1n2;     /*pointer to sparse-matrix location (pNode, nNode)*/
    double *pn1b1;     /*pointer to sparse-matrix location (nNode, nNode)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nNode, pNode)*/
    double *pb1b2;     /*pointer to sparse-matrix location (nNode, pNode)*/
} ccvs ;

