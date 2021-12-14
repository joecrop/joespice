#include "macros.h"
#include "defs.h"
#include "ccvs.h"
#include "sparse/spMatrix.h"

void makeHsrc(Hsrc, numHsrc, buf)
ccvs *Hsrc[];
int numHsrc;
char *buf;
{
    ccvs *inst;
    int j, nodeA, nodeB, cbranchNum, branchNum, atoi();
    char name[MAXFIELD], cname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    branchNum = getMappedBranch(name);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, cname, j);
    cbranchNum = getMappedBranch(cname);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(ccvs, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->branchNum = branchNum ;
    inst->cbranchNum = cbranchNum ;
    inst->Rt = value;
    Hsrc[numHsrc] = inst;
}

void printHsrc(Hsrc, numHsrc)
ccvs *Hsrc[];
int numHsrc;
{
    int i;
    ccvs *inst;
    for(i = 1; i <= numHsrc; i++) {
	inst = Hsrc[i];
	printf("%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], BranchArray[inst->cbranchNum],  inst->Rt);
    }
}

void setupHsrc(Matrix, Hsrc, numHsrc)
char *Matrix;
ccvs *Hsrc[];
int numHsrc;
{
    int i, n1, n2, b1, b2;
    ccvs *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numHsrc; i++) {
	inst = Hsrc[i];
	inst->branchNum += NumNodes;
	inst->cbranchNum += NumNodes;
	n1 = inst->pNode;
	n2 = inst->nNode;
	b1 = inst->branchNum;
	b2 = inst->cbranchNum;
	/* setup matrix and pointers */
	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pb1b2 = spGetElement(Matrix, b1, b2);
    }
}

void loadHsrc(Matrix, Rhs, Hsrc, numHsrc)
char *Matrix;
double *Rhs;
ccvs *Hsrc[];
int numHsrc;
{
    int i;
    ccvs *inst;
    double Rt ;

    /* stamp H source*/
    for(i = 1; i <= numHsrc; i++) {
	inst = Hsrc[i];
	Rt = inst->Rt;
	*(inst->pb1n1) += 1; 
	*(inst->pb1n2) -= 1; 
	*(inst->pn1b1) += 1; 
	*(inst->pn2b1) -= 1; 
	*(inst->pb1b2) -= Rt; 
    }
}
