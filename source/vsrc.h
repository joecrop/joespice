/* information used to describe a single instance */

typedef struct vsource{
    char *name;  /* pointer to character string naming this vsource */
    int pNode; /* number of positive node of vsource */
    int nNode; /* number of negative node of vsource */
    int branchNum ; /* number of branch */
    double voltage;  /* value of vsource */

    double *pn1b1;     /*pointer to sparse-matrix location (pNode, branchNum)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nNode, branchNum)*/
    double *pb1n1;     /*pointer to sparse-matrix location (branchNum, pNode)*/
    double *pb1n2;     /*pointer to sparse-matrix location (branchNum, nNode)*/

} vsource ;

