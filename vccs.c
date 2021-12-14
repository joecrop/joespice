#include "macros.h"
#include "defs.h"
#include "vccs.h"
#include "sparse/spMatrix.h"

void makeGsrc(Gsrc, numGsrc, buf)
vccs *Gsrc[];
int numGsrc;
char *buf;
{
    vccs *inst;
    int j, nodeA, nodeB, nodeC, nodeD, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
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

    inst = CALLOC(vccs, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->pCNode = nodeC;
    inst->nCNode = nodeD;
    inst->Gm = value;
    Gsrc[numGsrc] = inst;
}

void printGsrc(Gsrc, numGsrc)
vccs *Gsrc[];
int numGsrc;
{
    int i;
    vccs *inst;
    for(i = 1; i <= numGsrc; i++) {
	inst = Gsrc[i];
	printf("%s\t%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], NodeArray[inst->pCNode], NodeArray[inst->nCNode], inst->Gm);
    }
}

void setupGsrc(Matrix, Gsrc, numGsrc)
char *Matrix;
vccs *Gsrc[];
int numGsrc;
{
    int i, n1, n2, n3, n4;
    vccs *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numGsrc; i++) {
	inst = Gsrc[i];
	n1 = inst->pNode;
	n2 = inst->nNode;
	n3 = inst->pCNode;
	n4 = inst->nCNode;
	/* setup matrix and pointers */
	inst->pn1n3 = spGetElement(Matrix, n1, n3);
	inst->pn1n4 = spGetElement(Matrix, n1, n4);
	inst->pn2n3 = spGetElement(Matrix, n2, n3);
	inst->pn2n4 = spGetElement(Matrix, n2, n4);

    }
}

void loadGsrc(Matrix, Rhs, Gsrc, numGsrc)
char *Matrix;
double *Rhs;
vccs *Gsrc[];
int numGsrc;
{
    int i;
    vccs *inst;
    double Gm;
    /* stamp G source*/
    for(i = 1; i <= numGsrc; i++) {
	inst = Gsrc[i];
	Gm = inst->Gm;
	*(inst->pn1n3) += Gm; 
	*(inst->pn1n4) -= Gm; 
	*(inst->pn2n3) -= Gm; 
	*(inst->pn2n4) += Gm; 
    }
}
