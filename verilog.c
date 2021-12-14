#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "verilog.h"
#include "vsrc.h"
#include "defs.h"
#include "macros.h"

//********************************************************************
// Extract a substring from an existing string, return the length of the substring
// dst: receive the substring
// src: the source string to extract from
// start: extract start with the 'src' + 'start' char, zero based; no bound check
// len: length of the substring; may be shorter than expected.
// Ansi string only!
//********************************************************************
int substr(char* dst, char* src, int start, int len)
{
	char* d = dst;
	char* s = src + start;
	while (len-- > 0)
	{
		if (*s == '\n')
		{
			*d = '\0';
			return d - dst;
		}
		*d++ = *s++;
	}
	*d = '\0';
	return d - dst;
}

//********************************************************************
// Forces the receiving end of a pipe to read buffer by sending more
// characters than can fit in the buffer.
// Verilog: a verilog instance that has a modelSim stdin pipe.
//********************************************************************
void flush_pipe(verilog *Verilog)
{
	int i=0;
	int n;
	char buffer [100];
	while(i < 100)
	{
		i++;
		n=sprintf (buffer, "                                         \n");
		fputs(buffer, Verilog->fp_write);
	}
}

//********************************************************************
// Helper function to check if a file exists.
// filename: a string of the file to be checked.
//********************************************************************
int does_file_exist(char *filename)
{
	FILE *fp_read;
	if(fp_read = fopen(filename, "r"))
	{
		fclose(fp_read);
		return 1;
	}
	return 0;
}

//********************************************************************
// Helper function to make sure modelSim output is progressing.
// Because it can be slow mySpice often need to wait for the output
// file to be written.
// Verilog: a verilog instance that has a handle on a modelSim instance.
//********************************************************************
void increment_line_counter(Verilog)
verilog *Verilog;
{
	int lines = 0;
	FILE *fp_read;
	char line [122];
	//once there are more lines in the modelSim output transcript we know it has been written to.
	while (Verilog->line_num >= lines) //wait for buffer to be updated
	{
		fp_read = fopen("transcript", "r"); //open the modelSim transcript
		lines = 0;
		while ( fgets(line, 122, fp_read) != NULL) //read each line
		{
			lines++;
		}
		fclose(fp_read);
	}
	Verilog->line_num = lines; // update line counter for next buffer read
}

//********************************************************************
// Setup function to initialize modelSim and Verilog instance.
// Verilog: a verilog instance to be initialized.
// files: verilog file(s) to be compiled into modelSim
// top_level: top level module name to be initialized with modelSim.
//********************************************************************
void verilog_setup(Verilog, files, top_level)
verilog *Verilog;
char *files;
char *top_level;
{
	char buffer [100];
	char line [128];
	int n;
	assert(system("rm -f transcript") != -1); //clean directory from last simulation
	assert(system("vlib work") != -1); //setup modelSim library
	n=sprintf (buffer, "vlog %s", files); //compile verilog files
	assert(system(buffer) != -1);
	n=sprintf (buffer, "vsim -c %s > /dev/null", top_level); //start modelsim command
	printf("%s\n", buffer);

	Verilog->fp_write = popen(buffer, "w"); //open modelSim and retain pipe to its stdin
	printf("Starting ModelSim...");

	while(!does_file_exist("transcript")){} //wait for modelSim to start
	printf("Waiting...");

	while (line[2] != 'L') //wait for modelSim to intialize
	{
		FILE *fp_read = fopen("transcript", "r"); //open modelSim trascript
		Verilog->line_num = 0;
		while ( fgets( line, sizeof line, fp_read)) //get initial amount of lines in modelSim output
		{
			Verilog->line_num++;
		}
		fclose(fp_read);
	}
	printf("Started.\n");

	//initialize Verilog nets to 0
	Verilog->total_inputs = 0;
	Verilog->total_outputs = 0;
	Verilog->total_drivers = 0;

}

