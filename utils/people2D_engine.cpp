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


void people2D_engine::predict(std::vector < std::vector <Real> > descriptor, std::vector <Real> &label_out)
{
 	unsigned int numsamples = descriptor.size();
	 
	//~ input vectors
	for (unsigned int i = 0; i < numsamples; i++)
	{
		double sumstump = 0;
		for ( unsigned int j = 0; j < adaboost_stage.model.size(); j++)
		{
			Real class_label;			
			if( descriptor[ i ][ adaboost_stage.model[j].dimension ] > adaboost_stage.model[j].theta ) 
				class_label = 1.0;
			else
				class_label = -1.0;	

			//~ hyperplane direction
			class_label  *= adaboost_stage.model[j].mode;
			
			//~ weighted sum
			sumstump += adaboost_stage.alpha[j] * class_label;
		}	
		
		label_out[i] = sumstump/adaboost_stage.normalizer;
	}	
}
// ---------------------------------------------------------
int people2D_engine::load_adaboost_model(std::string modelfile)
{
	 
	std::ifstream ifs(modelfile.c_str());
	std::string line;
	 
	int count = 0;
	double anorm=0;
	while( getline( ifs, line ) )
	{  
		count++;
		
		std::vector<std::string> tokens;
		LSL_stringtoken(line, tokens, " ");

		if(tokens.size() != 13)
		{
			printf("[LBSTBOOST]] Parsing failure in loading the boost file\n");
			exit(1);
		}	
		
		adaboost_stage.feattype.push_back( atoi(tokens[6].c_str()) );
		adaboost_stage.w.push_back( atof(tokens[7].c_str()) );
		adaboost_stage.alpha.push_back( atof(tokens[8].c_str()) );
		anorm += atof(tokens[8].c_str());
					
		LSL_weak_model mdl;
		mdl.theta = atof(tokens[9].c_str());
		mdl.mode = atoi(tokens[10].c_str());
		mdl.feature_idx = atoi(tokens[11].c_str());
		mdl.dimension = atoi(tokens[12].c_str());
		
		adaboost_stage.model.push_back(mdl);
		
	}
	adaboost_stage.normalizer = anorm;
		
	if(params.verbosity >=1)
	{
		printf("Boost Stages: %lu\n", adaboost_stage.alpha.size()); 
		printf("Boost Model: %lu\n", adaboost_stage.model.size()); 
		printf("Boost Ftype: %lu\n", adaboost_stage.feattype.size()); 
	}
 	
 	return(count);
}


// ---------------------------------------------------------
void people2D_engine::set_featureset()
{
	std::vector <int> featnum;
	std::vector <int> featnum_comparative;
	//~ default mix
	if(params.featuremix == 0)
	{
		printf("Standard features mix\n");
		featnum.push_back(0);featnum.push_back(1);featnum.push_back(2);featnum.push_back(3);featnum.push_back(4);
		featnum.push_back(5);featnum.push_back(6);featnum.push_back(7);featnum.push_back(8);featnum.push_back(9);
		featnum.push_back(10);featnum.push_back(11);featnum.push_back(12);featnum.push_back(13);featnum.push_back(14);
		featnum.push_back(15);featnum_comparative.push_back(0);
	}
	
	//~ no distance
	if(params.featuremix == 1)
	{
		printf("Features mix 1\n");
		featnum.push_back(0);featnum.push_back(1);featnum.push_back(2);featnum.push_back(3);featnum.push_back(4);
		featnum.push_back(5);featnum.push_back(6);featnum.push_back(7);featnum.push_back(8);featnum.push_back(9);
		featnum.push_back(10);featnum.push_back(11);featnum.push_back(12);featnum.push_back(13);featnum.push_back(14);
		featnum_comparative.push_back(0);
	}

	//~ no distance no relational
	if(params.featuremix == 2)
	{
		printf("Features mix 2\n");
		featnum.push_back(0);featnum.push_back(1);featnum.push_back(2);featnum.push_back(3);featnum.push_back(4);
		featnum.push_back(5);featnum.push_back(6);featnum.push_back(7);featnum.push_back(8);featnum.push_back(9);
		featnum.push_back(10);featnum.push_back(11);featnum.push_back(12);featnum.push_back(13);featnum.push_back(14);
	}	
		
	lfeatures = new LSL_lfeatures_class (featnum, featnum_comparative);
	fsz =featnum.size()+featnum_comparative.size();

}
// ---------------------------------------------------------

