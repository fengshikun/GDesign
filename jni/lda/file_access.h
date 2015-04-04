#ifndef FILE_H
#define FILE_H
#include "document.h"
corpus* read_corpus(char* corpus_name,int doc_num);
void save_model(struct corpus* cps, struct est_param* p, string model_name,double alpha, double beta, int topic_num, int sample_num);
#endif
