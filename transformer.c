#include "macros.h"
#include "defs.h"
#include "transformer.h"
#include "sparse/spMatrix.h"

void makeNsrc(Nsrc, numNsrc, buf)
transformer *Nsrc[];
int numNsrc;
char *buf;
{
    transformer *inst;
    int j, nodeA, nodeB, nodeC, nodeD, branchNum, atoi();
    char name[MAXFIELD], cname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

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
    value = atof(num);

    inst = CALLOC(transformer, 1);
    inst->name = (char *)strdup(name);
    inst->nodeA = nodeA;
    inst->nodeB = nodeB;
    inst->nodeC = nodeC;
    inst->nodeD = nodeD;
    inst->branchNum = branchNum ;
    inst->N = value;
    Nsrc[numNsrc] = inst;
}

void printNsrc(Nsrc, numNsrc)
transformer *Nsrc[];
int numNsrc;
{
    int i;
    transformer *inst;
    for(i = 1; i <= numNsrc; i++) {
	inst = Nsrc[i];
	printf("%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->nodeA], NodeArray[inst->nodeB], NodeArray[inst->nodeC], NodeArray[inst->nodeD], BranchArray[inst->branchNum],  inst->N);
    }
}

void setupNsrc(Matrix, Nsrc, numNsrc)
char *Matrix;
transformer *Nsrc[];
int numNsrc;
{
    int i, n1, n2, n3, n4, b1;
    transformer *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numNsrc; i++) {
	inst = Nsrc[i];
	inst->branchNum += NumNodes;
	n1 = inst->nodeA;
	n2 = inst->nodeB;
	n3 = inst->nodeC;
	n4 = inst->nodeD;
	b1 = inst->branchNum;
	/* setup matrix and pointers */
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pn3b1 = spGetElement(Matrix, n3, b1);
	inst->pn4b1 = spGetElement(Matrix, n4, b1);
	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pb1n3 = spGetElement(Matrix, b1, n3);
	inst->pb1n4 = spGetElement(Matrix, b1, n4);

    }
}

void loadNsrc(Matrix, Rhs, Nsrc, numNsrc)
char *Matrix;
double *Rhs;
transformer *Nsrc[];
int numNsrc;
{
    int i;
    transformer *inst;
    double N ;
    /* stamp N source*/
    for(i = 1; i <= numNsrc; i++) {
        inst = Nsrc[i];
	N = inst->N;
	*(inst->pn1b1) += 1; 
	*(inst->pn2b1) -= 1; 
	*(inst->pn3b1) -= N; 
	*(inst->pn4b1) += N; 
	*(inst->pb1n1) += 1; 
	*(inst->pb1n2) -= 1; 
	*(inst->pb1n3) -= N; 
	*(inst->pb1n4) += N; 
    }
}
