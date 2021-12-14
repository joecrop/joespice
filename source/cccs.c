#include "macros.h"
#include "defs.h"
#include "cccs.h"
#include "sparse/spMatrix.h"

void makeFsrc(Fsrc, numFsrc, buf)
cccs *Fsrc[];
int numFsrc;
char *buf;
{
    cccs *inst;
    int j, nodeA, nodeB, cbranchNum, atoi();
    char name[MAXFIELD], cname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, cname, j);
    cbranchNum = getMappedBranch(cname);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(cccs, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->cbranchNum = cbranchNum ;
    inst->gain = value;
    Fsrc[numFsrc] = inst;
}

void printFsrc(Fsrc, numFsrc)
cccs *Fsrc[];
int numFsrc;
{
    int i;
    cccs *inst;
    for(i = 1; i <= numFsrc; i++) {
	inst = Fsrc[i];
	printf("%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], BranchArray[inst->cbranchNum], inst->gain);
    }
}

void setupFsrc(Matrix, Fsrc, numFsrc)
char *Matrix;
cccs *Fsrc[];
int numFsrc;
{
    int i, n1, n2, b1;
    cccs *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numFsrc; i++) {
	inst = Fsrc[i];
	inst->cbranchNum += NumNodes;
	n1 = inst->pNode;
	n2 = inst->nNode;
	b1 = inst->cbranchNum;
	/* setup matrix and pointers */
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
    }
}

void loadFsrc(Matrix, Rhs, Fsrc, numFsrc)
char *Matrix;
double *Rhs;
cccs *Fsrc[];
int numFsrc;
{
    int i;
    cccs *inst;
    double gain ;

    /* stamp F source*/
    for(i = 1; i <= numFsrc; i++) {
	inst = Fsrc[i];
	gain = inst->gain;
	*(inst->pn1b1) += gain; 
	*(inst->pn2b1) -= gain; 
    }
}
