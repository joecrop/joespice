/* information used to describe a single instance */

typedef struct mosfet {
    char *name;  /* pointer to character string naming this mosfet */
    char *mname;  /* pointer to character string for mosfet model */
    int dNode; /* number of drain node of mosfet */
    int gNode; /* number of gate node of mosfet */
    int sNode; /* number of source node of mosfet */
    int bNode; /* number of bulk node of mosfet */
    double L;  /* channel length */
    double W;  /* channel width */
    double Vds; //Vds of last iteration
    double Vgs; //Vgs of last iteration
    double Vbs; //bbs of last iteration
    double Vgd; //Vgd of last iteration
    double Vbd; //Vbd of last iteration
    double *pdd;	/* pointer to sparse matrix element at
                             * (Drain node, drain node) */
    double *pds;	/* pointer to sparse matrix element at
                             * (Drain node, source node) */
    double *pdg; 	/* pointer to sparse matrix element at
                             * (Drain node, gate node) */
    double *pdb; 	/* pointer to sparse matrix element at
                             * (Drain node, bulk node) */
    double *psd; 	/* pointer to sparse matrix element at
                             * (Source node, drain node) */
    double *pss; 	/* pointer to sparse matrix element at
                             * (Source node, source node) */
    double *psg; 	/* pointer to sparse matrix element at
                             * (Source node, gate node) */
    double *psb; 	/* pointer to sparse matrix element at
                             * (Source node, bulk node) */
} mosfet ;

