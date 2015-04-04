#include "./lda/lda.h"
#include "./lda/document.h"
#include "./lda/file_access.h"
#include <jni.h>
#include <android/log.h>
#include <fstream>
using namespace std;

void lda_main(JNIEnv *env, jobject pObj,jint lda_k,jint doc_num,jintArray category)
{
	int topic_num = lda_k;
	int sample_num = 50; // how many samples we want, we use these sample's to calculate phi and theta respectively and then use their averge to represent theta and phi

	int burn_in_num = 1000; //how many iterations are there until we are out of burn-in period
	// set beta = 0.1 and alpha = 50 / K
	double beta = 0.1;
	double alpha = (double)50 / (double)topic_num;
	int SAMPLE_LAG = 10;
	struct corpus *cps;
	struct est_param param;

	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);

	char Feas_absolute[200];
	sprintf(Feas_absolute,"%s",c_fea_dir);
	char *tail=Feas_absolute+strlen(Feas_absolute);
	sprintf(tail,"/frequencyCount");

	cps = read_corpus(Feas_absolute,doc_num); //读取训练集

	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "after read_corpus! documents number is %d",cps->num_docs);
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "after read_corpus! the random  is %d",cps->docs[5].words[35].count);
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "cps->num_terms:%d",cps->num_terms);
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "cps->docs[1].num_term is %d",cps->docs[1].num_term);
	init_param(cps,&param,topic_num); //初始化参数
	//do gibbs_num times sampling and compute averge theta,phi
	// burn in period
	//for (int iter_time=0; iter_time<burn_in_num + sample_num * SAMPLE_LAG; iter_time++)
	int iter_time = 0;
	int sample_time = 0;
	int p_size = topic_num<2?2:topic_num;
	//double *p = (double*)malloc(sizeof(double)*p_size); //multinomial sampling tempoary storage space;
	double *p = new double[p_size];
	double vbeta = cps->num_terms * beta; // v*beta is used in funciton sampling and this value never change so I put it here
	while (1)
	{
		//foreach documents, apply gibbs sampling
		for (int m=0; m<cps->num_docs; m++)
		{
			int word_index = 0; // word_index indicates that a word is the (word_index)th word in the document.
			double s_talpha = param.nd_sum[m] - 1 + topic_num * alpha; //nd_num[m]+topic_num * alpha will not change per document, so I put it here
			for (int l=0; l<cps->docs[m].length; l++)
			{
				for (int c=0; c<cps->docs[m].words[l].count; c++)
				{
					param.z[m][word_index] = sampling(m,word_index,cps->docs[m].words[l].id,topic_num,cps,&param,alpha,beta,p,s_talpha,vbeta); //sampling计算
					word_index++;
				}
			}
		}

		if ((iter_time >= burn_in_num) && (iter_time % SAMPLE_LAG == 0))
		{
			calcu_param(&param, cps,topic_num,alpha,beta); //计算参数
			sample_time++;
		}
		iter_time++;
		if (sample_time ==sample_num)
		{
			break;
		}
	}
	// calculate average of phi and theta
	average_param(&param, cps,topic_num,alpha,beta,sample_num); //计算出最后的参数结果

	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "after LDA SAMPLE");

	jint * c_array;
	c_array = env->GetIntArrayElements(category, NULL);




	sprintf(tail,"/lda.theta");
	ofstream theta_out(Feas_absolute);
	for (int m=0; m<cps->num_docs; m++)
	{
		int k;
		double max=0;
		int cate=-1;
		for (k=0; k<topic_num; k++)
		{
			if(param.theta[m][k]>max)
			{
				max=param.theta[m][k];
				cate=k;
			}
			if(k<topic_num-1)
				theta_out << param.theta[m][k] << " ";
			else
				theta_out << param.theta[m][k]<<endl;
		}
		c_array[m]=cate;
	}
	env->ReleaseIntArrayElements(category, c_array, 0);
	theta_out.close();
	//save_model(cps,&param,model_name,alpha,beta,topic_num,sample_num); //保存结果数据
}
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved)
{
    JNIEnv* env;
    if (pVm->GetEnv((void **)&env, JNI_VERSION_1_6)) {
		 return -1;
	}
    JNINativeMethod nm[1];
    nm[0].name = "lda_main";
    nm[0].signature = "(II[I)V";
    nm[0].fnPtr = (void*)lda_main;

    jclass cls = env->FindClass("com/skfeng/ndk/ExtractSiftFea");
    if(cls==NULL)
    	__android_log_print(ANDROID_LOG_INFO, "env->FindClass com/skfeng/ndk/ExtractSiftFea", "Failed!");
    // Register methods with env->RegisterNatives.
    if(env->RegisterNatives(cls, nm, 1)<0)
    	__android_log_print(ANDROID_LOG_INFO, "RegisterNatives", "RegisterNatives is failed!");
    return JNI_VERSION_1_6;
}
