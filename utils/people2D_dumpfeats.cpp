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
	"Dumps features and labels for learning people appeareance from 2D range data\n"
	"options:\n"
	"-i input scan file\n"
	"-o output model\n"
	"-d segmentation distance in m\n"
	"-fx (valids: f0,f1,f2,f3) feature set mix (default 0:all)\n"
	"-s ignores all params and writes segmentation only on disk\n"
	"-S do not run sanity checks on data (DISCOURAGED but faster)\n"
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

		if(!strcmp(argv[i], "-S"))		
				sw_param -> sanity = 0;

	}
	
	if(!sw_param -> inputfile.size() || !sw_param -> outputfile.size() || sw_param -> dseg == 0 ) 
	{
		prg_info();
		exit(0);
	}
			
	printf("[PAR] Input file: %s\n",sw_param -> inputfile.c_str());	
	printf("[PAR] Output file: %s\n",sw_param -> outputfile.c_str());	
	printf("[PAR] Segmentation distance: %g\n", sw_param -> dseg);		
	printf("[PAR] Feature preset: %d\n", sw_param -> featuremix);		

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
		ppl2D.segment_describe_save_all();
	}
}
