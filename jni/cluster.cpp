#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"//因为在属性中已经配置了opencv等目录，所以把其当成了本地目录一样
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include<opencv2/legacy/legacy.hpp>
#include <jni.h>
#include <android/log.h>
#include <fstream>
using namespace cv;
using namespace std;

void cluster_kmeanspp(JNIEnv *env, jobject pObj,jobjectArray files_paths,jint k,jint len)
{
	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);

	char Feas_absolute[200];
	sprintf(Feas_absolute,"%s",c_fea_dir);
    char *tail=Feas_absolute+strlen(Feas_absolute);
    sprintf(tail,"/Feas.xml");

    __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "Feas_absolute: %s ",Feas_absolute);
    __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "len: %d ",len);
    FileStorage fin(Feas_absolute,FileStorage::READ);
    Mat featuresUnclustered,temp;

    for(int i=0;i<len;i++)
    {
    	jstring string = (jstring)(env->GetObjectArrayElement(files_paths, i));
    	const char *rawString = env->GetStringUTFChars(string, 0);

    	char x='/';
        const char* ptr=strrchr(rawString,x);//ptr指向的内容变为/001.jpg
        ptr++;
        char *dot=strchr(ptr,'.');
        *dot='_';
        __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "ptr: %s ",ptr);
        fin[ptr]>>temp;
        __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "temp's rows: %d ",temp.rows);
        featuresUnclustered.push_back(temp);
        env->ReleaseStringUTFChars(string,rawString);
    }
    fin.release();

    TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
    //retries number
    int retries=1;
    //necessary flags
    int flags=KMEANS_PP_CENTERS;
    Mat clusters,centers;

    __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "the value k is: %d ",k);
    __android_log_print(ANDROID_LOG_INFO, "cluster.cpp", "the featuresUnclustered's rows: %d ",featuresUnclustered.rows);
    kmeans(featuresUnclustered,k,clusters,tc,retries,flags,centers);

    //下面开始保存聚类信息,暂时先不保存featuresUnclustered
    sprintf(tail,"/cluster_res.xml");
    FileStorage fout(Feas_absolute,FileStorage::WRITE);
    fout<<"clusters"<<clusters;
    fout<<"centers"<<centers;
    fout.release();
    env->ReleaseStringUTFChars(fea_dir,c_fea_dir);
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved)
{
    JNIEnv* env;
    if (pVm->GetEnv((void **)&env, JNI_VERSION_1_6)) {
		 return -1;
	}
    JNINativeMethod nm[1];
    nm[0].name = "cluster_kmeanspp";
    nm[0].signature = "([Ljava/lang/String;II)V";
    nm[0].fnPtr = (void*)cluster_kmeanspp;

    jclass cls = env->FindClass("com/skfeng/ndk/ExtractSiftFea");
    if(cls==NULL)
    	__android_log_print(ANDROID_LOG_INFO, "env->FindClass com/skfeng/ndk/ExtractSiftFea", "Failed!");
    // Register methods with env->RegisterNatives.
    if(env->RegisterNatives(cls, nm, 1)<0)
    	__android_log_print(ANDROID_LOG_INFO, "RegisterNatives", "RegisterNatives is failed!");
    return JNI_VERSION_1_6;
}
