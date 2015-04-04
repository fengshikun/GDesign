#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <jni.h>
#include <android/log.h>
#include <vector>
#include <fstream>
#include <string.h>
using namespace cv;
using namespace std;


void GeaFeaFile(JNIEnv *env, jobject pObj,jstring file_path)
{
	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);//c_fea_dir�д�����ɵ�fea�ļ����ŵ�Ŀ¼

	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "c_fea_dir is: %s!",c_fea_dir);
	const char *rawString=env->GetStringUTFChars(file_path, 0);//rawString���ͼƬ�ļ�����Ŀ¼
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "rawString is: %s!",rawString);

	//·����ʽ"./image/rs_query.png"��ע��б�ܷ���
	// Don't forget to call `ReleaseStringUTFChars` when you're done.
	Mat img_1=imread(rawString,CV_LOAD_IMAGE_GRAYSCALE);
	if(!img_1.data)//�������Ϊ��
	{
		__android_log_print(ANDROID_LOG_INFO, "extract_fea", "Mat img_1 is NULL!!!");
	}
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "OPEN RIGHT!!!");

	//��һ������SIFT���Ӽ��ؼ���
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "Before SIFT suanzi");
	SiftFeatureDetector detector;//���캯�������ڲ�Ĭ�ϵ�
	vector<KeyPoint> keypoints_1;//����1��ר���ɵ���ɵĵ����������洢������
    detector.detect(img_1,keypoints_1);//��img_1ͼ���м�⵽��������洢��������keypoints_1��
    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "After SIFT suanzi");
    SiftDescriptorExtractor extractor;//���������Ӷ���
    Mat descriptors_1;//������������ľ���
    extractor.compute(img_1,keypoints_1,descriptors_1);//������������
    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "After compute the fea vector!");
    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "descriptors_1's rows is: %d",descriptors_1.rows);
    char x='/';
    const char* ptr=strrchr(rawString,x);//ptrָ������ݱ�Ϊ/001.jpg
    char fea_absolute[200];//.fea�ļ��ľ���·���������ļ�����
    sprintf(fea_absolute,"%s",c_fea_dir);
    char *tail=fea_absolute+strlen(fea_absolute);
    sprintf(tail,"%s",ptr);
    tail=fea_absolute+strlen(fea_absolute);
    sprintf(tail,"%s",".fea.xml");//·��������ϣ���ʽΪ/storage/sdcard1/GraDesign/sift/e.jpg.fea.xml
    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "fea file dir is:%s",fea_absolute);

    ptr++;//��/001.jpg���001.jpg
    char *dot=strchr(ptr,'.');
    *dot='_';
    FileStorage fea_out;
    fea_out.open(fea_absolute,FileStorage::WRITE);
	fea_out<<ptr<<descriptors_1;//��������������ļ�

	fea_out.release();
	env->ReleaseStringUTFChars(fea_dir,c_fea_dir);
	env->ReleaseStringUTFChars(file_path,rawString);
}

void Gather_result(JNIEnv *env, jobject pObj,jobjectArray files_paths,jint len)
{
	__android_log_print(ANDROID_LOG_INFO, "Gather_result", "Gather_result is called!");
	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "c_fea_dir is: %s!",c_fea_dir);
	//const char *fea_absolute=new char[200];

	FileStorage fout;//�����Feas.xml
	Mat temp;
	for(int i=0;i<len;i++)
	{
		jstring string = (jstring)(env->GetObjectArrayElement(files_paths, i));
		const char *rawString = env->GetStringUTFChars(string, 0);
		char x='/';
	    const char* ptr=strrchr(rawString,x);//ptrָ������ݱ�Ϊ/001.jpg
	    char fea_absolute[200];
	    sprintf(fea_absolute,"%s",c_fea_dir);
	    char *tail=fea_absolute+strlen(fea_absolute);//Ŀ¼ֻ�� /storage/sdcard1/GraDesign/sift
	    char *tail_for_Feas=tail;//Ϊ����/storage/sdcard1/GraDesign/sift/Feas.xml����

	    sprintf(tail,"%s",ptr);//Ŀ¼��Ϊ /storage/sdcard1/GraDesign/sift/d.jpg
	    tail=fea_absolute+strlen(fea_absolute);
	    sprintf(tail,"%s",".fea.xml");//·���������,��Ϊ/storage/sdcard1/GraDesign/sift/d.jpg.fea.xml

	    ptr++;//��/001.jpg���001.jpg
	    char *dot=strchr(ptr,'.');
	    *dot='_';

	    FileStorage fea_in;
		fea_in.open(fea_absolute,FileStorage::READ);
		fea_in[ptr]>>temp;
		fea_in.release();

		sprintf(tail_for_Feas,"/Feas.xml");//����/storage/sdcard1/GraDesign/sift/Feas.xml
		__android_log_print(ANDROID_LOG_INFO, "Gather_result", "Feas.xml's path is: %s!",fea_absolute);
		if(0==i)
		   	fout.open(fea_absolute,FileStorage::WRITE);
		fout<<ptr<<temp;
	}
	fout.release();
}

