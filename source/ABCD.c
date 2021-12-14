#include "macros.h"
#include "defs.h"
#include "ABCD.h"
#include "sparse/spMatrix.h"

void makePsrc(Psrc, numPsrc, buf)
ABCD *Psrc[];
int numPsrc;
char *buf;
{
    ABCD *inst;
    int j, nodeA, nodeB, nodeC, nodeD, branchNum, atoi();
    char name[MAXFIELD], cname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double A, B, C, D, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    branchNum = getMappedBranch(name);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeC = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeD = getMappedNode(node);
    j = getNextField(buf, num, j);
    A = atof(num);
    j = getNextField(buf, num, j);
    B = atof(num);
    j = getNextField(buf, num, j);
    C = atof(num);
    j = getNextField(buf, num, j);
    D = atof(num);

    inst = CALLOC(ABCD, 1);
    inst->name = (char *)strdup(name);
    inst->nodeA = nodeA;
    inst->nodeB = nodeB;
    inst->nodeC = nodeC;
    inst->nodeD = nodeD;
    inst->branchNum = branchNum;
    inst->A = A;
    inst->B = B;
    inst->C = C;
    inst->D = D;
    Psrc[numPsrc] = inst;
}

void printPsrc(Psrc, numPsrc)
ABCD *Psrc[];
int numPsrc;
{
    int i;
    ABCD *inst;
    for(i = 1; i <= numPsrc; i++) {
	inst = Psrc[i];
	printf("%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\n", inst->name, NodeArray[inst->nodeA], NodeArray[inst->nodeB], NodeArray[inst->nodeC], NodeArray[inst->nodeD], inst->A, inst->B, inst->C, inst->D);
    }
}

void setupPsrc(Matrix, Psrc, numPsrc)
char *Matrix;
ABCD *Psrc[];
int numPsrc;
{
    int i, n1, n2, n3, n4, b1;
    ABCD *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numPsrc; i++) {
	inst = Psrc[i];
	inst->branchNum += NumNodes;
	n1 = inst->nodeA;
	n2 = inst->nodeB;
	n3 = inst->nodeC;
	n4 = inst->nodeD;
	b1 = inst->branchNum;
	/* setup matrix and pointers */
	inst->pn1n3 = spGetElement(Matrix, n1, n3);
	inst->pn1n4 = spGetElement(Matrix, n1, n4);
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2n3 = spGetElement(Matrix, n2, n3);
	inst->pn2n4 = spGetElement(Matrix, n2, n4);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pn3b1 = spGetElement(Matrix, n3, b1);
	inst->pn4b1 = spGetElement(Matrix, n4, b1);

	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pb1n3 = spGetElement(Matrix, b1, n3);
	inst->pb1n4 = spGetElement(Matrix, b1, n4);
	inst->pb1b1 = spGetElement(Matrix, b1, b1);
    }
}

void loadPsrc(Matrix, Rhs, Psrc, numPsrc)
char *Matrix;
double *Rhs;
ABCD *Psrc[];
int numPsrc;
{
    int i;
    ABCD *inst;
    double A, B, C, D;
    /* stamp P source*/
    for(i = 1; i <= numPsrc; i++) {
	inst = Psrc[i];
	A = Psrc[i]->A;
	B = Psrc[i]->B;
	C = Psrc[i]->C;
	D = Psrc[i]->D;

	*(inst->pn1n3) += C; 
	*(inst->pn1n4) -= C; 
	*(inst->pn1b1) -= D; 
	*(inst->pn2n3) -= C; 
	*(inst->pn2n4) += C; 
	*(inst->pn2b1) += D; 
	*(inst->pn3b1) += 1; 
	*(inst->pn4b1) -= 1; 
	*(inst->pb1n1) += 1; 
	*(inst->pb1n2) -= 1; 
	*(inst->pb1n3) -= A; 
	*(inst->pb1n4) += A; 
	*(inst->pb1b1) += B; 
    }
}
