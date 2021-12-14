/* information used to describe a single instance */

typedef struct vccs{
    char *name;  /* pointer to character string naming this vccs */
    int pNode; /* number of positive node of vccs */
    int nNode; /* number of negative node of vccs */
    int pCNode; /* number of positive control node of vccs */
    int nCNode; /* number of negative control node of vccs */
    double Gm;  /* value of trans-conductance */

    double *pn1n3;     /*pointer to sparse-matrix location (pNode, pCNode)*/
    double *pn1n4;     /*pointer to sparse-matrix location (pNode, nCNode)*/
    double *pn2n3;     /*pointer to sparse-matrix location (nNode, pCNode)*/
    double *pn2n4;     /*pointer to sparse-matrix location (nNode, nCNode)*/
} vccs ;

