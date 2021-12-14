#include <stdio.h>
#include "macros.h"
#include "defs.h"
#include "res.h"
#include "isrc.h"
#include "vsrc.h"
#include "vcvs.h"
#include "cccs.h"
#include "vccs.h"
#include "ccvs.h"
#include "dio.h"
#include "transformer.h"
#include "gyrator.h"
#include "ABCD.h"
#include "mosfet.h"
#include "bjt.h"
#include "nonlincap.h"
#include "lincap.h"
#include "linind.h"
#include "initcond.h"
#include "sparse/spMatrix.h"
#include "verilog.h"
#include <assert.h>

#define MAX_ITERATIONS 2000

int NumNodes = 0;
int NumBranches = 0;
char **NodeArray;
char **BranchArray;

main(ac, av)
char **av;
{
	char *inFile;
	FILE *fopen(), *fpIn;
	char buf[MAXLINE];
	resistor 	*Res[MAXELEM];
	isource	*Isrc[MAXELEM];
	vsource	*Vsrc[MAXELEM];
	vcvs	*Esrc[MAXELEM];
	cccs	*Fsrc[MAXELEM];
	vccs	*Gsrc[MAXELEM];
	ccvs	*Hsrc[MAXELEM];
	diode    	*Dio[MAXELEM]; 
	transformer	*Nsrc[MAXELEM];
	gyrator	*Tsrc[MAXELEM];
	ABCD	*Psrc[MAXELEM];
	mosfet	*Mosfet[MAXELEM];
	bjt	*BJT[MAXELEM];
	nonlincap	*NonLinCap[MAXELEM];
	lincap	*LinCap[MAXELEM];
	linind	*LinInd[MAXELEM];
	initcond	*InitCond[MAXELEM];
	int i = 0;
	int j = 0;
	int numRes = 0;
	int numIsrc = 0;
	int numVsrc = 0;
	int numEsrc = 0;
	int numFsrc = 0;
	int numGsrc = 0;
	int numHsrc = 0;
	int numDio = 0;
	int numNsrc = 0;
	int numTsrc = 0;
	int numPsrc = 0;
	int numMosfet = 0;
	int numBJT = 0;
	int numNonLinCap = 0;
	int numLinCap = 0;
	int numLinInd = 0;
	int numInitCond = 0;
	int numEqns;
	int newtonNotFinished = 1;
	char *cktMatrix;
	double *Rhs, *Sol, *SolOld;
	double tol;
	double abstol = 1e-6;
	double reltol = 1e-3;
	BOOLEAN foundError();
	int error;
	double time = 0.0;
	double t_step = 0.0;
	double t_stop = 0.0;
	char top_level[100];
	int size;
	double value, value2;
	int verilog_cosim = 0;
	verilog Verilog;

	switch (ac) {
		case 2:
			inFile = av[1];
		break;
		default:
			printf( "\n File Name Required \n");
			exit(-1);
	}
	/* initialization */
	NodeArray = CALLOC(char *, MAXNODE);
	BranchArray = CALLOC(char *, MAXBRANCH);
	for(i = 0; i < MAXNODE; i++) {
		NodeArray[i] = CALLOC(char, MAXFIELD);
	}
	for(i = 0; i < MAXBRANCH; i++ ) {
		BranchArray[i] = CALLOC(char, MAXFIELD) ;
	}
	/* fill in ground node */
	strcpy(NodeArray[0], (char *)"0");

	Verilog.inputs = CALLOC(char *, 100);
	Verilog.outputs = CALLOC(char *, 100);
	Verilog.drivers = CALLOC(char *, 100);
	for(i = 0; i < 100; i++) {
		Verilog.inputs[i] = CALLOC(char, 100);
		Verilog.outputs[i] = CALLOC(char, 100);
		Verilog.drivers[i] = CALLOC(char, 100);
	}

	fpIn = fopen( inFile, "r" );
	while (fgets( buf, MAXLINE, fpIn ) != NULL) {
		if(tolower(buf[0]) == 'r') 
		{
			// resistor
			numRes++;
			makeRes(Res, numRes, buf);
		} 
		else if(tolower(buf[0]) == 'i') 
		{
			// isource
			numIsrc++;
			makeIsrc(Isrc, numIsrc, buf);
		}
		else if(tolower(buf[0]) == 'v') 
		{
			// vsource
			numVsrc++;
			makeVsrc(Vsrc, numVsrc, buf);
		}
		else if(tolower(buf[0]) == 'e') 
		{
			// vcvs
			numEsrc++;
			makeEsrc(Esrc, numEsrc, buf);
		}
		else if(tolower(buf[0]) == 'f') 
		{
			// cccs
			numFsrc++;
			makeFsrc(Fsrc, numFsrc, buf);
		}
		else if(tolower(buf[0]) == 'g') 
		{
			// vccs
			numGsrc++;
			makeGsrc(Gsrc, numGsrc, buf);
		}
		else if(tolower(buf[0]) == 'h') 
		{
			// ccvs
			numHsrc++;
			makeHsrc(Hsrc, numHsrc, buf);
		}
		else if(tolower(buf[0]) == 'd') 
		{
			// diode
			numDio++;
			makeDio(Dio, numDio, buf);
		}
		else if(tolower(buf[0]) == 'n') 
		{
			// transformer
			numNsrc++;
			makeNsrc(Nsrc, numNsrc, buf);
		}
		else if(tolower(buf[0]) == 't') 
		{
			// gyrator
			numTsrc++;
			makeTsrc(Tsrc, numTsrc, buf);
		}
		else if(tolower(buf[0]) == 'p') 
		{
			// ABCD 2-port
			numPsrc++;
			makePsrc(Psrc, numPsrc, buf);
		}
		else if(tolower(buf[0]) == 'm') 
		{
			// Mosfet
			numMosfet++;
			makeMosfet(Mosfet, numMosfet, buf);
		}
		else if(tolower(buf[0]) == 'q') 
		{
			// BJT
			numBJT++;
			makeBJT(BJT, numBJT, buf);
		}
		else if(tolower(buf[0]) == 'c') 
		{
			// Linear Cap
			numLinCap++;
			makeLinCap(LinCap, numLinCap, buf);
		}
		else if(tolower(buf[0]) == 'l') 
		{
			// Linear Cap
			numLinInd++;
			makeLinInd(LinInd, numLinInd, buf);
		}
		else if(tolower(buf[0]) == 'u') 
		{
			// Linear Cap
			numNonLinCap++;
			makeNonLinCap(NonLinCap, numNonLinCap, buf);
		}
		else if(tolower(buf[0]) == '.') 
		{
			// SPICE command
    			char name[MAXFIELD];
			int jt = 1;
			double atof();
    			jt = getNextField(buf, name, j);
			if(strcmp(name, ".tran") == 0) //setup tran sim.
			{
				printf("Transient Simulation Selected...");
    				jt = getNextField(buf, name, jt);
    				t_step = atof(name);
    				jt = getNextField(buf, name, jt);
    				t_stop = atof(name);
				printf("step: %0.12f\tstop: %0.12f\n", t_step, t_stop);
			}
			else if(strcmp(name, ".ic") == 0) //setup initial conditions
			{
				// initial voltage source
				numInitCond++;
				makeInitCond(InitCond, numInitCond, buf);
			}
			else if(strcmp(name, ".vfile") == 0) //add verilog file to simulation
			{
				printf("Verilog Co-Simulation Selected...\n");
				jt = getNextField(buf, name, jt);
				jt = getNextField(buf, top_level, jt);
				verilog_setup(&Verilog, name, top_level);
				verilog_cosim = 1;
			}
			else if(strcmp(name, ".vin") == 0) //add verilog input net
			{
    				jt = getNextField(buf, name, jt);
    				size = atoi(name);
    				jt = getNextField(buf, name, jt);
				verilog_add_input(&Verilog, name, size);
			}
			else if(strcmp(name, ".vout") == 0) //add verilog output net
			{
    				jt = getNextField(buf, name, jt);
    				size = atoi(name);
    				jt = getNextField(buf, name, jt);
				verilog_add_output(&Verilog, name, size);
			}
			else if(strcmp(name, ".vdrive") == 0) //add verilog driver net
			{
    				jt = getNextField(buf, name, jt);
    				size = atoi(name);
				assert(size == 1);
    				jt = getNextField(buf, name, jt);
				verilog_add_driver(&Verilog, name);
				jt=sprintf(buf, "V%s %s 0 0", name, name);
				numVsrc++;
				makeVsrc(Vsrc, numVsrc, buf);
			}
			else if(strcmp(name, ".vlevel") == 0) //add verilog levels
			{
    				jt = getNextField(buf, name, jt);
    				value = atof(name);
    				jt = getNextField(buf, name, jt);
    				value2 = atof(name);
    				Verilog.low_level = value;
    				Verilog.high_level = value2;
    				Verilog.logic_level = (value2 - value)/2.0;
				printf("Verilog Logic Levels - Low:%f\t high:%f\t threshold:%f\n", value, value2, Verilog.logic_level);
			}
		}
	}
	fclose( fpIn );

	// print circuit elements
	printRes(Res, numRes);
	printIsrc(Isrc, numIsrc); 
	printVsrc(Vsrc, numVsrc); 
	printEsrc(Esrc, numEsrc); 
	printFsrc(Fsrc, numFsrc); 
	printGsrc(Gsrc, numGsrc); 
	printHsrc(Hsrc, numHsrc); 
	printDio(Dio, numDio);
	printNsrc(Nsrc, numNsrc); 
	printTsrc(Tsrc, numTsrc); 
	printPsrc(Psrc, numPsrc); 
	printMosfet(Mosfet, numMosfet); 
	printBJT(BJT, numBJT); 
	printNonLinCap(NonLinCap, numNonLinCap); 
	printLinCap(LinCap, numLinCap); 
	printLinInd(LinInd, numLinInd); 
	printInitCond(InitCond, numInitCond); 

	// setup circuit matrix
	numEqns = NumNodes+NumBranches;
	cktMatrix = spCreate( numEqns, 0, &error );
	if( error IS spNO_MEMORY ) {
		printf( "\n: --- NO MEMORY ---" );
		exit( -1 );
	}
	// Allocate RHS and Solution vectors
	Rhs = CALLOC(double, numEqns+1);
	Sol = CALLOC(double, numEqns+1);
	SolOld = CALLOC(double, numEqns+1);

	// do any preprocessing
	setupRes(cktMatrix, Res, numRes);
	setupVsrc(cktMatrix, Vsrc, numVsrc);
	setupEsrc(cktMatrix, Esrc, numEsrc);
	setupFsrc(cktMatrix, Fsrc, numFsrc);
	setupGsrc(cktMatrix, Gsrc, numGsrc);
	setupHsrc(cktMatrix, Hsrc, numHsrc);
	setupIsrc(cktMatrix, Isrc, numIsrc);
	setupDio(cktMatrix, Dio, numDio);
	setupNsrc(cktMatrix, Nsrc, numNsrc);
	setupTsrc(cktMatrix, Tsrc, numTsrc);
	setupPsrc(cktMatrix, Psrc, numPsrc);
	setupMosfet(cktMatrix, Mosfet, numMosfet);
	setupBJT(cktMatrix, BJT, numBJT);
	setupNonLinCap(cktMatrix, NonLinCap, numNonLinCap);
	setupLinCap(cktMatrix, LinCap, numLinCap);
	setupLinInd(cktMatrix, LinInd, numLinInd);
	setupInitCond(cktMatrix, InitCond, numInitCond);

	//iteration through time
	for(time = 0; time < t_stop; time = time+t_step)
	{
		// find DC solution - Newton Loop
		for(i = 0; i < MAX_ITERATIONS; i++) {
			// stamp circuit matrix
			spClear(cktMatrix);
			loadRes(cktMatrix, Rhs, Res, numRes);
			loadIsrc(cktMatrix, Rhs, Isrc, numIsrc);
			loadVsrc(cktMatrix, Rhs, Vsrc, numVsrc);
			loadVsrc(cktMatrix, Rhs, Vsrc, numVsrc);
			loadEsrc(cktMatrix, Rhs, Esrc, numEsrc);
			loadFsrc(cktMatrix, Rhs, Fsrc, numFsrc);
			loadGsrc(cktMatrix, Rhs, Gsrc, numGsrc);
			loadHsrc(cktMatrix, Rhs, Hsrc, numHsrc);
			loadDio(cktMatrix, Rhs, Dio, numDio, SolOld, i);
			loadNsrc(cktMatrix, Rhs, Nsrc, numNsrc);
			loadTsrc(cktMatrix, Rhs, Tsrc, numTsrc);
			loadPsrc(cktMatrix, Rhs, Psrc, numPsrc);
			loadMosfet(cktMatrix, Rhs, Mosfet, numMosfet,  SolOld, i);
			loadBJT(cktMatrix, Rhs, BJT, numBJT,  SolOld, i);
			loadNonLinCap(cktMatrix, Rhs, NonLinCap, numNonLinCap,  SolOld, i, t_step, time);
			loadLinCap(cktMatrix, Rhs, LinCap, numLinCap,  SolOld, i, t_step);
			loadLinInd(cktMatrix, Rhs, LinInd, numLinInd,  SolOld, i, t_step);
			loadInitCond(cktMatrix, Rhs, InitCond, numInitCond, time);

			if(i==0 && time==0) {
				// print circuit matrix
				printf("\n");
				printf("Printing Stamped Matrix...\n");
				spPrint(cktMatrix, 0, 1, 0);
				printf("RHS\n");
				for (j=1; j<=numEqns; j++) {
					SolOld[j] = 0;
					printf("%g\n", Rhs[j]);
				}
				printf("\n");
			}
			// compute DC solution
			// first Factor the matrix and then Forward/Back solve
			error = spFactor( cktMatrix );
			if( foundError( error ) ) {
				exit( -1 );
			}
			spSolve( cktMatrix, Rhs, Sol );
			if(i==0 && time==0) {
				printf("Number of Fill Ins: %d \n",spFillinCount(cktMatrix));
				printf("Solving\n");
			}
	
			newtonNotFinished = 0;
			for (j=1; j<=numEqns; j++)
			{
				tol = abstol + reltol * MAX(ABS(Sol[j]), ABS(SolOld[j]));
				if(ABS(Sol[j]-SolOld[j]) >= tol)
					newtonNotFinished = 1;
				SolOld[j]=Sol[j];
				Sol[j]=0;
				Rhs[j]=0;
			}
		//	printf(".");
			if(newtonNotFinished == 0)
				break;
		}
		if(time == 0) {
			printDCSolution(i, numEqns, SolOld, BranchArray, NodeArray, NumNodes);
		} else {
			printTranSolution(i, numEqns, SolOld, BranchArray, NodeArray, NumNodes, time);
		}
		if(verilog_cosim == 1) //interface to co-simulation
		{
			verilog_update_time(&Verilog, SolOld, NodeArray, NumNodes, time, Vsrc, numVsrc);
		}
	}
	printf("\n");
}

