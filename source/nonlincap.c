#include <math.h>
#include "macros.h"
#include "defs.h"
#include "nonlincap.h"
#include "sparse/spMatrix.h"


void makeNonLinCap(NonLinCap, numNonLinCap, buf)
nonlincap *NonLinCap[];
int numNonLinCap;
char *buf;
{
    nonlincap *inst;
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

    inst = CALLOC(nonlincap, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->value = value;
    NonLinCap[numNonLinCap] = inst;
}

void printNonLinCap(NonLinCap, numNonLinCap)
nonlincap *NonLinCap[];
int numNonLinCap;
{
    int i;
    nonlincap *inst;

    for(i = 1; i <= numNonLinCap; i++) {
	inst = NonLinCap[i];
	printf("%s\t%s\t%s\t%g\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->value);
    }
}

void setupNonLinCap(Matrix, NonLinCap, numNonLinCap)
char *Matrix;
nonlincap *NonLinCap[];
int numNonLinCap;
{
    int i, n1, n2;
    nonlincap *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numNonLinCap; i++) {
	inst = NonLinCap[i];
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

void loadNonLinCap(Matrix, Rhs, NonLinCap, numNonLinCap, Sol, it_count, h, time)
char *Matrix;
double *Rhs;
nonlincap *NonLinCap[];
int numNonLinCap;
double *Sol;
int it_count;
double h;
double time;
{
    int i, na, nb;
    nonlincap *inst;
    double Cjo, Vc, VL, Q, dQdV, Gk, Ik;

    /* load matrix */
    for(i = 1; i <= numNonLinCap; i++) {
        inst = NonLinCap[i];
        Cjo = inst->value;
        na = inst->pNode;
        nb = inst->nNode;
        // Calculate voltage across capacitor. It is assumed that 
	// at the first iteration of a new timepoint
        // Sol has the solution from the previous timepoint
	if(time == 0)
		Vc = 0.7;
	else
        	Vc = Sol[na]-Sol[nb];

        // calculate charge and VL
        VL = 0.75-0.1*log(1.0+exp(-10.0*(Vc-0.75)));
        Q = 1.6*Cjo*(1.0-sqrt(1.0-VL/0.8));
        dQdV = (Cjo*exp(-10.0*Vc+7.5)) / (sqrt(0.0625+0.125*log(1.0+exp(-10.0*Vc+7.5)))*(1.0+exp(-10.0*Vc+7.5)));
        if(it_count == 0) {
           // first iteration of a given timepoint
           if(time == 0) {
               // first time point
               inst->qdot = 0;
           }
           else {
               // subsequent time points
               inst->qdot = (inst->alpha)*Q+(inst->beta);
           }
           intgr8(Q,inst->qdot,h,&inst->alpha,&inst->beta);
        }

        Gk = (inst->alpha)*dQdV;
        Ik = (inst->alpha)*Q-(inst->alpha)*dQdV*Vc+(inst->beta);

        // stamp matrix and rhs
	*(inst->pn1n1) += Gk;
	*(inst->pn2n1) -= Gk;
	*(inst->pn1n2) -= Gk;
	*(inst->pn2n2) += Gk;
	Rhs[inst->pNode] -= Ik;
	Rhs[inst->nNode] += Ik;
    }
}
