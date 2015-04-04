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

void fre_count(JNIEnv *env, jobject pObj,jobjectArray files_paths,jint k,jint len)
{
	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);

	char Feas_absolute[200];
	sprintf(Feas_absolute,"%s",c_fea_dir);
	char *tail=Feas_absolute+strlen(Feas_absolute);
	sprintf(tail,"/Feas.xml");
	FileStorage Feas_in(Feas_absolute,FileStorage::READ);

	sprintf(tail,"/cluster_res.xml");
	FileStorage cluster_in(Feas_absolute,FileStorage::READ);

	Mat clusters;
	cluster_in["clusters"]>>clusters;
	cluster_in.release();

	MatIterator_ <int> it=clusters.begin<int>(),end=clusters.end<int>();
	int clusters_index=0;


	sprintf(tail,"/frequencyCount");
	ofstream fre_count_out(Feas_absolute);
	for(int path_index=0;path_index<len;path_index++)
	{
		jstring string = (jstring)(env->GetObjectArrayElement(files_paths, path_index));
	    const char *rawString = env->GetStringUTFChars(string, 0);

		char x='/';
		const char* ptr=strrchr(rawString,x);//ptr指向的内容变为/001.jpg
		ptr++;
		char *dot=strchr(ptr,'.');
		*dot='_';
		Mat temp;
		Feas_in[ptr]>>temp;
		int temp_rows=temp.rows;

		int* fre_count=new int[k];
		for(int x=0;x<k;x++)
			fre_count[x]=0;

		for(int i=0;i<temp_rows&&it!=end;i++,it++)
		{
			fre_count[*it]++;
		}
		int count=0;
		for(int y=0;y<k;y++)
		{
			if(0!=fre_count[y])
			{
				count++;
			}
		}

		fre_count_out<<count<<" ";
		for(int z=0;z<k;z++)
		{
			if(0!=fre_count[z])
			{
				fre_count_out<<z<<":"<<fre_count[z];
				if(z!=k-1)
					fre_count_out<<" ";
			}
		}
		fre_count_out<<endl;
		delete[] fre_count;
	}

	Feas_in.release();
	fre_count_out.close();
	env->ReleaseStringUTFChars(fea_dir,c_fea_dir);
}
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved)
{
    JNIEnv* env;
    if (pVm->GetEnv((void **)&env, JNI_VERSION_1_6)) {
		 return -1;
	}
    JNINativeMethod nm[1];
    nm[0].name = "fre_count";
    nm[0].signature = "([Ljava/lang/String;II)V";
    nm[0].fnPtr = (void*)fre_count;

    jclass cls = env->FindClass("com/skfeng/ndk/ExtractSiftFea");
    if(cls==NULL)
    	__android_log_print(ANDROID_LOG_INFO, "env->FindClass com/skfeng/ndk/ExtractSiftFea", "Failed!");
    // Register methods with env->RegisterNatives.
    if(env->RegisterNatives(cls, nm, 1)<0)
    	__android_log_print(ANDROID_LOG_INFO, "RegisterNatives", "RegisterNatives is failed!");
    return JNI_VERSION_1_6;
}
