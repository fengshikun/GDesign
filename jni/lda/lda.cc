#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "lda.h"
#include "file_access.h"
//test code
#include <fstream>
#include <iostream>
#include "document.h"
#include <time.h>
using namespace std;
clock_t start, finish;
//test code

#include "file_access.h"

void init_param(struct corpus *cps,struct est_param* param,int topic_num);

// ./lda topic_num sample_num data model_name


// param int m: mth document
// param int n: nth word in the document
// param double *p : used as a temporay space to store multinomial sampling, I do not choose to malloc it in this func because this func is in a loop and will lead to too many malloc operation and a waste of time
// return a topic sample  
int sampling(int m, int n, int word_id, int topic_num, struct corpus* cps, struct est_param *param,double alpha, double beta,double* p,double s_talpha,double vbeta)
{
	// the following codes are similar to GibbsLDA++, but has a few differences. If you want to learn more about how to sampling, please reference to "Gibbs sampling in the generative model of latent dirichlet allocation"
	// first remove currrent z[m][n] from the count variables
	int topic_id = param->z[m][n];
	param->nw[topic_id][word_id]--;
	param->nd[m][topic_id]--;
	param->nw_sum[topic_id]--;

	// calculate multinomial sampling 
	// the following code is not very readable, because I comine two loop into one
	p[0] = (param->nw[0][word_id] + beta) / (param->nw_sum[0] + vbeta) * (param->nd[m][0] + alpha) / s_talpha;
	p[1] = p[0];
	for (int k = 1; k < topic_num-1; k++) 
	{
		p[k] += (param->nw[k][word_id] + beta) / (param->nw_sum[k] + vbeta) * (param->nd[m][k] + alpha) / s_talpha;
		p[k+1] = p[k];
	}
	p[topic_num-1] += (param->nw[topic_num-1][word_id] + beta) / (param->nw_sum[topic_num-1] + vbeta) * (param->nd[m][topic_num-1] + alpha) / s_talpha;
	// choose a sample from topic
	double temp = ((double)rand() / RAND_MAX) * p[topic_num - 1];
	double rand = temp;//貌似是变量名的问题，不这么转一下它不给过
	int sample_topic;
	for (sample_topic = 0; sample_topic < topic_num; sample_topic++)
	{
		if (p[sample_topic] > rand)
		{
			break;
		}
	}
	if(sample_topic == topic_num) sample_topic--; //如果相等就数组访问越界了
	// add new topic to statistics var
	param->nw[sample_topic][word_id]++;
	param->nd[m][sample_topic]++;
	param->nw_sum[sample_topic]++;
	return sample_topic;
}

void average_param(struct est_param *param, struct corpus* cps,int topic_num,double alpha,double beta, int sample_num)
{
	// theta
	for (int m=0; m<cps->num_docs; m++)
	{
		for (int k=0; k<topic_num; k++)
		{
			param->theta[m][k] /= sample_num;
		}
	}
	// phi
	for (int k=0; k<topic_num; k++)
	{
		for (int v=0; v<cps->num_terms; v++)
		{
			param->phi[k][v] /= sample_num;
		}
	}
}


// calculate theta and phi
void calcu_param(struct est_param *param, struct corpus* cps,int topic_num,double alpha,double beta)
{
	// theta
	for (int m=0; m<cps->num_docs; m++)
	{
		for (int k=0; k<topic_num; k++)
		{
			param->theta[m][k] += (param->nd[m][k] + alpha) / (param->nd_sum[m] + topic_num * alpha);
		}
	}
	// phi
	for (int k=0; k<topic_num; k++)
	{
		for (int v=0; v<cps->num_terms; v++)
		{
			param->phi[k][v] += (param->nw[k][v] + beta) / (param->nw_sum[k] + cps->num_terms * beta);
		}
	}
}

void set_zero(int **a,int rows,int columns)
{
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<columns; j++)
		{
			a[i][j] = 0;	
		}
	}
}

void set_zero_double(double **a,int rows,int columns)
{
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<columns; j++)
		{
			a[i][j] = 0;	
		}
	}
}


