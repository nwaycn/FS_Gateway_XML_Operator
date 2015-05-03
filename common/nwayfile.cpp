#include "nwayfile.h"
#ifdef WIN32
#include <io.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#endif
#include <stdlib.h>
#include <string.h>
int get_file_list( const char* fs_conf_path,nway_filename* files,int startpos, int number_per_page )
{
	int res_status = 0;
	char szPath[255]={0};
	int cur_pos=0;
	nway_filename* curr_file=files;
	bool bFirst = true;
#ifdef WIN32
	_finddata_t FileInfo;
	sprintf(szPath,"%s\\*",fs_conf_path);
	long Handle = _findfirst(szPath, &FileInfo);

	if (Handle == -1L)
	{
		printf("WARNING: the path match failed\n");
		res_status = -1;
	}
	else
	{
		do{
			//判断是否有子目录
			if (FileInfo.attrib & _A_SUBDIR)    
			{
				
				if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))   
				{
					 
					//char szNewpath[255]={0};
					//sprintf(szNewpath, "%s\\%s",fs_conf_path , FileInfo.name);
					//get_file_list(szNewpath);
					//这里不用递归
				}
			}
			else  
			{
				if (cur_pos >= startpos )
				{
					 
					nway_filename* myfile =(nway_filename*) malloc(sizeof(nway_filename));
					sprintf(myfile->szFullPath,"%s\\%s\0",fs_conf_path,FileInfo.name);
					sprintf(myfile->szFilename,"%s\0",FileInfo.name);
					myfile->next = NULL;
					curr_file->next = myfile;
					curr_file = myfile;
				}
				cur_pos ++;
				if (cur_pos >= startpos + number_per_page) break;
				
			}
		}while (_findnext(Handle, &FileInfo) == 0);
	}
	_findclose(Handle);
#else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir(fs_conf_path)) == NULL) {
		fprintf(stderr,"cannot open directory: %s\n", fs_conf_path);
		res_status = -1;
	}
	chdir(fs_conf_path);
	while((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) {


			if(strcmp(".",entry->d_name) == 0 ||
				strcmp("..",entry->d_name) == 0)
				continue;
			//char szNewpath[255]={0};
			//sprintf(szNewpath, "%s/%s",fs_conf_path , entry->d_name);
			//get_file_list(szNewpath);
			//这里不用递归 
			 
		} else {
			 
			if (cur_pos >= startpos )
			{

				nway_filename* myfile =(nway_filename*) malloc(sizeof(nway_filename));
				sprintf(myfile->szFullPath,"%s/%s\0",fs_conf_path,entry->d_name);
				sprintf(myfile->szFilename,"%s/0",entry->d_name);
				myfile->next = NULL;
				curr_file->next = myfile;
				curr_file = myfile;
			}
			cur_pos ++;
			if (cur_pos >= startpos + number_per_page) break;
		}
	}
	chdir("..");
	closedir(dp);
#endif
	return res_status;
}

int destory_file_list( nway_filename* files )
{
	nway_filename* p;
	while(files)
	{
		p = files->next;
		free(files);
		files = p;
	}
	return 0;
}

int get_file_count( const char* fs_conf_path )
{
	int res_status = 0;
	char szPath[255]={0};
 
#ifdef WIN32
	_finddata_t FileInfo;
	sprintf(szPath,"%s\\*",fs_conf_path);
	long Handle = _findfirst(szPath, &FileInfo);

	if (Handle == -1L)
	{
		printf("WARNING: the path match failed\n");
		res_status = -1;
	}
	else
	{
		do{
			//判断是否有子目录
			if (FileInfo.attrib & _A_SUBDIR)    
			{

				if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))   
				{

					//char szNewpath[255]={0};
					//sprintf(szNewpath, "%s\\%s",fs_conf_path , FileInfo.name);
					//get_file_list(szNewpath);
					//这里不用递归
				}
			}
			else  
			{
				res_status ++;
			}
		}while (_findnext(Handle, &FileInfo) == 0);
	}
	_findclose(Handle);
#else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir(fs_conf_path)) == NULL) {
		fprintf(stderr,"cannot open directory: %s\n", fs_conf_path);
		res_status = -1;
	}
	chdir(fs_conf_path);
	while((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)) {


			if(strcmp(".",entry->d_name) == 0 ||
				strcmp("..",entry->d_name) == 0)
				continue;
			//char szNewpath[255]={0};
			//sprintf(szNewpath, "%s/%s",fs_conf_path , entry->d_name);
			//get_file_list(szNewpath);
			//这里不用递归 

		} else {

			res_status ++;
		}
	}
	chdir("..");
	closedir(dp);
#endif
	return res_status;
}

int file_exists( const char *filename )
{
	return (access(filename, 0) == 0);
}

int nway_remove( const char* filename )
{
	return (remove(filename)== 0 );
}
