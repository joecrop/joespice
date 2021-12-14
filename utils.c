
#include <stdio.h>
#include <strings.h>
#include "macros.h"
#include "defs.h"
#include "sparse/spMatrix.h"

void remove_extention(char *original, char * split1)
{
  int i = 0;
  int j = 0;
  int flag = 0;

  while (original[i] != '\0')
  {
     if (flag == 0)
     {
        if (original[i] == '.')
        {
           flag = 1;
           split1[i] = '\0';
        }
        else
        {
           split1[i] = original[i];
        }
     }
     i++;
  }
}

int getNextField(input, field, start)
char *input, *field;
int start;
{
    int i, j;
    for(i=0; i < MAXFIELD; i++, j++) {
	field[i] = '\0';
    }
    for(i = 0, j = start; i < MAXFIELD; i++, j++) {
	if (input[j] == ' ' OR input[j] == '\n') break;
	field[i] = input[j];
    }
    /* strip trailing blanks */
    while( input[j] == ' ') j++;
    return( j );
}

/* map a nodename to an integer */
int getMappedNode(nodeName)
char *nodeName;
{
    int i;
    for(i = 0; i <= NumNodes; i++) {
	if(!strcmp(NodeArray[i], nodeName)) return( i );
    }
    /* node doesn't exist in NodeArray - so insert */
    NumNodes++;
    strcpy(NodeArray[NumNodes],  nodeName);
    return(NumNodes);
}

/* map a branch name to an integer */
int getMappedBranch(branchName)
char *branchName;
{
    int i;
    for(i = 0; i <= NumBranches; i++) {
	if(!strcmp(BranchArray[i], branchName)) return( i );
    }
    /* branch doesn't exist in BranchArray - so insert */
    NumBranches++;
    strcpy(BranchArray[NumBranches],  branchName);
    return(NumBranches);
}

BOOLEAN
foundError( error )
int error;
{
    BOOLEAN matrixError;
    switch( error ) {
	case spSMALL_PIVOT:
	    printf( "\n *****Error in Decomp: SMALL_PIVOT" );
	    matrixError = TRUE;
	    break;
	case spPANIC:
	    printf( "\n *****Error in Decomp: RANGE" );
	    matrixError = TRUE;
	    break;
	case spSINGULAR:
	    printf( "\n *****Error in Decomp: SINGULAR" );
	    matrixError = TRUE;
	    break;
	case spZERO_DIAG:
	    printf( "\n *****Error in Decomp: ZERO PIVOT" );
	    matrixError = TRUE;
	    break;
	case spNO_MEMORY:
	    printf( "\n *****Error in Decomp: NO_MEMORY" );
	    matrixError = TRUE;
	    break;
	default:
	    matrixError = FALSE;
	    break;
    }
    return( matrixError );
}
