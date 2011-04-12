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



#ifndef PEOPLE2D_NGN_H
#define PEOPLE2D_NGN_H

#include <stdio.h>	
#include <vector>	
#include <string>	
#include <fstream>
#include <string.h>
#include <sys/time.h>

#include "lfeatures.hpp"	
#define PEOPLE2D_ISPEOPLE 0
#define PEOPLE2D_ISNOTPEOPLE 1
#define PEOPLE2D_PADDING 450
#define PEOPLE2D_PRDISCR 50.0

typedef struct 
{
	double theta;
	int mode;
	int feature_idx;
	int dimension;
}LSL_weak_model;


typedef struct 
{
	//~ feature type
	std::vector <int> feattype;
	//~ sample weights
	std::vector <Real> w;
	//~ learned prior for each stump
	std::vector <Real> alpha;
	//~ params for weak classifiers
	std::vector <LSL_weak_model> model;
	//~ normalizer
	double normalizer;
	
}LSL_boost_stage_param;

typedef struct
{
	std::string inputfile, outputfile, modelfile,  modelfile_extra;
	char precall,benchmark;
	double dseg,sqjumpdist;
	int featuremix;
	char segonly,sanity,verbosity;
}sw_param_str;


typedef struct
{
	int tp;
	int fp;
	int fn;
	int poslabel;
	int neglabel;
}stats_str;

class laserscan_data
{
	public:
	
	LSL_Point3D_container data;
	double timestamp;
	
	laserscan_data();
	laserscan_data(int num)
	{
		data = LSL_Point3D_container (num);
	}
};
	
class people2D_engine
{
	private:
		sw_param_str params;
		std::vector <laserscan_data> laserscan;
		int  get_breakpoint(std::vector <LSL_Point3D_str> &pts, int last_breaking_idx, double sqjumpdist);
		int sanity_check (std::vector < std::vector <Real> > & descriptor);
		void get_labelfrom_segments(std::vector<LSL_Point3D_container> &clusters,std::vector <int> &label);
		void predict(std::vector < std::vector <Real> > descriptor, std::vector <Real> &label_out);
		void collect_statistics(std::vector<LSL_Point3D_container> &clusters, std::vector <Real> & label);

		
		LSL_lfeatures_class *lfeatures;
		unsigned int fsz;
		LSL_boost_stage_param adaboost_stage;
		
		std::vector <stats_str> stats_detector;
	public:
		int load_scandata(std::string fname );
		people2D_engine(sw_param_str param_in)
		{
			fsz=0;
			params = param_in;
			params.sqjumpdist = params.dseg * params.dseg ;
		}
		
		void segment_describe_save_all(void);
		void set_featureset();
		int segmentscanJDC(int idx, std::vector<LSL_Point3D_container> &clusters);
		void segment_and_save(std::string fname);
		int load_adaboost_model(std::string modelfile);
		void detect_save_all();
		void save_precall(std::string fname);


};

//~  string tokenizer
void LSL_stringtoken(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

#endif