//********************************************************************
// Gets the digital value of a Verilog net or bus.
// Verilog: a verilog instance with a modelSim connection.
// var: the name of the variable to be read.
// result: a string that will be set with either "1", "0", or "X"
// after the veraible has been read.
//********************************************************************
void verilog_get_var(Verilog, var, result)
verilog *Verilog;
char *var;
char *result;
{
	char buffer [100];
	char line[130];
	int n;
	int lines = 0;
	FILE *fp_read;
	int line_found;

	n=sprintf (buffer, "examine %s\n", var); //examine a bus/net
	fputs(buffer, Verilog->fp_write);

	flush_pipe(Verilog);// force command to be read
toploop:
	line_found = 0;
	while (Verilog->line_num >= lines || line_found == 0) //wait for modelSim
	{
		fp_read = fopen("transcript", "r");
		lines = 0;
		while ( fgets(line, 130, fp_read) != NULL)
		{
			if(strstr(line, "examine") != NULL && Verilog->line_num < lines) //modelSim command has been executed
			{
				line_found == 1;
				goto outloop;
			}
			lines++;
		}
		fclose(fp_read);
	}
outloop:
	fgets(line, 130, fp_read); //get the next line with the result
	if(strstr(line, "examine") != NULL) //whoops, the transcript isn't quite finished, go back
	{
		goto toploop;
	}
	substr(result, line, 2, 128); //get the result, chop off the junk at the beginning.
	fclose(fp_read);
} 

//********************************************************************
// Set a variable in modelSim as a result of SPICE interpretation.
// Verilog: a verilog instance with a modelSim connection.
// var: the name of the variable to be written.
// data: a string that sets the Verilog net with either "1" or "0".
//********************************************************************
void verilog_set_var(Verilog, var, data)
verilog *Verilog;
char *var;
char *data;
{
	char buffer [100];
	int n;
	int i=0;
	n=sprintf (buffer, "force %s %s\n", var, data); //force the veriable to a value
	fputs(buffer, Verilog->fp_write); //write it to modelSim

	flush_pipe(Verilog); //force a modelSim read
		
	increment_line_counter(Verilog); //wait for modelSim to compute and update its transcript
}

//********************************************************************
// Step modelSim forward in time to stay in synch with mySpice.
// Verilog: a verilog instance with a modelSim connection.
// time_step: how long to step forward in time.
// time_scale: the time scale of the simulator.
//********************************************************************
void verilog_step_time(Verilog, time_step, time_scale)
verilog *Verilog;
double time_step;
double time_scale;
{
	char buffer [100];
	int n;
	n=sprintf (buffer, "run %d\n", (int)round(time_step/time_scale)); //step the simulator forward
	fputs(buffer, Verilog->fp_write);

	flush_pipe(Verilog); //force the command to be read

	increment_line_counter(Verilog); //wait for completion
}

//********************************************************************
// Add an input net to be manipulated in modelSim.
// Verilog: a verilog instance with a modelSim connection.
// name: the name of the net (needs to be the same from mySpice to modelSim.
// size: the bit-width of the bus (note: only 1 bit is currently supported).
//********************************************************************
void verilog_add_input(Verilog, name, size)
verilog *Verilog;
char *name;
int size;
{
	assert(size == 1); //only single bit wide buses are supported to simplify modelSim parsing
	int i, n;
	n=sprintf (Verilog->inputs[Verilog->total_inputs], "%s", name); //save name into array
	printf("input %d:\t%s\n", Verilog->total_inputs, Verilog->inputs[Verilog->total_inputs]); //print it to the user
	Verilog->total_inputs++; //increment number of inputs
}

//********************************************************************
// Add an output net to be manipulated in modelSim.
// Verilog: a verilog instance with a modelSim connection.
// name: the name of the net (needs to be the same from mySpice to modelSim.
// size: the bit-width of the bus (currently not used for generic outputs).
//********************************************************************
void verilog_add_output(Verilog, name, size)
verilog *Verilog;
char *name;
int size;
{
	int i, n;
	n=sprintf(Verilog->outputs[Verilog->total_outputs], "%s", name); //save name to array
	printf("output %d:\t%s\n", Verilog->total_outputs, Verilog->outputs[Verilog->total_outputs]); //print it to the user
	Verilog->total_outputs++; //increment number of outputs
}

