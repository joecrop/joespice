#include "macros.h"
#include "defs.h"
#include "res.h"
#include "sparse/spMatrix.h"

void makeRes(Res, numRes, buf)
resistor *Res[];
int numRes;
char *buf;
{
    resistor *inst;
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

    inst = CALLOC(resistor, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->value = value;
    Res[numRes] = inst;
}

void printRes(Res, numRes)
resistor *Res[];
int numRes;
{
    int i;
    resistor *inst;

    for(i = 1; i <= numRes; i++) {
	inst = Res[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->value);
    }
}

void setupRes(Matrix, Res, numRes)
char *Matrix;
resistor *Res[];
int numRes;
{
    int i, n1, n2;
    resistor *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numRes; i++) {
	inst = Res[i];
	inst->conduct = 1.0/inst->value;
	n1 = inst->pNode;
	n2 = inst->nNode;
	/* setup matrix and pointers */
	inst->pn1n1 = spGetElement(Matrix, n1, n1);
	inst->pn1n2 = spGetElement(Matrix, n1, n2);
	inst->pn2n2 = spGetElement(Matrix, n2, n2);
	inst->pn2n1 = spGetElement(Matrix, n2, n1);
    }
}

void loadRes(Matrix, Rhs, Res, numRes)
char *Matrix;
double *Rhs;
resistor *Res[];
int numRes;
{
    int i;
    resistor *inst;
    double conduct;

    /* load matrix */
    for(i = 1; i <= numRes; i++) {
	inst = Res[i];
	conduct = inst->conduct;
	*(inst->pn1n1) += conduct; 
	*(inst->pn1n2) -= conduct; 
	*(inst->pn2n2) += conduct; 
	*(inst->pn2n1) -= conduct; 
    }
}
