/* information used to describe a single instance */

typedef struct transformer{
    char *name;  /* pointer to character string naming this transformer */
    int nodeA; /* number of node A */
    int nodeB; /* number of node B */
    int nodeC; /* number of node C */
    int nodeD; /* number of node D */
    int branchNum ; /* number of transformer branch */
    double N;  /* value of turns ratio */

    double *pn1b1;     /*pointer to sparse-matrix location (nodeA, branchNum)*/
    double *pn2b1;     /*pointer to sparse-matrix location (nodeB, branchNum)*/
    double *pn3b1;     /*pointer to sparse-matrix location (nodeC, branchNum)*/
    double *pn4b1;     /*pointer to sparse-matrix location (nodeD, branchNum)*/
    double *pb1n1;     /*pointer to sparse-matrix location (branchNum, nodeA)*/
    double *pb1n2;     /*pointer to sparse-matrix location (branchNum, nodeB)*/
    double *pb1n3;     /*pointer to sparse-matrix location (branchNum, nodeC)*/
    double *pb1n4;     /*pointer to sparse-matrix location (branchNum, nodeD)*/

} transformer ;