int people2D_engine::sanity_check (std::vector < std::vector <Real> > &descriptor)
{
	int ret = 1;
	if(!descriptor.size())
	{
		printf("Feature set is 0 dimensional\n");
		return(0);
	}
		
	unsigned int f_num = descriptor[0].size();
	
	for(unsigned int i=0; i < descriptor.size(); i++)
	{
		if (f_num != descriptor[i].size())
		{
			printf("Feature size mismatch [%d]\n",i);
			ret = 0;
		}
	
		for(unsigned int j = 0; j < descriptor[i].size(); j++)
		{
			int typeval =  fpclassify(descriptor[i][j]);
			if(typeval == FP_NAN || typeval == FP_INFINITE)
			{
				printf("nan or inf found in the feature @ position [%d][%d] = %g\n",i,j, descriptor[i][j]);
				ret = 0;
			}	
		}
	}
			
	return ret;
}

// ---------------------------------------------------------

void people2D_engine::get_labelfrom_segments(std::vector<LSL_Point3D_container> &clusters,std::vector <int> &label)
{
	//~ at least 3pts need to be labeled in order to consider a segment part of a person
	
	for (uint i=0;i<clusters.size();i++)
	{
		int counter = 0;
		for (uint j=0;j<clusters[i].pts.size();j++)
		{
			if(clusters[i].pts[j].label== PEOPLE2D_ISPEOPLE)
				counter ++;
		}
		if(counter >= L_MINCLUSTERSZ)
			label[i] = PEOPLE2D_ISPEOPLE;
	}
}

// ---------------------------------------------------------

void people2D_engine::segment_and_save(std::string fname)
{
	FILE *f=fopen(fname.c_str(), "wt");

	for (uint i=0;i<laserscan.size();i++)
	{
		std::vector<LSL_Point3D_container> clusters;

		//~ compute segments
		segmentscanJDC(i, clusters);
		
		if( (i%100) == 0)
		{
			printf(".");
		}
		
		unsigned int count = 0;
		fprintf(f,"%d ",i);
		for (uint w=0;w<clusters.size();w++)
		{
			count += clusters[w].pts.size();	
			for (unsigned int j = 0; j < clusters[w].pts.size(); j++)
				fprintf(f,"%f %f %d ", clusters[w].pts[j].x,clusters[w].pts[j].y,  w);
			
		}

		//~ padding for easy visualization
		for (unsigned int j = 0; j < PEOPLE2D_PADDING-count; j++)	
			fprintf(f,"0.0 0.0 -1 ");
		fprintf(f,"\n");
	}	
	fclose(f);
	printf("\n");	
}

// ---------------------------------------------------------
void people2D_engine::save_precall(std::string fname)
{
	FILE *f=fopen(fname.c_str(), "wt");
	float athr = -0.5;
	for (unsigned int acnt = 0; acnt < stats_detector.size(); athr += 1.0/PEOPLE2D_PRDISCR, acnt++)
	{
		double precision = (double)stats_detector[acnt].tp / (double)((double)stats_detector[acnt].tp+(double)stats_detector[acnt].fp);
		double recall = (double)stats_detector[acnt].tp / (double)((double)stats_detector[acnt].tp+(double)stats_detector[acnt].fn);
		fprintf(f,"%g %g %g %d %d %d %d %d\n",recall, precision, athr, stats_detector[acnt].tp, stats_detector[acnt].fp, stats_detector[acnt].fn, stats_detector[acnt].poslabel, stats_detector[acnt].neglabel);
	}

	fclose(f);
}

// ---------------------------------------------------------
void people2D_engine::collect_statistics(std::vector<LSL_Point3D_container> &clusters, std::vector <Real> & label)
{
	
	//~ cluster is annotated by majority vote
	float athr = -0.5;
	for (unsigned int acnt = 0; acnt < stats_detector.size(); athr += 1.0/PEOPLE2D_PRDISCR, acnt++)
	{
		//~ cluster is annotated by majority vote
		for (unsigned int w = 0; w < clusters.size();w++)
		{
			for (unsigned int j = 0; j < clusters[w].pts.size(); j++)
			{
				if(clusters[w].pts[j].label == PEOPLE2D_ISPEOPLE)
				{		
					stats_detector[acnt].poslabel++;
							
					//~ detected as people
					if(label[w] > athr)
						stats_detector[acnt].tp++;
					else	
						stats_detector[acnt].fn++;
				}
				else
				{
					stats_detector[acnt].neglabel++;

					if(label[w] > athr)
						stats_detector[acnt].fp++;
				}
				
			}
		}
	}
}
/*
void people2D_engine::collect_statistics(std::vector<LSL_Point3D_container> &clusters, std::vector <Real> & label)
{
	
	//~ cluster is annotated by majority vote
	int acnt = 0;
	for (float athr = -1.0; w < 1.0; w += 1.0/PEOPLE2D_PRDISCR, acnt++)
	{
		//~ cluster is annotated by majority vote
		for (unsigned int w = 0; w < clusters.size();w++)
		{
			int count =0;
			for (unsigned int j = 0; j < clusters[w].pts.size(); j++)
			{
				if(clusters[w].pts.label == PEOPLE2D_ISPEOPLE)
					count++;
			}
			float cthr = (float)clusters[w].pts.size()/2.0;
			
			//~ segment is labeled as 'people'
			if( (float)count/(float)clusters[w].pts.size() > cthr)
			{
				//~ detected as people
				if(label[w] > athr)
				{
					stats_detector[acnt] = ;
				}
				
			}
		}
	}
}*/

