#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <jni.h>
#include <android/log.h>
#include "document.h"

int create_document(document* doc,int *max_id, string line)
{
	int wp; // word point
	string temp;
	istringstream istream(line);

	wp = 0;
	istream >> doc->length; //read out length
	doc->num_term = 0;
	//doc->words = (struct word*)malloc(sizeof(struct word)*doc->length);
	doc->words = new word[doc->length];
	while(istream >> temp)
	{
		int word_id;
		int count;
		sscanf(temp.c_str(),"%d:%d",&word_id,&count);
		doc->words[wp].id = word_id;
		doc->words[wp].count = count;
		doc->num_term += count;
		// find the biggest word id, this id indicates the total number of words in the vocabulary
		if (*max_id<=doc->words[wp].id)
		{
			*max_id = doc->words[wp].id + 1;
		}
		wp++;
	}
	__android_log_print(ANDROID_LOG_INFO, "ldaApply.cpp", "doc->num_term is %d",doc->num_term);
	return 1;
}
