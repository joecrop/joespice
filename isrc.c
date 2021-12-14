#include "macros.h"
#include "defs.h"
#include "isrc.h"
#include "sparse/spMatrix.h"

void makeIsrc(Isrc, numIsrc, buf)
isource *Isrc[];
int numIsrc;
char *buf;
{
    isource *inst;
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

    inst = CALLOC(isource, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->current = value;
    Isrc[numIsrc] = inst;
}

void printIsrc(Isrc, numIsrc)
isource *Isrc[];
int numIsrc;
{
    int i;
    isource *inst;
    for(i = 1; i <= numIsrc; i++) {
	inst = Isrc[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->current);
    }
}

void setupIsrc(Matrix, Isrc, numIsrc)
char *Matrix;
isource *Isrc[];
int numIsrc;
{
    int i;
    isource *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numIsrc; i++) {
    }
}

void loadIsrc(Matrix, Rhs, Isrc, numIsrc)
char *Matrix;
double *Rhs;
isource *Isrc[];
int numIsrc;
{
    int i, pNode, nNode;
    double current;

    /* stamp current source*/
    for(i = 1; i <= numIsrc; i++) {
	pNode = Isrc[i]->pNode;
	nNode = Isrc[i]->nNode;
	current = Isrc[i]->current;
	Rhs[pNode] -= current;
	Rhs[nNode] += current;
    }
}