// ---------------------------------------------------------

void people2D_engine::detect_save_all()
{
	FILE *f=fopen(params.outputfile.c_str(), "wt");
	//~ init in case of pr recall curve
    if(params.precall)
    {
		stats_detector = std::vector <stats_str> (PEOPLE2D_PRDISCR);
		for (uint i=0;i<stats_detector.size();i++)
		{
			stats_detector[i].tp = 0;
			stats_detector[i].fp = 0;
			stats_detector[i].fn = 0;
			stats_detector[i].poslabel = 0;
			stats_detector[i].neglabel = 0;
			
		}
	}
	//~ detection and save
	for (uint i=0;i<laserscan.size();i++)
	{
		std::vector < std::vector <Real> > descriptor;

		if(params.verbosity >= 1)
			printf("Segmenting scan and computing descriptor [%d/%lu]\n", i,laserscan.size());
		std::vector<LSL_Point3D_container> clusters;
		
		//~ compute segments
		segmentscanJDC(i, clusters);
		if(params.verbosity == 2)
			printf("Found %lu segments\n", clusters.size());		
		
		//~ compute descriptors
	    lfeatures->compute_descriptor(clusters, descriptor);
	    
	    //~ sanity check
	    if(params.sanity)
	    {
			int ret = sanity_check (descriptor);
			if(!ret)
			{
				printf("Sanity check failed @ %d scan \n",i);
				exit(1);
			}
			else
			{
				if(params.verbosity == 2)
					printf("Sanity check passed \n");
			}
		}
		
		//~ associate label to segment
		if(params.verbosity == 2)
			printf("Adaboost predict \n");
		std::vector <Real> label(clusters.size());
		predict(descriptor,label);

		//~ collects statistics
	    if(params.precall)
			collect_statistics(clusters, label);
		
		if(params.verbosity == 2)
			printf("Save on disk\n");		 
			
		if(!params.benchmark)	
		{
			fprintf(f,"%d ", i);
			uint count =0;
			for (unsigned int w = 0; w < clusters.size();w++)
			{
				count += clusters[w].pts.size();
				for (unsigned int j = 0; j < clusters[w].pts.size(); j++)
					fprintf(f,"%f %f %g ", clusters[w].pts[j].x,clusters[w].pts[j].y, label[w]);
			}	
			
			//~ padding for easy visualization
			for (unsigned int j = 0; j < PEOPLE2D_PADDING-count; j++)	
				fprintf(f,"0.0 0.0 -2 ");
			fprintf(f,"\n");			
		}

	}
	fclose(f);	
}

// ---------------------------------------------------------

void people2D_engine::segment_describe_save_all()
{
	FILE *f=fopen(params.outputfile.c_str(), "wt");
	fprintf(f,"MULTIDIM 0\n");
	fprintf(f,"NCLASSES 2\n");
	fprintf(f,"NDIM %d\n",fsz);

	for (uint i=0;i<laserscan.size();i++)
	{
		std::vector < std::vector <Real> > descriptor;
		
		if(params.verbosity >= 1)
			printf("Segmenting scan and computing descriptor [%d/%lu]\n", i,laserscan.size());
		std::vector<LSL_Point3D_container> clusters;
		
		
		//~ compute segments
		segmentscanJDC(i, clusters);
		if(params.verbosity == 2)
			printf("Found %lu segments\n", clusters.size());		
		
		//~ compute descriptors
	    lfeatures->compute_descriptor(clusters, descriptor);
	    
	    //~ sanity check
	    if(params.sanity)
	    {
			int ret = sanity_check (descriptor);
			if(!ret)
			{
				printf("Sanity check failed @ %d scan \n",i);
				exit(1);
			}
			else
			{
				if(params.verbosity == 2)
					printf("Sanity check passed \n");
			}
		}
		
		//~ associate label to segment
		if(params.verbosity == 2)
			printf("Associating labels to segments \n");
		std::vector <int> label(clusters.size(),PEOPLE2D_ISNOTPEOPLE);
		get_labelfrom_segments(clusters,label);
		 
		if(params.verbosity == 2)	    
			printf("Save on disk\n");		
		for (uint w=0;w<descriptor.size();w++)
		{
			for (uint m=0;m<descriptor[w].size();m++)
				fprintf(f,"%20.10f ", descriptor[w][m]);
			fprintf(f,"%d \n",label[w]);
		}

		
	}
	fclose(f);	
}

