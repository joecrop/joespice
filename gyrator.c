#include "macros.h"
#include "defs.h"
#include "gyrator.h"
#include "sparse/spMatrix.h"

void makeTsrc(Tsrc, numTsrc, buf)
gyrator *Tsrc[];
int numTsrc;
char *buf;
{
    gyrator *inst;
    int j, nodeA, nodeB, nodeC, nodeD, atoi();
    char name[MAXFIELD], cname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
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

    inst = CALLOC(gyrator, 1);
    inst->name = (char *)strdup(name);
    inst->nodeA = nodeA;
    inst->nodeB = nodeB;
    inst->nodeC = nodeC;
    inst->nodeD = nodeD;
    inst->G = value;
    Tsrc[numTsrc] = inst;
}

void printTsrc(Tsrc, numTsrc)
gyrator *Tsrc[];
int numTsrc;
{
    int i;
    gyrator *inst;
    for(i = 1; i <= numTsrc; i++) {
	inst = Tsrc[i];
	printf("%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->nodeA], NodeArray[inst->nodeB], NodeArray[inst->nodeC], NodeArray[inst->nodeD],  inst->G);
    }
}

void setupTsrc(Matrix, Tsrc, numTsrc)
char *Matrix;
gyrator *Tsrc[];
int numTsrc;
{
    int i, n1, n2, n3, n4;
    gyrator *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numTsrc; i++) {
	inst = Tsrc[i];
	n1 = inst->nodeA;
	n2 = inst->nodeB;
	n3 = inst->nodeC;
	n4 = inst->nodeD;
	/* setup matrix and pointers */
	inst->pn1n3 = spGetElement(Matrix, n1, n3);
	inst->pn1n4 = spGetElement(Matrix, n1, n4);
	inst->pn2n3 = spGetElement(Matrix, n2, n3);
	inst->pn2n4 = spGetElement(Matrix, n2, n4);
	inst->pn3n1 = spGetElement(Matrix, n3, n1);
	inst->pn3n2 = spGetElement(Matrix, n3, n2);
	inst->pn4n1 = spGetElement(Matrix, n4, n1);
	inst->pn4n2 = spGetElement(Matrix, n4, n2);

    }
}

void loadTsrc(Matrix, Rhs, Tsrc, numTsrc)
char *Matrix;
double *Rhs;
gyrator *Tsrc[];
int numTsrc;
{
    int i;
    gyrator *inst;
    double G;
    /* stamp T source*/
    for(i = 1; i <= numTsrc; i++) {
        inst = Tsrc[i];
	G = inst->G;
	*(inst->pn1n3) -= G; 
	*(inst->pn1n4) += G; 
	*(inst->pn2n3) += G; 
	*(inst->pn2n4) -= G; 

	*(inst->pn3n1) += G; 
	*(inst->pn3n2) -= G; 
	*(inst->pn4n1) -= G; 
	*(inst->pn4n2) += G; 
    }
}