//********************************************************************
// Add a driver net to be manipulated in modelSim.
// Verilog: a verilog instance with a modelSim connection.
// name: the name of the net (needs to be the same from mySpice to modelSim.
// (note: only 1 bit nets are currently supported).
//********************************************************************
void verilog_add_driver(Verilog, name)
verilog *Verilog;
char *name;
{
	int n;
	n=sprintf(Verilog->drivers[Verilog->total_drivers], "%s", name); //save to name array
	printf("driver %d:\t%s\n", Verilog->total_drivers, Verilog->drivers[Verilog->total_drivers]); //print to the user
	Verilog->total_drivers++; //increment number of drivers
}

//********************************************************************
// Close a verilgo connection to modelSim
// Verilog: a verilog instance with a modelSim connection.
//********************************************************************
int verilog_close(Verilog)
verilog *Verilog;
{
	return pclose(Verilog->fp_write);
}

//********************************************************************
// This is where all the magic happens...This function updates the 
// interface between mySpice and modelSim.
// Verilog: a verilog instance with a modelSim connection.
// Sol: the Solution vector from the last iteration of the transient simulation.
// NodeArray: the array of node names used to lookup and match Verilog nets with mySpice nodes.
// NumNodes: total number of entries in NodeAray
// time: the amout of time that has passed in the transient simulation.
// Vsrc: an array of voltage sources so drivers can update them if needed.
// numVsrc: number of entries in the Vsrc array.
//********************************************************************
void verilog_update_time(Verilog, Sol, NodeArray, NumNodes, time, Vsrc, numVsrc)
verilog *Verilog;
double *Sol;
char **NodeArray;
int NumNodes;
double time;
vsource *Vsrc[];
int numVsrc;
{
	int i, j;
	char result[128];
	vsource *inst;
	//handle inputs
	for(i = 0; i<(Verilog->total_inputs); i++) { //n^2 routine to match verilog nets to spice nodes
		for(j = 1; j<=NumNodes; j++) {
			if(strstr(NodeArray[j], Verilog->inputs[i]) != NULL) {
				if(Sol[j] >= (Verilog->logic_level)) //logic high when solution is above threshold
				{
					verilog_set_var(Verilog, Verilog->inputs[i], "1");
				}
				else //logic low othrewise
				{
					verilog_set_var(Verilog, Verilog->inputs[i], "0");
				}
			}
		}
	}
	//handle outputs
	if(time > 0.0) //print results of outputs only...drivers are printed in the Sol vector
	{
		for(i = 0; i<(Verilog->total_outputs); i++) {
			verilog_get_var(Verilog, Verilog->outputs[i], result);
			printf("%s\t",result);
		}
	}
	else //print table before first iteration
	{
		for(i = 0; i<(Verilog->total_outputs); i++) {
			printf("%s\t",Verilog->outputs[i]);
		}
	}
	//handle drivers
	if(time > 0.0) //once simulation has started...
	{
		for(i = 0; i<(Verilog->total_drivers); i++) { //n^2 routine to match verilog driver nets with spice nodes
			for(j = 1; j <= numVsrc; j++) {
				inst = Vsrc[j];
				if(strstr(inst->name, Verilog->drivers[i]) != NULL) //if it doesn't exist, don't continue
					break;
			}
			verilog_get_var(Verilog, Verilog->drivers[i], result); //otherwise get its value from modelSim
			if(strstr(result, "1") != NULL) //if the result is "1"
			{
				if(inst->voltage < Verilog->high_level) //ramp the voltage up until the logic_high point
					inst->voltage = inst->voltage + 0.1*Verilog->high_level; 
				else //otherwise it is already logic_high
					inst->voltage = Verilog->high_level;
			}
			else // if the result is "0"
			{
				if(inst->voltage > Verilog->low_level) //ramp the soltage source down until logic_low point
					inst->voltage = inst->voltage - 0.1*Verilog->high_level;
				else //otherwise it is already logic_low
					inst->voltage = Verilog->low_level;
			}
		}
	}


	verilog_step_time(Verilog, 100.0, 1.0); //step the time...fixed values are used because there is no timing constraints needed.

}
