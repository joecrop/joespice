#define MAXLINE 132
#define MAXFIELD 16
#define MAXNODE 100
#define MAXBRANCH 100
#define MAXELEM 100
extern int NumNodes;
extern int NumBranches;
extern char **NodeArray;
extern char **BranchArray;

void intgr8(double x, double xdot, double h, double *alpha, double *beta);
 