void set_int_array_zero(int *a,int len)
{
	for(int i=0;i<len;i++)
		a[i]=0;
}
void init_param(struct corpus *cps, struct est_param* param,int topic_num)
{
	//1. alloc z[m][n], z[m][n] stands for topic assigned to nth word in mth document
	//param->z = (int**)malloc (sizeof(int*) * cps->num_docs);
	param->z = new int*[cps->num_docs];
	for (int i=0; i<cps->num_docs; i++)
	{
		//param->z[i] = (int*)malloc (sizeof(int) * cps->docs[i].num_term);
		__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "i is %d, cps->docs[i].num_term is %d",i,cps->docs[i].num_term);
		param->z[i] = new int[cps->docs[i].num_term];
	}

	//2. alloc theta[m][k], theta[m][k] stands for the topic mixture proportion for document m
	//param->theta = (double**)malloc (sizeof(double*) * cps->num_docs);
	param->theta = new double*[cps->num_docs];
	for (int j=0; j<cps->num_docs; j++)
	{
		//param->theta[j] = (double*)malloc (sizeof(double) * topic_num);
		param->theta[j] = new double[topic_num];
	}
	set_zero_double(param->theta,cps->num_docs,topic_num);

	//3. alloc phi[k][v], phi[k][v] stands for the probability of vth word in vocabulary is assigned to topic k 
	//param->phi = (double**)malloc (sizeof(double*) * topic_num);
	param->phi = new double*[topic_num];
	for (int z=0; z<topic_num; z++)
	{
		//param->phi[z] = (double*)malloc (sizeof(double) * cps->num_terms);
		param->phi[z] = new double[cps->num_terms];
	}
	set_zero_double(param->phi,topic_num,cps->num_terms);

	//4. alloc nd[m][k], nd[m][k] stands for the number of kth topic assigned to mth document
	//param->nd = (int**)malloc (sizeof(int*) * cps->num_docs);
	param->nd = new int*[cps->num_docs];
	for (int m=0; m< cps->num_docs; m++)
	{
		//param->nd[m] = (int*)malloc(sizeof(int) * topic_num);
		param->nd[m] = new int[topic_num];
	}
	set_zero(param->nd, cps->num_docs, topic_num);

	//5. alloc nw[k][t], nw[k][t] stands for the number of kth topic assigned to tth term
	//param->nw = (int**)malloc (sizeof(int*) * topic_num);
	param->nw = new int*[topic_num];
	for (int n=0; n < topic_num; n++)
	{
		//param->nw[n] = (int*)malloc(sizeof(int) * cps->num_terms);
		param->nw[n] = new int[cps->num_terms];
	}
	set_zero(param->nw, topic_num, cps->num_terms);

	//6. alloc nd_sum[m], nd_sum[m] total number of word in mth document
	//param->nd_sum = (int*)malloc(sizeof(int)*cps->num_docs);
	param->nd_sum = new int[cps->num_docs];
	//memset(param->nd_sum,0,sizeof(int)*cps->num_docs);
	set_int_array_zero(param->nd_sum,cps->num_docs);
	//7. alloc nw_sum[k], nw_sum[k] total number of terms assigned to kth topic
	//param->nw_sum = (int*)malloc(sizeof(int)*topic_num);
	param->nw_sum = new int[topic_num];
	//memset(param->nw_sum,0,sizeof(int)*topic_num);
	set_int_array_zero(param->nw_sum,topic_num);
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "I am the marker 001!");
	//8. init nd[m][k],nw[k][t],nd_sum[m],nw_sum[k]
	srand(time(0)); // set seed for random function


	for (int d=0; d<cps->num_docs; d++)
	{
		int word_index = 0; // word_index indicates that a word is the (word_index)th word in the document.
		__android_log_print(ANDROID_LOG_INFO,"ldaApply.cpp","cps->docs[d].length: %d",cps->docs[d].length);
		for (int l=0; l<cps->docs[d].length; l++)
		{
			__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "cps->docs[d].words[l].count:%d",cps->docs[d].words[l].count);
			for (int t=0; t<cps->docs[d].words[l].count; t++)
			{

				param->z[d][word_index] = (int)(((double)rand() / RAND_MAX) * (topic_num - 0.00000001)); //确保上界不为topic_num，否则后面的数组调用会越界
				param->nw[param->z[d][word_index]][cps->docs[d].words[l].id] ++;
				param->nd[d][param->z[d][word_index]]++;
				param->nw_sum[param->z[d][word_index]]++;

				word_index++;
			}
		}
		param->nd_sum[d] += word_index;
	}
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "I am the marker 002!");
}
