#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include "document.h"
#include "lda.h"
#include <jni.h>
#include <android/log.h>

using namespace std;

struct corpus* read_corpus(char* corpus_name,int doc_num)
{
	ifstream infile(corpus_name);
	string line;

	//corpus* cps = (corpus*)malloc(sizeof(struct corpus));
	corpus* cps = new corpus;
	cps->docs = NULL; //不加这句后面的realloc会出问题
	int d; // var show how many documents(lines) we've read
	d = 0;
	if ( !infile )
	{
		cout << "unable to open input file" << endl;
		return 0;
	}
	cps->num_terms = 0;
	cps->docs = new document[doc_num];
	// read one line per time, one line stands for one document
	while (getline(infile,line))
	{
		//cps->docs = (struct document*)realloc(cps->docs,sizeof(struct document) * (d+1));
		if (cps->docs == NULL)
		{
			cout << "out of memory" << endl;
			exit(1);
		}
		// create a document model
		cps->docs[d].id = d;
		create_document(&cps->docs[d],&cps->num_terms,line);
		d++;
	}
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "cps->docs[1].num_term is %d",cps->docs[1].num_term);
	cps->num_docs = d;
	infile.close();
	return cps;
}

void save_model(struct corpus* cps, struct est_param* p, string model_name,double alpha, double beta, int topic_num, int sample_num)
{
	string file_name;
	// model_name.other contains: alpha,beta,topic_num,sample_num
	file_name = model_name+".other";
	ofstream other_out(file_name.c_str());
	other_out << "alpha: " << alpha << endl;
	other_out << "beta: " << beta << endl;
	other_out << "topic_num: " << topic_num << endl;
	other_out << "sample_num: " << sample_num << endl;
	other_out.close();

	// modeal_name.phi contains a matrix represents phi[k][v]
	file_name = model_name + ".phi";
	ofstream phi_out(file_name.c_str());
	for (int k=0; k<topic_num; k++)
	{
		int v;
		for (v=0; v<cps->num_terms-1; v++)
		{
			phi_out << p->phi[k][v] << "+";
		}
		phi_out << p->phi[k][v];
		phi_out << endl;
	}
	phi_out.close();

	//model_name.theta contains a matrix represents theta[m][k]
	file_name = model_name + ".theta";
	ofstream theta_out(file_name.c_str());
	for (int m=0; m<cps->num_docs; m++)
	{
		int k;
		for (k=0; k<topic_num-1; k++)
		{
			theta_out << p->theta[m][k] << " ";
		}
		theta_out << p->theta[m][k];
		theta_out << endl;
	}
	theta_out.close();

	// model_name.topic_assgin contains a matrix represents z[m][n]
	file_name = model_name + ".topic_assgin";
	ofstream tassgin_out(file_name.c_str());
	for (int m=0; m<cps->num_docs; m++)
	{
		int n;
		for (n=0; n<cps->docs[m].num_term-1; n++)
		{
			tassgin_out << p->z[m][n] << " ";
		}
		tassgin_out << p->z[m][n];
		tassgin_out << endl;
	}
	tassgin_out.close();
}
