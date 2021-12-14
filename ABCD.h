/* information used to describe a single instance */

typedef struct ABCD{
    char *name;  /* pointer to character string naming this ABCD 2-port */
    int nodeA; /* number of node A */
    int nodeB; /* number of node B */
    int nodeC; /* number of node C */
    int nodeD; /* number of node D */
    int branchNum; /* number of branch i2 */
    double A;  /* value of A parameter */
    double B;  /* value of B parameter */
    double C;  /* value of C parameter */
    double D;  /* value of D parameter */

    double *pn1n3;     /*pointer to sparse-matrix location (nodeA, nodeC)*/
    double *pn1n4;     /*pointer to sparse-matrix location (nodeA, nodeD)*/
    double *pn1b1;     /*pointer to sparse-matrix location (nodeA, branchNum)*/
    double *pn2n3;     /*pointer to sparse-matrix location (nodeB, nodeC)*/
    double *pn2n4;     /*pointer to sparse-matrix location (nodeB, nodeD)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nodeB, branchNum)*/
    double *pn3b1;     /*pointer to sparse-matrix location (nodeC, branchNum)*/
    double *pn4b1;     /*pointer to sparse-matrix location (nodeD, branchNum)*/
    double *pb1n1;     /*pointer to sparse-matrix location (branchNum, nodeA)*/
    double *pb1n2;     /*pointer to sparse-matrix location (branchNum, nodeB)*/
    double *pb1n3;     /*pointer to sparse-matrix location (branchNum, nodeC)*/
    double *pb1n4;     /*pointer to sparse-matrix location (branchNum, nodeD)*/
    double *pb1b1;     /*pointer to sparse-matrix location (branchNum, branchNum)*/
} ABCD ;

