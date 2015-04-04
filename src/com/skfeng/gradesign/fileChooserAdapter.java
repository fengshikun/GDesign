package com.skfeng.gradesign;

import java.util.ArrayList;
import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

public class fileChooserAdapter extends BaseAdapter {

	private ArrayList<FileInfo> mFileLists;
	private LayoutInflater mLayoutInflater = null;
	
	public fileChooserAdapter(Context context, ArrayList<FileInfo> fileLists) {
		super();
		mFileLists = fileLists;
		mLayoutInflater = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mFileLists.size();
	}

	@Override
	public FileInfo getItem(int position) {
		// TODO Auto-generated method stub
		return mFileLists.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub
		View view = null;
		ViewHolder holder = null;
		if (convertView == null || convertView.getTag() == null) {
			view = mLayoutInflater.inflate(R.layout.file_chooser_item, null);
			holder = new ViewHolder(view);
			view.setTag(holder);
		} else {
			view = convertView;
			holder = (ViewHolder) convertView.getTag();
		}

		FileInfo fileInfo = getItem(position);
		// TODO

		holder.tvFileName.setText(fileInfo.getFileName());
		if (fileInfo.isDirectory()) { // �ļ���
			holder.imgFileIcon.setImageResource(R.drawable.folder_icon);// ��ɳ����
			holder.tvFileName.setTextColor(Color.GRAY);
			holder.is_selected.setEnabled(false);// ����Ϊ����ѡ
		} else if (fileInfo.isImgineFile()) { // ͼ���ļ�
			holder.imgFileIcon.setImageResource(R.drawable.pho_icon);// �պ�
			holder.tvFileName.setTextColor(Color.RED);
			if(Select_file_set.selected_files.contains(fileInfo))
				holder.is_selected.setChecked(true);
			else {
				holder.is_selected.setChecked(false);
			}
		} else { // δ֪�ļ�
			holder.imgFileIcon.setImageResource(R.drawable.unknown_icon);// Ұ��:�
			holder.tvFileName.setTextColor(Color.GRAY);
			holder.is_selected.setEnabled(false);// ����Ϊ����ѡ
		}
		return view;
	}

}
