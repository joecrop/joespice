#include "macros.h"
#include "defs.h"
#include "vsrc.h"
#include "sparse/spMatrix.h"

void makeVsrc(Vsrc, numVsrc, buf)
vsource *Vsrc[];
int numVsrc;
char *buf;
{
    vsource *inst;
    int j, nodeA, nodeB, branchNum, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

	/* incrementing the number of branches */

    j = 0;	
    j = getNextField(buf, name, j);
    branchNum = getMappedBranch(name);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(vsource, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->branchNum = branchNum ;
    inst->voltage = value;
    Vsrc[numVsrc] = inst;
}

void printVsrc(Vsrc, numVsrc)
vsource *Vsrc[];
int numVsrc;
{
    int i;
    vsource *inst;

    for(i = 1; i <= numVsrc; i++) {
	inst = Vsrc[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->voltage);
    }
}

void setupVsrc(Matrix, Vsrc, numVsrc)
char *Matrix;
vsource *Vsrc[];
int numVsrc;
{
    int i, n1, n2, b1;
    vsource *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numVsrc; i++) {
	inst = Vsrc[i];
	inst->branchNum += NumNodes;
	n1 = inst->pNode;
	n2 = inst->nNode;
	b1 = inst->branchNum;
	/* setup matrix and pointers */
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
    }
}

void loadVsrc(Matrix, Rhs, Vsrc, numVsrc)
char *Matrix;
double *Rhs;
vsource *Vsrc[];
int numVsrc;
{
    int i, branchNum;
    vsource *inst;
    double voltage;
	
    /* stamp voltage source and load matrix*/
    for(i = 1; i <= numVsrc; i++) {
	inst = Vsrc[i];
	branchNum = inst->branchNum;
	voltage = inst->voltage;

	*(inst->pn1b1) += 1; 
	*(inst->pn2b1) -= 1; 
	*(inst->pb1n1) += 1; 
	*(inst->pb1n2) -= 1; 
	Rhs[branchNum] += voltage;
    }
}
