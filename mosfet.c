#include <math.h>
#include "macros.h"
#include "defs.h"
#include "mosfet.h"
#include "sparse/spMatrix.h"


    /* limvds(vnew,vold)
     * limit the per-iteration change of VDS
     * (From Spice3)
     */

double limvds(vnew,vold)
    double vnew;
    double vold;

{

    if(vold >= 3.5) {
        if(vnew > vold) {
            vnew = MIN(vnew,(3 * vold) +2);
        } else {
            if (vnew < 3.5) {
                vnew = MAX(vnew,2);
            }
        }
    } else {
        if(vnew > vold) {
            vnew = MIN(vnew,4);
        } else {
            vnew = MAX(vnew,-.5);
        }
    }
    return(vnew);
}

    /*
     * fetlim(vnew,vold.vto)
     * limit the per-iteration change of FET voltages 
     * (From Spice3)
     */

double fetlim(vnew,vold,vto)
    double vnew;
    double vold;
    double vto;
{
    double vtsthi;
    double vtstlo;
    double vtox;
    double delv;
    double vtemp;

    vtsthi = ABS(2*(vold-vto))+2;
    vtstlo = vtsthi/2 +2;
    vtox = vto + 3.5;
    delv = vnew-vold;

    if (vold >= vto) {
        if(vold >= vtox) {
            if(delv <= 0) {
                /* going off */
                if(vnew >= vtox) {
                    if(-delv >vtstlo) {
                        vnew =  vold - vtstlo;
                    }
                } else {
                    vnew = MAX(vnew,vto+2);
                }
            } else {
                /* staying on */
                if(delv >= vtsthi) {
                    vnew = vold + vtsthi;
                }
            }
        } else {
            /* middle region */
            if(delv <= 0) {
                /* decreasing */
                vnew = MAX(vnew,vto-.5);
            } else {
                /* increasing */
                vnew = MIN(vnew,vto+4);
            }
        }
    } else {
        /* off */
        if(delv <= 0) {
            if(-delv >vtsthi) {
                vnew = vold - vtsthi;
            } 
        } else {
            vtemp = vto + .5;
            if(vnew <= vtemp) {
                if(delv >vtstlo) {
                    vnew = vold + vtstlo;
                }
            } else {
                vnew = vtemp;
            }
        }
    }
    return(vnew);
}

void makeMosfet(Mos, numMos, buf)
mosfet *Mos[];
int numMos;
char *buf;
{
    mosfet *inst;
    int j, nodeG, nodeD, nodeS, nodeB, atoi();
    char name[MAXFIELD], mname[MAXFIELD], node[MAXFIELD], num[MAXFIELD];
    double width, length, atof();

    j = 0;	
    j = getNextField(buf, name, j);
    j = getNextField(buf, node, j);
    nodeD = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeG = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeS = getMappedNode(node);
    j = getNextField(buf, node, j);
    nodeB = getMappedNode(node);
    j = getNextField(buf, mname, j);
    j = getNextField(buf, num, j);
    width = atof(num);
    j = getNextField(buf, num, j);
    length = atof(num);

    inst = CALLOC(mosfet, 1);
    inst->name = (char *)strdup(name);
    inst->dNode = nodeD;
    inst->gNode = nodeG;
    inst->sNode = nodeS;
    inst->bNode = nodeB;
    inst->mname = (char *)strdup(mname);
    inst->W = width;
    inst->L = length;
    Mos[numMos] = inst;
}

void printMosfet(Mos, numMos)
mosfet *Mos[];
int numMos;
{
    int i;
    mosfet *inst;

    for(i = 1; i <= numMos; i++) {
	inst = Mos[i];
	printf("%s\t%s\t%s\t%s\t%s\t%s\t%f\t%f\n", inst->name, NodeArray[inst->dNode], NodeArray[inst->gNode], NodeArray[inst->sNode], NodeArray[inst->bNode], inst->mname, inst->W, inst->L);
    }
}

void setupMosfet(Matrix, Mos, numMos)
char *Matrix;
mosfet *Mos[];
int numMos;
{
    int i, d, g, s, b;
    mosfet *inst;

    /* do any preprocessing steps here */
    for(i = 1; i <= numMos; i++) {
	inst = Mos[i];
	d = inst->dNode;
	g = inst->gNode;
	s = inst->sNode;
	b = inst->bNode;
	/* setup matrix and pointers */
	inst->pdd = spGetElement(Matrix, d, d);
	inst->pds = spGetElement(Matrix, d, s);
	inst->pdg = spGetElement(Matrix, d, g);
	inst->pdb = spGetElement(Matrix, d, b);
	inst->psd = spGetElement(Matrix, s, d);
	inst->pss = spGetElement(Matrix, s, s);
	inst->psg = spGetElement(Matrix, s, g);
	inst->psb = spGetElement(Matrix, s, b);
	inst->Vds = 0;
	inst->Vgs = 0;
	inst->Vbs = 0;
	inst->Vbd = 0;
	inst->Vbs = 0;
    }
}

