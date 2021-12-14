#include "macros.h"
#include "defs.h"
#include "vcvs.h"
#include "sparse/spMatrix.h"

void makeEsrc(Esrc, numEsrc, buf)
vcvs *Esrc[];
int numEsrc;
char *buf;
{
    vcvs *inst;
    int j, nodeA, nodeB, nodeC, nodeD, branchNum, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
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

    inst = CALLOC(vcvs, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->pCNode = nodeC;
    inst->nCNode = nodeD;
    inst->branchNum = branchNum ;
    inst->gain = value;
    Esrc[numEsrc] = inst;
}

void printEsrc(Esrc, numEsrc)
vcvs *Esrc[];
int numEsrc;
{
    int i;
    vcvs *inst;
    for(i = 1; i <= numEsrc; i++) {
	inst = Esrc[i];
	printf("%s\t%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], NodeArray[inst->pCNode], NodeArray[inst->nCNode], inst->gain);
    }
}

void setupEsrc(Matrix, Esrc, numEsrc)
char *Matrix;
vcvs *Esrc[];
int numEsrc;
{
    int i, n1, n2, n3, n4, b1;
    vcvs *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numEsrc; i++) {
	inst = Esrc[i];
	inst->branchNum += NumNodes;
	n1 = inst->pNode;
	n2 = inst->nNode;
	n3 = inst->pCNode;
	n4 = inst->nCNode;
	b1 = inst->branchNum;

	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pb1n3 = spGetElement(Matrix, b1, n3);
	inst->pb1n4 = spGetElement(Matrix, b1, n4);
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
    }
}

void loadEsrc(Matrix, Rhs, Esrc, numEsrc)
char *Matrix;
double *Rhs;
vcvs *Esrc[];
int numEsrc;
{
    int i;
    vcvs *inst;
    double gain;

    /* stamp E source*/
    for(i = 1; i <= numEsrc; i++) {
	inst = Esrc[i];
	gain = inst->gain;
	*(inst->pb1n1) -= 1; 
	*(inst->pb1n2) += 1; 
	*(inst->pb1n3) += gain; 
	*(inst->pb1n4) -= gain; 
	*(inst->pn1b1) += 1; 
	*(inst->pn2b1) -= 1; 
    }
}
