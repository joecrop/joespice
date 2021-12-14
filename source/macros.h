#include <stdio.h>


/* 
 * BOOLEANS
 */

#define  BOOLEAN	int
#define  FALSE		0
#define  TRUE		1
#define  YES            1
#define  NO             0
#define  NOT		!
#define  AND		&&
#define  OR		||
#define  IS		==
#define  ISNOT          !=


#define NIL(type) 	((type *)0)

/* 
 * RELATIONAL
 */

/* Macro functions that return the maximun or minimum independent of type. */
#define  MAX(a,b)           ((a) > (b) ? (a) : (b))
#define  MIN(a,b)           ((a) < (b) ? (a) : (b))

/* Macro function that returns the absolute value of a floating point number. */
#define  ABS(a)             ((a) < 0.0 ? -(a) : (a))
#define  SIGN(a)            ((a) < 0.0 ? -(1.0) : (1.0))

/* Macro procedure that swaps two entities. */
#define  SWAP(type, a, b)   {type swapx; swapx = a; a = b; b = swapx;}


/*
 * MEMORY ALLOCATION
 */

#define MALLOC(type,number)  (type *)malloc((unsigned)(sizeof(type)*(number)))
#define CALLOC(type,number)  (type *)calloc((number), (unsigned)(sizeof(type)))
#define REALLOC(ptr,type,number)  \
           ptr = (type *)realloc((char *)ptr,(unsigned)(sizeof(type)*(number)))
#define FREE(pointer)  \
	   {  if ((pointer) != NULL) free((char *)pointer); (pointer) = NULL; }


