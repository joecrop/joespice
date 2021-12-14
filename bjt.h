/* information used to describe a single instance */

typedef struct bjt {
    char *name;  /* pointer to character string naming this mosfet */
    char *mname;  /* pointer to character string for mosfet model */
    int cNode; /* number of collector node of mosfet */
    int bNode; /* number of base node of mosfet */
    int eNode; /* number of emitter node of mosfet */
    double Vbe; //Vbe of last iteration
    double Vbc; //Vbc of last iteration
    double *pee;	/* pointer to sparse matrix element at
                             * (Emitter node, Emitter node) */
    double *pec;	/* pointer to sparse matrix element at
                             * (Emitter node, Collector node) */
    double *peb; 	/* pointer to sparse matrix element at
                             * (Emitter node, base node) */
    double *pce; 	/* pointer to sparse matrix element at
                             * (collector node, Emitter node) */
    double *pcc; 	/* pointer to sparse matrix element at
                             * (Collector node, Collector node) */
    double *pcb; 	/* pointer to sparse matrix element at
                             * (Collector node, Base node) */
    double *pbe; 	/* pointer to sparse matrix element at
                             * (Base node, Emitter node) */
    double *pbc; 	/* pointer to sparse matrix element at
                             * (Base node, Collector node) */
    double *pbb; 	/* pointer to sparse matrix element at
                             * (Base node, Base node) */
} bjt ;

