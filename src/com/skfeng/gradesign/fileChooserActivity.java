package com.skfeng.gradesign;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

public class fileChooserActivity extends Activity {
	private Button mBackView;
	private GridView mGridView;
	private Button select_all_button;
	private Button select_inverse_button;
	private Button ok_return_button;
	private Button cancel_return_button;
	private TextView mTvPath;

	private String mSdcardRootPath; // sdcard ��·��
	private String mLastFilePath; // ��ǰ��ʾ��·��

	private ArrayList<FileInfo> mFileLists;// ��ǰ�ļ�·���µ���Ϣ
	private fileChooserAdapter mAdapter;
    
	
	private static final String TAG="fileChosserActivity";
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.filechooser_show);

		mSdcardRootPath = Environment.getExternalStorageDirectory()
				.getAbsolutePath();

		mBackView = (Button) findViewById(R.id.imgBackFolder);
		mGridView = (GridView) findViewById(R.id.gvFileChooser);
		select_all_button = (Button) findViewById(R.id.select_all);
		select_inverse_button = (Button) findViewById(R.id.invert_select);
		ok_return_button = (Button) findViewById(R.id.ok_button);
		cancel_return_button = (Button) findViewById(R.id.btExit);
		mTvPath = (TextView) findViewById(R.id.tvPath);

		mBackView.setOnClickListener(mClickListener);
		select_all_button.setOnClickListener(mClickListener);
		select_inverse_button.setOnClickListener(mClickListener);
		ok_return_button.setOnClickListener(mClickListener);
		cancel_return_button.setOnClickListener(mClickListener);

		mGridView.setOnItemClickListener(mItemClickListener);
		setGridViewAdapter(mSdcardRootPath);
	}

	// ����������
	private void setGridViewAdapter(String filePath) {
		updateFileItems(filePath);
		mAdapter = new fileChooserAdapter(this, mFileLists);
		mGridView.setAdapter(mAdapter);
	}

	// ����·���������ݣ�����֪ͨAdatper���ݸı�
	private void updateFileItems(String filePath) {
		mLastFilePath = filePath;
		mTvPath.setText(mLastFilePath);

		if (mFileLists == null)
			mFileLists = new ArrayList<FileInfo>();
		if (!mFileLists.isEmpty())
			mFileLists.clear();

		File[] files = folderScan(filePath);
		if (files == null)
			return;

		for (int i = 0; i < files.length; i++) {
			if (files[i].isHidden()) // ����ʾ�����ļ�
				continue;

			String fileAbsolutePath = files[i].getAbsolutePath();
			String fileName = files[i].getName();
			boolean isDirectory = false;
			if (files[i].isDirectory()) {
				isDirectory = true;
			}
			FileInfo fileInfo = new FileInfo(fileAbsolutePath, fileName,
					isDirectory);
			mFileLists.add(fileInfo);
		}
		// When first enter , the object of mAdatper don't initialized
		if (mAdapter != null)
			mAdapter.notifyDataSetChanged(); // ����ˢ��
	}

	// ��õ�ǰ·���������ļ�
	private File[] folderScan(String path) {
		File file = new File(path);
		File[] files = file.listFiles();
		return files;
	}

	// ������һ��Ŀ¼�Ĳ���
	public void backProcess() {
		// �жϵ�ǰ·���ǲ���sdcard·�� �� ������ǣ��򷵻ص���һ�㡣
		if (!mLastFilePath.equals(mSdcardRootPath)) {
			File thisFile = new File(mLastFilePath);
			String parentFilePath = thisFile.getParent();
			updateFileItems(parentFilePath);
		} else { // ��sdcard·�� ��ֱ�ӽ���
			setResult(RESULT_CANCELED);
			finish();
		}
	}

	public void select_all_process() {
		File[] current_files = folderScan(mLastFilePath);
		for (int i = 0; i < current_files.length; i++) {
			if (current_files[i].isHidden()) // ����ʾ�����ļ�
				continue;

			String fileAbsolutePath = current_files[i].getAbsolutePath();
			String fileName = current_files[i].getName();
			boolean isDirectory = false;
			if (current_files[i].isDirectory()) {
				isDirectory = true;
			}
			FileInfo fileInfo = new FileInfo(fileAbsolutePath, fileName,
					isDirectory);
			if (fileInfo.isImgineFile()) {
				Log.v(fileChooserActivity.TAG,"�����һ��fileInfo");
				Select_file_set.selected_files.add(fileInfo);
			}
		}
		// When first enter , the object of mAdatper don't initialized
		if (mAdapter != null)
			mAdapter.notifyDataSetChanged(); // ����ˢ�½���
	}

	public void inverse_select_process() {
		File[] current_files = folderScan(mLastFilePath);
		for (int i = 0; i < current_files.length; i++) {
			if (current_files[i].isHidden()) // ����ʾ�����ļ�
				continue;

			String fileAbsolutePath = current_files[i].getAbsolutePath();
			String fileName = current_files[i].getName();
			boolean isDirectory = false;
			if (current_files[i].isDirectory()) {
				isDirectory = true;
			}
			FileInfo fileInfo = new FileInfo(fileAbsolutePath, fileName,
					isDirectory);
			if (fileInfo.isImgineFile()) {
				if (!Select_file_set.selected_files.contains(fileInfo))
					Select_file_set.selected_files.add(fileInfo);
				else {
					Select_file_set.selected_files.remove(fileInfo);
				}
			}
		}
		if (mAdapter != null)
			mAdapter.notifyDataSetChanged(); // ����ˢ�½���
	}

	private void toast(CharSequence hint) {
		Toast.makeText(this, hint, Toast.LENGTH_SHORT).show();
	}

	private View.OnClickListener mClickListener = new OnClickListener() {
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.imgBackFolder:
				backProcess();
				break;
			case R.id.select_all:
				select_all_process();
				break;
			case R.id.invert_select:
				inverse_select_process();
				break;
			case R.id.ok_button:
				//finish_test();// �������ѡ��Ľ��,�Ѿ����Դ˷����ĵ��÷ŵ���MainActivity��
				setResult(RESULT_OK);
				finish();// ����Ѿ���������Select_file_set��
				break;
			case R.id.btExit:
				Select_file_set.selected_files.clear();// ��ս��
				setResult(RESULT_CANCELED);
				finish();
				break;
			default:
				break;
			}
		}
	};
	private AdapterView.OnItemClickListener mItemClickListener = new OnItemClickListener() {
		public void onItemClick(AdapterView<?> adapterView, View view,
				int position, long id) {
			Log.v(fileChooserActivity.TAG,"��ѽ���ң�mItemClickListener��������һ��");
			FileInfo fileInfo = (FileInfo) (((fileChooserAdapter) adapterView
					.getAdapter()).getItem(position));
			if (fileInfo.isDirectory()) // �����Ϊ�ļ���, ��ʾ���ļ����������ļ�
				updateFileItems(fileInfo.getFilePath());
			else if (fileInfo.isImgineFile()) { // ��ͼ���ļ� �� �򽫸�·��֪ͨ��������
				ViewHolder vh = (ViewHolder) view.getTag();
				if (vh.is_selected.isChecked() == false) {// ����֮ǰ����δѡ��״̬
					Select_file_set.selected_files.add(fileInfo);

				} else {
					Select_file_set.selected_files.remove(fileInfo);
				}
				vh.is_selected.toggle();// �ı�״̬
			} else { // �����ļ�.....
				toast(getText(R.string.open_file_error_format));
			}
		}
	};

/*	private void finish_test() {
		LayoutInflater lay_out=(LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		View view=lay_out.inflate(R.layout.activity_main, null);
		TextView show_choose_res = (TextView) view.findViewById(R.id.Test_chooses_files);
		for (FileInfo f_temp : Select_file_set.selected_files) {
			Log.v(fileChooserActivity.TAG,f_temp.getFileName());
			show_choose_res.append(f_temp.getFileName() + "\n");
		}
		
	}*/
}
