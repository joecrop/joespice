#include <math.h>
#include "macros.h"
#include "defs.h"
#include "linind.h"
#include "sparse/spMatrix.h"


void makeLinInd(LinInd, numLinInd, buf)
linind *LinInd[];
int numLinInd;
char *buf;
{
    linind *inst;
    int j, nodeA, nodeB, branchNum, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    branchNum = getMappedBranch(name);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(linind, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->branchNum = branchNum;
    inst->size = value;
    LinInd[numLinInd] = inst;
}

void printLinInd(LinInd, numLinInd)
linind *LinInd[];
int numLinInd;
{
    int i;
    linind *inst;

    for(i = 1; i <= numLinInd; i++) {
	inst = LinInd[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->size);
    }
}

void setupLinInd(Matrix, LinInd, numLinInd)
char *Matrix;
linind *LinInd[];
int numLinInd;
{
    int i, n1, n2, b1;
    linind *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numLinInd; i++) {
	inst = LinInd[i];
	n1 = inst->pNode;
	n2 = inst->nNode;
	inst->branchNum += NumNodes;
	b1 = inst->branchNum;
	/* setup matrix and pointers */
	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pb1b1 = spGetElement(Matrix, b1, b1);
	inst->alpha = 0.0;
	inst->beta = 0.0;
    }
}

void loadLinInd(Matrix, Rhs, LinInd, numLinInd, Sol, iter, h)
char *Matrix;
double *Rhs;
linind *LinInd[];
int numLinInd;
double *Sol;
int iter;
double h;
{
    int i;
    linind *inst;
    double G, I, l, x, xdot;

    /* load matrix */
    for(i = 1; i <= numLinInd; i++) {
	inst = LinInd[i];
	l = inst->size;
	if(iter == 0)
	{
		x = Sol[inst->branchNum];
		xdot = inst->alpha*x + inst->beta;
		intgr8(x,xdot,h,&inst->alpha,&inst->beta);
		//printf("x:%f\txdot:%f\n", x, xdot);
	}

	G = l * inst->alpha;
	I = l * inst->beta;

	*(inst->pb1n1) += 1; 
	*(inst->pb1n2) -= 1; 
	*(inst->pn1b1) += 1; 
	*(inst->pn2b1) -= 1; 
	*(inst->pb1b1) -= G; 
	Rhs[inst->branchNum] += I;
    }
}
