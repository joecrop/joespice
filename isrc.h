/* information used to describe a single instance */

typedef struct isource{
    char *name;  /* pointer to character string naming this isource */
    int pNode; /* number of positive node of isource */
    int nNode; /* number of negative node of isource */

    double current;  /* value of isource */
} isource ;

