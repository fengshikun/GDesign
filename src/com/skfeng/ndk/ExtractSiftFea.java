package com.skfeng.ndk;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import android.util.Log;

import com.skfeng.gradesign.FileInfo;
import com.skfeng.gradesign.Select_file_set;


public class ExtractSiftFea {
	
	public static  String SIFT_FEA_DIR_STRING="/storage/sdcard1/GraDesign/sift";
	public static int k=70;//the k of kmeans
	public static int lda_k=3;
	public static String[] pho_paths=
	{"/storage/sdcard1/DCIM/myphoto/e.jpg","/storage/sdcard1/DCIM/myphoto/d.jpg",
	"/storage/sdcard1/DCIM/myphoto/f.jpg","/storage/sdcard1/DCIM/myphoto/a.jpg",
	"/storage/sdcard1/DCIM/myphoto/b.jpg","/storage/sdcard1/DCIM/myphoto/c.jpg"};
	//���ÿ��ͼƬ���������ļ����ļ���	
	private static final String TAG="ExtractSiftFea";
	public static int[]category = new int[100];//Ҫ�����ͼƬ�ĸ������ᳬ��100�ġ�
	public static String res_dir="/storage/sdcard1/GraDesign/res";
	
	public static final void Gen_Sift_fea()
	{
		int len=Select_file_set.selected_files.size();
		FileInfo[] fileInfos=Select_file_set.selected_files.toArray(new FileInfo[len]);
		pho_paths=new String[len];
		for (int i = 0; i < pho_paths.length; i++) {
			pho_paths[i]=fileInfos[i].getFilePath();
		}
		ExecutorService fixedThreadPool = Executors.newFixedThreadPool(4);
		
		for (int i = 0; i < fileInfos.length; i++) {
			final String file_path=pho_paths[i];
			fixedThreadPool.execute(new Runnable(){
				@Override
				public void run() {
					System.out.println("Thread begin!");
					GeaFeaFile(file_path);
				}
			});
		}
		fixedThreadPool.shutdown();
		try  
        {  
            // awaitTermination����false����ʱ�����ѭ��������true���̳߳��е��߳�ִ��������߳�����ѭ������ִ�У�
			//ÿ��10��ѭ��һ��  
            while (!fixedThreadPool.awaitTermination(10, TimeUnit.SECONDS));  
        }  
        catch (InterruptedException e)  
        {  
            e.printStackTrace();  
        }    
		Log.v(TAG, "����Gather_resultǰ");
		Gather_result(pho_paths, len);
		Log.v(TAG, "����Gather_result��");
	}
	
	public static final void cluster_kmeans()
	{
		int len=pho_paths.length;
		
		Log.v(TAG, "����cluster_kmeansppǰ");
		cluster_kmeanspp(pho_paths,k,len);
		Log.v(TAG, "����cluster_kmeanspp��");
	}
	public static final void freq_count()
	{
		int len=pho_paths.length;
		Log.v(TAG, "����fre_countǰ");
		fre_count(pho_paths,k,len);
		Log.v(TAG, "����fre_count��");
	}
	public static final void final_lda()
	{
		int len=pho_paths.length;
		Log.v(TAG, "����lda_mainǰ");
		lda_main(lda_k,len,category);
		Log.v(TAG, "����lda_main��");
	}
	
	/**
	 * ɾ��һ��Ŀ¼�������Ƿǿ�Ŀ¼��
	 * 
	 * @param dir
	 */
	public  static boolean delDir(File dir) {
		if (dir == null || !dir.exists() || dir.isFile()) {
			return false;
		}
		for (File file : dir.listFiles()) {
			if (file.isFile()) {
				file.delete();
			} else if (file.isDirectory()) {
				delDir(file);// �ݹ�
			}
		}
		dir.delete();
		return true;
	}
	
	public static final void copy(File src, File dst) throws IOException {
	    InputStream in = new FileInputStream(src);
	    OutputStream out = new FileOutputStream(dst);

	    // Transfer bytes from in to out
	    byte[] buf = new byte[1024];
	    int len;
	    while ((len = in.read(buf)) > 0) {
	        out.write(buf, 0, len);
	    }
	    in.close();
	    out.close();
	}
	public static final void Cre_res()//���ݷ�����������Ӧ��ͼƬ����Ŀ¼
	{
		File res_file_dir=new File(res_dir);
		if(res_file_dir.exists())
			delDir(res_file_dir);
		res_file_dir.mkdir();
		
		String base_name="class";
		for (int i = 0; i < lda_k; i++) {
			String class_name=base_name+Integer.toString(i);
			File class_name_dir=new File(res_dir+"/"+class_name);
			class_name_dir.mkdir();
			
			for(int j=0;j<pho_paths.length;j++)
			{
				if(category[j]==i)
				{
					String dst_string=class_name_dir.getAbsolutePath()+
							pho_paths[j].substring(pho_paths[j].lastIndexOf("/"));
					File src = new File(pho_paths[j]),
						 dst = new File(dst_string);
					try {
						copy(src, dst);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}
	public static final native void GeaFeaFile(String file_path);
	public static final native void Gather_result(String[] files_paths,int len);
	public static final native void GeaFeaFiles(String[] files_paths,int len);
	public static final native void cluster_kmeanspp(String[] files_paths,int k,int len);
	public static final native void fre_count(String[] files_paths,int k,int len);
	public static final native void lda_main(int lda_k,int doc_num,int[] category);
	public  static void test1()
	{
		Log.v(TAG,"�뿪ʼ�����ɣ�");
	}
	//���濪ʼ���붯̬���ӿ�
	static{
		System.loadLibrary("opencv_java");
		System.loadLibrary("nonfree");
    	System.loadLibrary("Sift");
    	System.loadLibrary("cluster");
    	System.loadLibrary("freCount");
    	System.loadLibrary("ldaApply");
    	Log.v(TAG,"all library loaded successfully,no UnsatisfiedLinkError throwed!!!");
	}
	
}
