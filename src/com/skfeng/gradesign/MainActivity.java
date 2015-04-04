package com.skfeng.gradesign;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import com.skfeng.ndk.*;

public class MainActivity extends Activity implements View.OnClickListener{
	
	
	private ArrayList<FileInfo> choosed_files;//ѡ����ļ�����Ϣ
	private Button choose_file_button;
	private Button generate_fea_button;
	private Button cluster_button;
	private Button freq_button;
	private Button lda_button;
	private Button copy_button;
	private Intent fileChooserIntent ;
	private static final int REQUEST_CODE = 1;   //�����뷵���ļ���Ϣ��������
	public static final String EXTRA_FILE_CHOOSER = "file_chooser";
	private static final String TAG="MainActivity";
	private TextView show_choose_file;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		ExtractSiftFea.test1();//�뿪ʼ�����ɣ�
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		choose_file_button=(Button)findViewById(R.id.select_file_button);
		show_choose_file=(TextView) findViewById(R.id.Test_chooses_files);
		generate_fea_button=(Button)findViewById(R.id.generate_sift_fea_button);
		generate_fea_button.setEnabled(false);//һ��ʼ������Ϊ������
		cluster_button=(Button)findViewById(R.id.cluster_button);
		cluster_button.setEnabled(false);
		freq_button=(Button)findViewById(R.id.freq_button);
		freq_button.setEnabled(false);
		lda_button=(Button)findViewById(R.id.lda_button);
		copy_button=(Button)findViewById(R.id.copy_button);
		lda_button.setEnabled(false);
		copy_button.setEnabled(false);
		//show_choose_file
		
		choose_file_button.setOnClickListener(this);
		generate_fea_button.setOnClickListener(this);//������һ������˴��鷳
		cluster_button.setOnClickListener(this);
		freq_button.setOnClickListener(this);
		lda_button.setOnClickListener(this);
		copy_button.setOnClickListener(this);
		fileChooserIntent=new Intent(this,fileChooserActivity.class);
		
	}



	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		System.out.println("����public void onClick(View v)");
		switch(v.getId()){
	    case R.id.select_file_button :
	    	if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
			    startActivityForResult(fileChooserIntent , REQUEST_CODE);
	    	else
	    		toast(getText(R.string.sdcard_unmonted_hint));
			break;
	    case R.id.generate_sift_fea_button:
	    	System.out.println("���濪ʼ����ExtractSiftFea.Gen_Sift_fea()");
	    	Log.v(TAG, "���濪ʼ����ExtractSiftFea.Gen_Sift_fea()");
	    	ExtractSiftFea.Gen_Sift_fea();
	    	System.out.println("����ExtractSiftFea.Gen_Sift_fea()���");
	    	cluster_button.setEnabled(true);//���Ծ�����
	    	break;
	    case R.id.cluster_button:
	    	System.out.println("���濪ʼ����ExtractSiftFea.cluster_kmeans()");
	    	Log.v(TAG, "���濪ʼ����ExtractSiftFea.cluster_kmeans()");
	    	ExtractSiftFea.cluster_kmeans();
	    	System.out.println("����ExtractSiftFea.cluster_kmeans()���");
	    	freq_button.setEnabled(true);
	    	break;
	    case R.id.freq_button:
	    	System.out.println("ExtractSiftFea.freq_count()");
	    	Log.v(TAG, "���濪ʼ����ExtractSiftFea.freq_count()");
	    	ExtractSiftFea.freq_count();
	    	System.out.println("����ExtractSiftFea.freq_count()���");
	    	lda_button.setEnabled(true);
	    	break;
	    case R.id.lda_button:
	    	System.out.println("ExtractSiftFea.final_lda()");
	    	Log.v(TAG, "���濪ʼ����ExtractSiftFea.final_lda()");
	    	ExtractSiftFea.final_lda();
	    	System.out.println("����ExtractSiftFea.final_lda()���");
	    	copy_button.setEnabled(true);
	    	break;
	    case R.id.copy_button:
	    	System.out.println("ExtractSiftFea.Cre_res()");
	    	Log.v(TAG, "���濪ʼ����ExtractSiftFea.Cre_res()");
	    	ExtractSiftFea.Cre_res();
	    	System.out.println("����ExtractSiftFea.Cre_res()���");
	    	break;
        default :
    	    break ;
    	    }
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (1==requestCode&&RESULT_OK==resultCode) {
			finish_test();
			generate_fea_button.setEnabled(true);//��������sift.fea�ļ���
		}
	}
	private void toast(CharSequence hint){
	    Toast.makeText(this, hint , Toast.LENGTH_SHORT).show();
	}
	
	private void finish_test() {
		for (FileInfo f_temp : Select_file_set.selected_files) {
			Log.v(MainActivity.TAG,f_temp.getFileName());
			show_choose_file.append(f_temp.getFileName() + "\n");
		}
		
	}
	
}