void loadMosfet(Matrix, Rhs, Mosfet, numMosfet, Xk, iter)
char *Matrix;
double *Rhs;
mosfet *Mosfet[];
int numMosfet;
double* Xk;
int iter;
{
  int i, g, d, s, b;
  mosfet *inst;
  double gm, gds, gmbs, Ik, Id, Vgs, Vgd, Vds, Vbs, Vbd;
  double lambda, K, Vto, gamma, phi, W, L, beta, betap, vgst;
  double type, norm, rev, von, arg, sarg;
  int j;

  for(i = 1; i <= numMosfet; i++) {
    inst = Mosfet[i];
    g = inst->gNode;
    d = inst->dNode;
    s = inst->sNode;
    b = inst->bNode;
        
    W = inst->W;
    L = inst->L;

    gamma = 0.5;
    phi = 0.6;

    if (!strcmp(inst->mname, "pmos")) {
      /* case pmos */
      lambda = 0.05;
      K = 50.0e-6;
      Vto = -0.9;
      type = -1.0;
    } else {
      /* case nmos */
      lambda = 0.05;
      K = 100.0e-6;
      Vto = 0.7;
      type = 1;
    }

    Vgs = Xk[g]-Xk[s];
    Vgd = Xk[g]-Xk[d];
    Vds = Xk[d]-Xk[s];
    Vbs = Xk[b]-Xk[s];
    Vbd = Xk[b]-Xk[d];

    if(iter == 0)
    {
        Vds = 5;
        Vgs = 4;
        Vgd = 1;
        Vbs = 0;
        Vbd = -5;
    }


    beta = K*W/L;
    Vto = type*Vto;
    Vgs = type*Vgs;
    Vgd = type*Vgd;
    Vds = type*Vds;
    Vbs = type*Vbs;
    Vbd = type*Vbd;
    
    norm = 1.0;
    rev = 0.0;
    if(Vds < 0.0 ) {
	/* reverse mode */
	Vds = -Vds;
	Vgs = Vgd;
	Vbs = Vbd;
	norm = 0.0;
	rev = 1.0;
    }
    /* calculation of Vt for Vsb < 0 */

    if (Vbs <= 0 ) {
	sarg=sqrt(phi-Vbs);
    } else {
	sarg=sqrt(phi);
	sarg=sarg-Vbs/(sarg+sarg);
	sarg=MAX(0,sarg);
    }
    von=Vto+gamma*(sarg-sqrt(phi));
    vgst=Vgs-von;
    if (sarg <= 0) {
	arg=0;
    } else {
	arg=gamma/(sarg+sarg);
    }
    if (vgst <= 0) {
	/*
	 *     cutoff region
	 */
	Id=0;
	gm=0;
	gds=0;
	gmbs=0;
    } else{
	/*
	 *     saturation region
	 */
	betap=beta*(1+lambda*Vds);
	if (vgst <= Vds) {
	    Id=betap*vgst*vgst*.5;
	    gm=betap*vgst;
	    gds=lambda*beta*vgst*vgst*.5;
	    gmbs=gm*arg;
	} else {
	/*
	 *     linear region
	 */
	    Id=betap*Vds*(vgst-.5*Vds);
	    gm=betap*Vds;
	    gds=betap*(vgst-Vds)+lambda*beta*Vds*(vgst-.5*Vds);
	    gmbs=gm*arg;
	}
    }
    if (norm == 1.0) {
	Ik = type*(Id-gm*Vgs-gds*Vds-gmbs*Vbs);    
    } else {
	Ik = -type *(Id-gm*Vgs-gds*Vds-gmbs*Vbs);    
    }
    /* stamp mosfet*/
    *(inst->pdd) += gds + rev*(gm+gmbs);
    *(inst->pds) -= gds + norm*(gm+gmbs);
    *(inst->pdg) += (norm - rev)*gm;
    *(inst->pdb) += (norm - rev)*gmbs;
    *(inst->psd) -= gds + rev*(gm+gmbs);
    *(inst->pss) += gds + norm*(gm+gmbs);
    *(inst->psg) -= (norm - rev)*gm;
    *(inst->psb) -= (norm - rev)*gmbs;
    
    Rhs[d] -= Ik;
    Rhs[s] += Ik;
  }
}
