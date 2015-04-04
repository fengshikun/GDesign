#ifndef LDA_H
#define LDA_H

struct est_param
{
	int **z; //z[m][n] stands for topic assigned to nth word in mth document
	double **theta; // theta[m][k] stands for the topic mixture proportion for document m
	double **phi; // phi[k][v] stands for the probability of vth word in vocabulary is assigned to topic k 

	// count statistics
	int **nd; //nd[m][k] stands for the number of words assigned to kth topic in mth document
	int **nw; //nw[k][t] stands for the number of kth topic assigned to tth term
	int *nd_sum; //nd_sum[m] total number of word in mth document
	int *nw_sum; //nw_sum[k] total number of terms assigned to kth topic
};

void calcu_param(struct est_param *param, struct corpus* cps,int topic_num,double alpha,double beta);
int sampling(int m, int n, int word_id, int topic_num, struct corpus* cps, struct est_param *param,double alpha, double beta,double* p,double s_talpha,double vbeta);
void average_param(struct est_param *param, struct corpus* cps,int topic_num,double alpha,double beta, int sample_num);
void init_param(struct corpus *cps, struct est_param* param,int topic_num);
#endif