void GeaFeaFiles(JNIEnv *env, jobject pObj,jobjectArray files_paths,jint len)
{
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "extract_fea is called!");
	jclass sift_class=env->FindClass("com/skfeng/ndk/ExtractSiftFea");
	jfieldID dirFID=env->GetStaticFieldID(sift_class,"SIFT_FEA_DIR_STRING","Ljava/lang/String;");
	jstring fea_dir=(jstring)env->GetStaticObjectField(sift_class,dirFID);
	const char *c_fea_dir=env->GetStringUTFChars(fea_dir,0);
	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "c_fea_dir is: %s!",c_fea_dir);
	//const char *fea_absolute=new char[200];

	FileStorage fout;//�����Feas.xml
	for (int i=0; i<len; i++) {
		jstring string = (jstring)(env->GetObjectArrayElement(files_paths, i));
		const char *rawString = env->GetStringUTFChars(string, 0);
		__android_log_print(ANDROID_LOG_INFO, "extract_fea", "rawString is: %s!",rawString);
		//·����ʽ"./image/rs_query.png"��ע��б�ܷ���
	    // Don't forget to call `ReleaseStringUTFChars` when you're done.
		Mat img_1=imread(rawString,CV_LOAD_IMAGE_GRAYSCALE);

		if(!img_1.data)//�������Ϊ��
		{
			__android_log_print(ANDROID_LOG_INFO, "extract_fea", "Mat img_1 is NULL!!!");
		}
		__android_log_print(ANDROID_LOG_INFO, "extract_fea", "OPEN RIGHT!!!");

		//��һ������SIFT���Ӽ��ؼ���
		__android_log_print(ANDROID_LOG_INFO, "extract_fea", "Before SIFT suanzi");
	    SiftFeatureDetector detector;//���캯�������ڲ�Ĭ�ϵ�
        vector<KeyPoint> keypoints_1;//����1��ר���ɵ���ɵĵ����������洢������

	    detector.detect(img_1,keypoints_1);//��img_1ͼ���м�⵽��������洢��������keypoints_1��

	    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "After SIFT suanzi");
	    SiftDescriptorExtractor extractor;//���������Ӷ���
	    Mat descriptors_1;//������������ľ���
	    extractor.compute(img_1,keypoints_1,descriptors_1);//������������


	    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "After compute the fea vector!");
	    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "descriptors_1's rows is: %d",descriptors_1.rows);
	    char x='/';
	    const char* ptr=strrchr(rawString,x);//ptrָ������ݱ�Ϊ/001.jpg

	    char fea_absolute[200];
	    sprintf(fea_absolute,"%s",c_fea_dir);
	    char *tail=fea_absolute+strlen(fea_absolute);
	    sprintf(tail,"%s",ptr);
	    tail=fea_absolute+strlen(fea_absolute);
	    sprintf(tail,"%s",".fea");//·���������

	    __android_log_print(ANDROID_LOG_INFO, "extract_fea", "fea file dir is:%s",fea_absolute);
	    ofstream fea_out;
	    fea_out.open(fea_absolute);
	    if(!fea_out.is_open())
	    {
	    	__android_log_print(ANDROID_LOG_INFO, "extract_fea", "Ofstream fails to open the file:%s",fea_absolute);
	    }
	    fea_out<<descriptors_1<<endl;//��������������ļ�
        fea_out.close();


	    sprintf(fea_absolute,"%s",c_fea_dir);
	    tail=fea_absolute+strlen(fea_absolute);
	    sprintf(tail,"/Feas.xml");
	    if(0==i)
	    	fout.open(fea_absolute,FileStorage::WRITE);
	    else
	    	fout.open(fea_absolute,FileStorage::APPEND);
	    ptr++;//��/001.jpg���001.jpg
	    char *dot=strchr(ptr,'.');
	    *dot='_';
	    fout<<ptr<<descriptors_1;
	    fout.release();

	    env->ReleaseStringUTFChars(string,rawString);
	}
	env->ReleaseStringUTFChars(fea_dir,c_fea_dir);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved)
{
    JNIEnv* env;
    if (pVm->GetEnv((void **)&env, JNI_VERSION_1_6)) {
		 return -1;
	}
    JNINativeMethod nm[3];
    nm[0].name = "GeaFeaFiles";
    nm[0].signature = "([Ljava/lang/String;I)V";
    nm[0].fnPtr = (void*)GeaFeaFiles;

    nm[1].name = "Gather_result";
    nm[1].signature = "([Ljava/lang/String;I)V";
    nm[1].fnPtr = (void*)Gather_result;

    nm[2].name = "GeaFeaFile";
    nm[2].signature = "(Ljava/lang/String;)V";
    nm[2].fnPtr = (void*)GeaFeaFile;

    jclass cls = env->FindClass("com/skfeng/ndk/ExtractSiftFea");
    if(cls==NULL)
    	__android_log_print(ANDROID_LOG_INFO, "env->FindClass com/skfeng/ndk/ExtractSiftFea", "Failed!");
    // Register methods with env->RegisterNatives.
    if(env->RegisterNatives(cls, nm, 3)<0)
    	__android_log_print(ANDROID_LOG_INFO, "RegisterNatives", "RegisterNatives is failed!");
    return JNI_VERSION_1_6;
}
