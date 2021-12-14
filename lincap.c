#include <math.h>
#include "macros.h"
#include "defs.h"
#include "lincap.h"
#include "sparse/spMatrix.h"


void makeLinCap(LinCap, numLinCap, buf)
lincap *LinCap[];
int numLinCap;
char *buf;
{
    lincap *inst;
    int j, nodeA, nodeB, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(lincap, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->size = value;
    LinCap[numLinCap] = inst;
}

void printLinCap(LinCap, numLinCap)
lincap *LinCap[];
int numLinCap;
{
    int i;
    lincap *inst;

    for(i = 1; i <= numLinCap; i++) {
	inst = LinCap[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->size);
    }
}

void setupLinCap(Matrix, LinCap, numLinCap)
char *Matrix;
lincap *LinCap[];
int numLinCap;
{
    int i, n1, n2;
    lincap *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numLinCap; i++) {
	inst = LinCap[i];
	n1 = inst->pNode;
	n2 = inst->nNode;
	/* setup matrix and pointers */
	inst->pn1n1 = spGetElement(Matrix, n1, n1);
	inst->pn1n2 = spGetElement(Matrix, n1, n2);
	inst->pn2n1 = spGetElement(Matrix, n2, n1);
	inst->pn2n2 = spGetElement(Matrix, n2, n2);
	inst->alpha = 0.0;
	inst->beta = 0.0;
    }
}

void loadLinCap(Matrix, Rhs, LinCap, numLinCap, Sol, iter, h)
char *Matrix;
double *Rhs;
lincap *LinCap[];
int numLinCap;
double *Sol;
int iter;
double h;
{
    int i;
    lincap *inst;
    double v1, v2, G, I, c, x, xdot;

    /* load matrix */
    for(i = 1; i <= numLinCap; i++) {
	inst = LinCap[i];
	c = inst->size;
	if(iter == 0)
	{
		v1 = Sol[inst->pNode];
		v2 = Sol[inst->nNode];
		x = (v1-v2);
		xdot = inst->alpha*x + inst->beta;
		intgr8(x,xdot,h,&inst->alpha,&inst->beta);
		//printf("x:%f\txdot:%f\n", x, xdot);
	}

	G = c * inst->alpha;
	I = c * inst->beta;

	*(inst->pn1n1) += G; 
	*(inst->pn2n1) -= G; 
	*(inst->pn1n2) -= G; 
	*(inst->pn2n2) += G; 
	Rhs[inst->pNode] -= I;
	Rhs[inst->nNode] += I;
    }
}