void printDCSolution(i, numEqns, SolOld, BranchArray, NodeArray, NumNodes)
int i;
int numEqns;
double *SolOld;
char **BranchArray;
char **NodeArray;
int NumNodes;
{
	//printf("\n");
	if(i<MAX_ITERATIONS) {
		// solution found
		// print solution
		printf("----Solution-------------------------------\n");
		printf("time\t");
		for(i = 1; i<=numEqns; i++) {
			if(strlen(NodeArray[i]) < 1)
				printf("I(%s)\t", BranchArray[i-(NumNodes)]);
			else
				printf("V(%s)\t", NodeArray[i]);
		}
		printf("\n0.0\t");
		for(i = 1; i<=numEqns; i++) {
			if(strlen(NodeArray[i]) < 1)
				printf("%0.12g\t", SolOld[i]);
			else
				printf("%0.12g\t", SolOld[i]);
		}
	} else {
		printf("Solution did not converge!\n");
	}
}

void printTranSolution(i, numEqns, SolOld, BranchArray, NodeArray, NumNodes, time)
int i;
int numEqns;
double *SolOld;
char **BranchArray;
char **NodeArray;
int NumNodes;
double time;
{
//	printf("\n");
	if(i<MAX_ITERATIONS) {
		printf("\n%0.12g\t", time);
		for(i = 1; i<=numEqns; i++) {
			if(strlen(NodeArray[i]) < 1)
				printf("%0.12g\t", SolOld[i]);
			else
				printf("%0.12g\t", SolOld[i]);
		}
	} else {
		printf("Solution did not converge!\n");
	}
}