// ---------------------------------------------------------
int people2D_engine::segmentscanJDC(int lidx, std::vector<LSL_Point3D_container> &clusters)
{

	//~ bailing var
	char split_complete = 1;

	//~ numpts
	int ptsz = laserscan[lidx].data.pts.size();

	//~ avoid nulls
	if(ptsz == 0)
		return (0);
	
	//~ last break point index
	int last_breaking_idx = 0;
	
	while(split_complete)
	{
		//~ min pts number
		if(last_breaking_idx < ptsz - 1)
		{
			//~ max distance check
			int breaking_idx = get_breakpoint(laserscan[lidx].data.pts, last_breaking_idx, params.sqjumpdist);
			
			if(breaking_idx - last_breaking_idx >= L_MINCLUSTERSZ)
			{
				//~ a cluster
				LSL_Point3D_container single_cluster;
				
				//~ pump it into
				single_cluster.pts.insert(single_cluster.pts.begin(), laserscan[lidx].data.pts.begin() + last_breaking_idx, laserscan[lidx].data.pts.begin() + breaking_idx);
				clusters.push_back(single_cluster);
 
			}	
			
			//~ endpoint
			last_breaking_idx = breaking_idx;
		}
		else
		{
			//~ break cycle
			split_complete = 0;
		}
	}

	
	return(1);
}

// ---------------------------------------------------------

int people2D_engine::load_scandata(std::string fname)
{
	//~ FORMAT: TSTAMP X1 Y1 L1 X2 Y2 L2 .... XM YM LM
	std::ifstream ifs(fname.c_str());
	std::string line;
    
    int count=0;
	
	// index it
	while( std::getline( ifs, line ) )
	{
		count++;
		//~ avoid comment
		std::vector <std::string> tokens;
		LSL_stringtoken(line, tokens, " ");  
		if(tokens.size())
		{
			//~ not a comment
			if(tokens[0].c_str()[0] != '#')
			{
				laserscan_data onescan( (tokens.size()-1)/3);
				onescan.timestamp = atof(tokens[0].c_str());
				
				//~ check divider
				int szdiv = (tokens.size()-1) % 3;
				if(szdiv != 0)
				{
					printf("Error in input file format\n");
					exit(1);
				}
				for (unsigned int i = 1, a = 0; i < tokens.size();i+=3,a++)
				{
					onescan.data.pts[a].x = atof(tokens[i].c_str());
					onescan.data.pts[a].y = atof(tokens[i+1].c_str());
					onescan.data.pts[a].label = atof(tokens[i+2].c_str());
				}
				//~ order by theta
				order_bytheta_incart(onescan.data.pts);

				//~ add it
				laserscan.push_back(onescan);
			}

		}
	}
	
	return(count);	
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


int  people2D_engine :: get_breakpoint(std::vector <LSL_Point3D_str> &pts, int last_breaking_idx, double sqjumpdist)
{
	int ptsz = pts.size() - 1;
	
	//~ failsafe
	int jmp_idx = pts.size(); 
	
	for(int i = last_breaking_idx; i < ptsz; i++)
	{
		double dist;
		dist = distance_L2_XY_sqr(&pts[i], &pts[i + 1]);

		if(dist > sqjumpdist)
		{ 
			//~ printf("dist: %g  (%g): [%g %g]v[%g %g]\n",dist, sqjumpdist, pts[i].x,pts[i].y, pts[i + 1].x, pts[i + 1].y);			
			
			//~ mark index
			jmp_idx = i + 1; 
			
			//~ bail out
			i = ptsz;
		}	
	}
	
	return(jmp_idx);
}

// ---------------------------------------------------------

void LSL_stringtoken(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
  
 
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}



