/*****************************************************************
 *
 * This file is part of the People2D project
 *
 * People2D Copyright (c) 2011 Luciano Spinello
 *
 * This software is licensed under the "Creative Commons 
 * License (Attribution-NonCommercial-ShareAlike 3.0)" 
 * and is copyrighted by Luciano Spinello
 * 
 * Further information on this license can be found at:
 * http://creativecommons.org/licenses/by-nc-sa/3.0/
 * 
 * People2D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  
 *
 *****************************************************************/



#include "people2D_engine.hpp"



////~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 

void prg_info(void)
{
		printf(
	"Detects people in 2D range data\n"
	"options:\n"
	"-i input scan file\n"
	"-m model file\n"	
	"-d segmentation distance in m\n"
	"-pr consider the annotations in the file and compute pr curve\n"
	"-fx (valids: f0,f1,f2) feature set mix (default 0:all)\n"
	"-o output file\n"
	"-s ignores all params and writes segmentation only on disk\n"
	"-S do not run sanity checks on data (DISCOURAGED but faster)\n"
	"-B benchmark mode, no verbosity, no result save\n"
	"-v (1-3) verbosity (default = 0)\n"
	);
}

//~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 


int parse_command_line(int argc, char **argv, sw_param_str *sw_param)
{
	int i;
	
	sw_param -> sanity = 1;
	sw_param -> segonly = 0;
	sw_param -> featuremix = 0;
	sw_param -> verbosity = 0;
	sw_param -> precall = 0;
	sw_param -> benchmark = 0;
	
	if(argc < 5)
	{
		prg_info();
		return(0);
	}
		
	for(i=0; i<argc; i++)
	{
		
	 
		if(!strcmp(argv[i], "-i"))		
				sw_param -> inputfile = argv[i+1];
		
		if(!strcmp(argv[i], "-o"))		
				sw_param -> outputfile = argv[i+1];

		if(!strcmp(argv[i], "-m"))		
				sw_param -> modelfile = argv[i+1];
		
		if(!strcmp(argv[i], "-d"))		
				sw_param -> dseg = atof(argv[i+1]);

		if(!strcmp(argv[i], "-f1"))		
				sw_param -> featuremix = 1;

		if(!strcmp(argv[i], "-f2"))		
				sw_param -> featuremix = 2;

		if(!strcmp(argv[i], "-f3"))		
				sw_param -> featuremix = 3;
				
		if(!strcmp(argv[i], "-s"))		
				sw_param -> segonly = 1;

		if(!strcmp(argv[i], "-v1"))		
				sw_param -> verbosity = 1;

		if(!strcmp(argv[i], "-v2"))		
				sw_param -> verbosity = 2;

		if(!strcmp(argv[i], "-v3"))		
				sw_param -> verbosity = 3;

		if(!strcmp(argv[i], "-S"))		
				sw_param -> sanity = 0;

		if(!strcmp(argv[i], "-B"))		
				sw_param -> benchmark = 1;

		if(!strcmp(argv[i], "-pr"))		
				sw_param -> precall = 1;

	}
	
	if(!sw_param -> inputfile.size() || !sw_param -> outputfile.size() || !sw_param -> modelfile.size() || sw_param -> dseg == 0 ) 
	{
		prg_info();
		exit(0);
	}
			
	printf("[PAR] Input file: %s\n",sw_param -> inputfile.c_str());	
	printf("[PAR] Output file: %s\n",sw_param -> outputfile.c_str());	
	printf("[PAR] Model file: %s\n",sw_param -> modelfile.c_str());	
	if(sw_param -> benchmark)
	{
		printf("** Benchmark mode **\n");	
		sw_param -> verbosity = 0;
		sw_param -> sanity = 0;
		sw_param -> precall = 0;
	}

	return(1);
}

//~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 

int main (int argc, char **argv)
{
	sw_param_str sw_param;
	
	if(!parse_command_line(argc, argv, &sw_param))
		exit(0);
	printf("Attempting to load file: [%s]\n", sw_param.inputfile.c_str());
	people2D_engine ppl2D(sw_param);
	int ret = ppl2D.load_scandata(sw_param.inputfile);

	if(!ret)
	{
		printf("No data or error in parsing\n");
		exit(1);
	}
	printf("File contains %d laser scans\n", ret);
	
	if(sw_param.segonly)
	{
		printf("Segmentation only dumping in segments.seg\n");
		ppl2D.segment_and_save("segments.seg");	
	}
	else
	{
		printf("Setting up\n");
		ppl2D.set_featureset();
		printf("Load model\n");
		int reta = ppl2D.load_adaboost_model(sw_param.modelfile);
		if(!reta)
		{
			printf("Cannot open [%s]\n",sw_param.modelfile.c_str());
			exit(1);
		}
		struct timeval start_clk, stop_clk;
		gettimeofday(&start_clk, (struct timezone *)NULL);
	
		if(!sw_param.benchmark)
			printf("Detect and save..");
		else	
			printf("Detect only..");
		fflush(stdout);
		if(sw_param.precall)
			printf("and precision-recall computation..");fflush(stdout);
		ppl2D.detect_save_all();

		gettimeofday(&stop_clk, (struct timezone *)NULL);
		double t1 =  (double)start_clk.tv_sec + (double)start_clk.tv_usec/(1000000);
		double t2 =  (double)stop_clk.tv_sec + (double)stop_clk.tv_usec/(1000000);
		double elapsed = t2 - t1;		
		
		printf("completed in %g s [avg per scan %g Hz]\n",elapsed, (double)ret/elapsed);    
		
		if(sw_param.precall)
		{	
			char filen[800];
			sprintf(filen, "%s.prec",sw_param.outputfile.c_str());

			printf("Saving precision-recall file in '%s' -- use matlab script (or other) to visualize it\n",filen);
			ppl2D.save_precall(filen);
		}
	}
}
