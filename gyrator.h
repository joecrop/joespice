/* information used to describe a single instance */

typedef struct gyrator{
    char *name;  /* pointer to character string naming this gyrator */
    int nodeA; /* number of node A */
    int nodeB; /* number of node B */
    int nodeC; /* number of node C */
    int nodeD; /* number of node D */
    double G;  /* value of gyration constant */

    double *pn1n3;     /*pointer to sparse-matrix location (nodeA, nodeC)*/
    double *pn1n4;     /*pointer to sparse-matrix location (nodeA, nodeD)*/
    double *pn2n3;     /*pointer to sparse-matrix location (nodeB, nodeC)*/
    double *pn2n4;     /*pointer to sparse-matrix location (nodeB, nodeD)*/
    double *pn3n1;     /*pointer to sparse-matrix location (nodeC, nodeA)*/
    double *pn3n2;     /*pointer to sparse-matrix location (nodeC, nodeB)*/
    double *pn4n1;     /*pointer to sparse-matrix location (nodeD, nodeA)*/
    double *pn4n2;     /*pointer to sparse-matrix location (nodeD, nodeB)*/
} gyrator ;

