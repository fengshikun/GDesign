package com.skfeng.gradesign;

import android.view.View;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

public class ViewHolder {
	ImageView imgFileIcon;
	TextView tvFileName;
	CheckBox is_selected;

	public ViewHolder(View view) {
		imgFileIcon = (ImageView) view.findViewById(R.id.imgFileIcon);
		tvFileName = (TextView) view.findViewById(R.id.tvFileName);
		is_selected= (CheckBox) view.findViewById(R.id.is_selected);
	}
}
