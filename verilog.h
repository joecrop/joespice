
//**********************************
// Structure for a Verilog instance
// used in mySpice.
//**********************************
typedef struct verilog {
	FILE *fp_write; 	//pointer to modelSim stdin
	int total_inputs; 	//number of inputs nets to modelSim
	char **inputs; 		//input net names
	int total_outputs; 	//number of outputs from modelSim to be printed
	char **outputs; 	//output net names
	int total_drivers; 	//output nets from modelSim that drive SPICE nets
	char **drivers; 	//driver net names
	double logic_level; 	//analog value of digital switch threshold (i.e. 2.5V)
	double high_level; 	//analog value of a logic high (i.e. 5V)
	double low_level; 	//analog value of a logic low (i.e. 0V)
	int line_num; 		//line number to keep track of modelSim output
} verilog;
