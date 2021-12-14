#include <math.h>
#include "macros.h"
#include "defs.h"
#include "bjt.h"
#include "sparse/spMatrix.h"

    /* pnjlim(vnew,vold,vt,vcrit,icheck)
     * limit the per-iteration change of PN junction  voltages 
     * (From Spice3)
     * vcrit=Vt*log(Vt/(sqrt(2)*Is))
     * icheck is a flag that is set to 1 if limiting is performed else
     * it is set to 0
     */


double pnjlim(vnew,vold,vt,vcrit)
    double vnew;
    double vold;
    double vt;
    double vcrit;
{
    double arg;

    if((vnew > vcrit) && (ABS(vnew - vold) > (vt + vt))) {
        if(vold > 0) {
            arg = 1.0 + (vnew - vold) / vt;
            if(arg > 0) {
                vnew = vold + vt * log(arg);
            } else {
                vnew = vcrit;
            }
        } else {
            vnew = vt *log(vnew/vt);
        }
    }
    return(vnew);
}

void makeBJT(BJT, numBJT, buf)
bjt *BJT[];
int numBJT;
char *buf;
{
    bjt *inst;
    int j, nodeE, nodeC, nodeB, atoi();
    char name[MAXFIELD], mname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];

    j = 0;	
    j = getNextField(buf, name, j);
    j = getNextField(buf, node, j);
    nodeC = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeE = getMappedNode(node);
    j = getNextField(buf, mname, j);

    inst = CALLOC(bjt, 1);
    inst->name = (char *)strdup(name);
    inst->eNode = nodeE;
    inst->cNode = nodeC;
    inst->bNode = nodeB;
    inst->mname = (char *)strdup(mname);
    BJT[numBJT] = inst;
}

void printBJT(BJT, numBJT)
bjt *BJT[];
int numBJT;
{
    int i;
    bjt *inst;

    for(i = 1; i <= numBJT; i++) {
	inst = BJT[i];
	printf("%s\t%s\t%s\t%s\t%s\n", inst->name, NodeArray[inst->cNode], NodeArray[inst->eNode], NodeArray[inst->bNode], inst->mname);
    }
}

void setupBJT(Matrix, BJT, numBJT)
char *Matrix;
bjt *BJT[];
int numBJT;
{
    int i, c, e, b;
    bjt *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numBJT; i++) {
	inst = BJT[i];
	c = inst->cNode;
	e = inst->eNode;
	b = inst->bNode;
	/* setup matrix and pointers */
	inst->pee = spGetElement(Matrix, e, e);
	inst->pec = spGetElement(Matrix, e, c);
	inst->peb = spGetElement(Matrix, e, b);
	inst->pce = spGetElement(Matrix, c, e);
	inst->pcc = spGetElement(Matrix, c, c);
	inst->pcb = spGetElement(Matrix, c, b);
	inst->pbe = spGetElement(Matrix, b, e);
	inst->pbc = spGetElement(Matrix, b, c);
	inst->pbb = spGetElement(Matrix, b, b);
	inst->Vbc = 0;
	inst->Vbe = 0;
    }
}

void loadBJT(Matrix, Rhs, BJT, numBJT, Xk, iter)
char *Matrix;
double *Rhs;
bjt *BJT[];
int numBJT;
double* Xk;
int iter;
{
  int i, c, e, b;
  bjt *inst;
  double gee, gec, gcc, gce, ie, ic, Ie, Ic, Vbc, Vbe;
  int j;
  double Vt = 0.0258;
  double Is = 1.0e-16;
  double af = 0.99;
  double ar = 0.01;
  double vcrit = Vt*log(Vt/(sqrt(2)*Is));

  for(i = 1; i <= numBJT; i++) {
    inst = BJT[i];
    e = inst->eNode;
    c = inst->cNode;
    b = inst->bNode;

    Vbc = pnjlim(Xk[b]-Xk[c],inst->Vbc,Vt,vcrit);
    Vbe = pnjlim(Xk[b]-Xk[e],inst->Vbe,Vt,vcrit);

    inst->Vbc = Vbc;
    inst->Vbe = Vbe;

    gee = (Is/(af*Vt))*exp(Vbe/Vt);
    gec = (Is/Vt)*exp(Vbc/Vt);
    gce = (Is/Vt)*exp(Vbe/Vt);
    gcc = (Is/(ar*Vt))*exp(Vbc/Vt);

    ie = -(Is/af)*(exp(Vbe/Vt)-1)+Is*(exp(Vbc/Vt)-1);
    ic = Is*(exp(Vbe/Vt)-1)-(Is/ar)*(exp(Vbc/Vt)-1);
    Ie = ie + gee*Vbe - gec*Vbc;
    Ic = ic - gce*Vbe + gcc*Vbc;

    /* stamp bjt*/
    *(inst->pee) += gee;
    *(inst->pec) -= gec;
    *(inst->peb) += (gec-gee);
    *(inst->pce) -= gce;
    *(inst->pcc) += gcc;
    *(inst->pcb) += (gce-gcc);
    *(inst->pbe) += (gce-gee);
    *(inst->pbc) += (gec-gcc);
    *(inst->pbb) += (+gcc+gee-gce-gec);
    
    Rhs[e] -= Ie;
    Rhs[c] -= Ic;
    Rhs[b] += (Ic+Ie);
  }
}
