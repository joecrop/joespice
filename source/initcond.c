#include "macros.h"
#include "defs.h"
#include "initcond.h"
#include "sparse/spMatrix.h"

void makeInitCond(InitCond, numInitCond, buf)
initcond *InitCond[];
int numInitCond;
char *buf;
{
    initcond *inst;
    int j, nodeA, nodeB, branchNum, atoi();
    char name[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    branchNum = getMappedBranch(name);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    nodeB = getMappedNode("0");
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(initcond, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->branchNum = branchNum ;
    inst->voltage = value;
    InitCond[numInitCond] = inst;
}

void printInitCond(InitCond, numInitCond)
initcond *InitCond[];
int numInitCond;
{
    int i;
    initcond *inst;

    for(i = 1; i <= numInitCond; i++) {
	inst = InitCond[i];
	printf("%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->voltage);
    }
}

void setupInitCond(Matrix, InitCond, numInitCond)
char *Matrix;
initcond *InitCond[];
int numInitCond;
{
    int i, n1, n2, b1;
    initcond *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numInitCond; i++) {
	inst = InitCond[i];
	inst->branchNum += NumNodes;
	n1 = inst->pNode;
	n2 = inst->nNode;
	b1 = inst->branchNum;
	/* setup matrix and pointers */
	inst->pn1b1 = spGetElement(Matrix, n1, b1);
	inst->pn2b1 = spGetElement(Matrix, n2, b1);
	inst->pb1n1 = spGetElement(Matrix, b1, n1);
	inst->pb1n2 = spGetElement(Matrix, b1, n2);
	inst->pb1b1 = spGetElement(Matrix, b1, b1);
    }
}

void loadInitCond(Matrix, Rhs, InitCond, numInitCond, time)
char *Matrix;
double *Rhs;
initcond *InitCond[];
int numInitCond;
double time;
{
    int i, branchNum;
    initcond *inst;
    double voltage;
	
    /* stamp voltage source and load matrix*/
    for(i = 1; i <= numInitCond; i++) {
	inst = InitCond[i];
	branchNum = inst->branchNum;
	voltage = inst->voltage;
	if(time == 0)
	{
		*(inst->pn1b1) += 1; 
		*(inst->pn2b1) -= 1; 
		*(inst->pb1n1) += 1; 
		*(inst->pb1n2) -= 1; 
		Rhs[branchNum] += voltage;
	}
	else
	{
		*(inst->pb1b1) += 1;  //turn it off
	}
    }
}
