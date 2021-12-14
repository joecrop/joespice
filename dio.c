#include <math.h>
#include "macros.h"
#include "defs.h"
#include "dio.h"
#include "sparse/spMatrix.h"

void makeDio(Dio, numDio, buf)
diode *Dio[];
int numDio;
char *buf;
{
    diode *inst;
    int j, nodeA, nodeB, atoi();
    char name[MAXFIELD], mname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double value, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    j = getNextField(buf, node, j);
    nodeA = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, mname, j);
    j = getNextField(buf, num, j);
    value = atof(num);

    inst = CALLOC(diode, 1);
    inst->name = (char *)strdup(name);
    inst->pNode = nodeA;
    inst->nNode = nodeB;
    inst->mname = (char *)strdup(mname);
    inst->area = value;
    Dio[numDio] = inst;
}

void printDio(Dio, numDio)
diode *Dio[];
int numDio;
{
    int i;
    diode *inst;

    for(i = 1; i <= numDio; i++) {
	inst = Dio[i];
	printf("%s\t%s\t%s\t%s\t%f\n", inst->name, NodeArray[inst->pNode], NodeArray[inst->nNode], inst->mname, inst->area);
    }
}

void setupDio(Matrix, Dio, numDio)
char *Matrix;
diode *Dio[];
int numDio;
{
    int i, n1, n2;
    diode *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numDio; i++) {
	inst = Dio[i];
	n1 = inst->pNode;
	n2 = inst->nNode;
	/* setup matrix and pointers */
	inst->pn1n1 = spGetElement(Matrix, n1, n1);
	inst->pn1n2 = spGetElement(Matrix, n1, n2);
	inst->pn2n2 = spGetElement(Matrix, n2, n2);
	inst->pn2n1 = spGetElement(Matrix, n2, n1);
    }
}

void loadDio(Matrix, Rhs, Dio, numDio, Sol, iter)
char *Matrix;
double *Rhs;
diode *Dio[];
int numDio;
double *Sol;
int iter;
{
    int i;
    int *check;
    diode *inst;
    double G, id, Ieq, v1, v2, vold, vnew;
    double Isat = 1.0*pow(10.0,-16);
    double Vt = 0.0258;
    double vcrit = Vt*log(Vt/(sqrt(2)*Isat));
    double arg;

    /* load matrix */
    for(i = 1; i <= numDio; i++) {
	inst = Dio[i];
	if(iter == 0)
	{
	    v1 = 0.8;
	    v2 = 0.0;
	    vold = 0.0;
	}
	else
	{
	    v1 = Sol[inst->pNode];
	    v2 = Sol[inst->nNode];
	    vold = inst->vd;
	}

	vnew = (v1-v2);
	//pnjlim function
	if((vnew > vcrit) && (ABS(vnew - vold) > (Vt + Vt))) {
            if(vold > 0) {
                arg = 1.0 + (vnew - vold) / Vt;
                if(arg > 0) {
                    vnew = vold + Vt * log(arg);
                } else {
                    vnew = vcrit;
                }
            } else {
                vnew = Vt*log(vnew/Vt);
            }
        }
	inst->vd = vnew;

	G = (Isat/Vt)*exp((vnew)/Vt);
	id = inst->area*Isat*(exp((vnew)/Vt)-1);
	Ieq = id - G*(vnew);

	*(inst->pn1n1) += G; 
	*(inst->pn1n2) -= G; 
	*(inst->pn2n2) += G; 
	*(inst->pn2n1) -= G; 
	Rhs[inst->pNode] -= Ieq;
	Rhs[inst->nNode] += Ieq;
    }
}
