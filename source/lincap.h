/* information used to describe a single instance */

typedef struct lincap{
    char *name;  /* pointer to character string naming this instance */
    int pNode; /* number of positive node of resistor */
    int nNode; /* number of negative node of resistor */

    double size;  /* cap */
    double alpha;  /* cap */
    double beta;  /* cap */
    double *pn1n1;     /*pointer to sparse-matrix location (pNode, pNode)*/
    double *pn1n2;     /*pointer to sparse-matrix location (pNode, nNode)*/
    double *pn2n2;     /*pointer to sparse-matrix location (nNode, nNode)*/
    double *pn2n1;     /*pointer to sparse-matrix location (nNode, pNode)*/
} lincap ;
