/*
* This file is part of wdcd 
* Copyright (C) 2013 Rafal Wokacz (rafal.wokacz@gmail.com)
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "fileparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static FILE *fp;
static char* fpath;
static char PROFILE_TOKEN[8] = "profile{";
fpos_t next_profile;
fpos_t next_profile_name_pos;

char EOL = '\n';
char TAB = '\t';
char L_BRACKET = '{';
char R_BRACKET = '}';

void gotonextline(FILE *fp) {
	char c;
	do {
		c = fgetc(fp);
	}
	while(c != '\n');
}

/* 
int main (int argc,char**argv)
{

	char* fpath = "/home/common/Projects/wdcd/cfg/wdcd.conf\0";
    int nprofiles = cfgfile_getnumberofprofiles(fpath);
	printf("found profiles: %d\n",nprofiles);
	for (int i = 0; i<nprofiles ; i++)
	{
		cfgfile_profile_readnext();
		char name[50];
		memset(&name[0],0,sizeof(name));
		profile_getname(name);
		printf("profile name: %s\n",name );
		char param[100];
		memset(&param[0],0,sizeof(param));
		int nparams = profile_getnumberofparams();
		printf("number of parameters: %d\n",nparams);
		while(profile_getnext_parameter(param)){
			printf("%s\n",param);
			memset(&param[0],0,sizeof(param));
		}
	}
}
 */

int cfgfile_getnumberofprofiles(char* path)
{
	fp = fopen(path,"r");	
	fpath = path;

	if (fp == NULL)
		return -1;

	char c;

	int nprofiles=0;	
	int profile_size = sizeof(PROFILE_TOKEN); 
	int c_count=0;

	do {
		c = fgetc(fp);
		if( c == '{') {
			c_count++; //adding to overal PROFILE_TOKEN size count
			continue;
		}
		if( c == ' ' || c == '\n' || c == TAB ) 
			continue;

		//all chars that compose PROFILE_TOKEN have been found
		if(c_count == profile_size){
			nprofiles++;
			c_count=0;
		}

		//match profile chars with char read from file
	    if(PROFILE_TOKEN[c_count] == c)
			c_count++;

	}
	while( c != EOF);
	
	fclose(fp);
	return nprofiles;
}

/*
void iterate(char* path)
{
	fp = fopen(path,"r");	
	if (fp == NULL) 
	{
		printf("Could not open file for read\n");
		exit(0);
	}
	char key[200];
	char val[200];

	int retval;
	do {
		char bracket[2];
		memset(&bracket[0],0,sizeof(bracket));
		retval = fscanf(fp," profile %[{] ",bracket);
		if(retval == -1) continue;
		printf("rv: %d, found profile: \n",retval);

		char* hasBracket;
		do {
			//read key
			memset(&key[0],0,sizeof(key));
			retval = fscanf(fp," %s = ",key);
			hasBracket = strrchr(key,'}');
			//read value
			if(hasBracket == NULL && retval == 1) {
				memset(&val[0],0,sizeof(val));
				char c;
				int i = 0;
				do {
					c = fgetc(fp);
					if(c != ';' && c != EOF) val[i++] = c;
				}
				while(c != ';' && c != EOF);
				val[i]='\0';
				printf("rv: %d, %s = %s\n",retval,key,val);
			}

		} while(hasBracket == NULL && retval == 1);
	}
	while( retval != EOF);
	//reach last closing bracket }
	
	retval = fscanf(fp," name = %s;",val);
	printf("rv: %d, name =  %s\n",retval,val);

	memset(&val[0],0,sizeof(val));
	//retval = fscanf(fp," %s = %s;",key, val);
	while(fscanf(fp," %s = %s; ",key, val)==2)
	{
		printf("rv: %d, %s = %s\n",retval,key,val);
		memset(&val[0],0,sizeof(val));
		memset(&key[0],0,sizeof(key));
	}

	memset(&bracket[0],0,sizeof(bracket));
	retval = fscanf(fp," %s ",bracket);
	printf("rv: %d, bracket: %s\n",retval, bracket);
	if (retval == EOF) 
		printf("EOF reached\n");
	fclose(fp);
}
*/
int cfgfile_profile_readnext()
{
	if( fp == NULL)
		return -1;
	
	fp = fopen(fpath,"r");
	fsetpos(fp,&next_profile);

	char c;

	int nprofiles=0;	
	int profile_size = sizeof(PROFILE_TOKEN); 
	int c_count=0;

	do {
		c = fgetc(fp);
		if( c == '{') {
			c_count++; //adding to overal PROFILE_TOKEN size count
			continue;
		}
		if( c == ' ' || c == EOL || c == TAB ) 
			continue;

		//mark position where profile name starts
		if(c == ':' )
		{
			fgetpos(fp,&next_profile_name_pos);
		}

		//all chars that compose PROFILE_TOKEN have been found
		if(c_count == profile_size){
			nprofiles++;
			//c=fgetc(fp);
		}

		//match profile chars with char read from file
	    if(PROFILE_TOKEN[c_count] == c)
		{
			c_count++;
		}

	}
	while (c != EOF && c_count != profile_size);
	if (c==EOF) {
		fclose(fp);
	} else {
		gotonextline(fp);
		fgetpos(fp,&next_profile);
	}

	if (c_count == profile_size)
		return 1;
	return -1;
	
}

int profile_getnext_parameter(char* param)
{
	
	if( fp == NULL)
		return -1;
	
	fp = fopen(fpath,"r");
	fsetpos(fp,&next_profile);

	//char val[200];

	//int retval;
	//char* hasBracket;
	//char line[128];
	char c;
	int c_counter=0;
	do {
	   c = fgetc(fp);
	   if( c == EOL || c == TAB || c=='}') // || c=='{')
	   {
		   continue;
		}
	   if ( c==EOF)
		   break;
	   *param = c;
	   param++;
	   c_counter++;
	} while ( c != EOL );
	fgetpos(fp,&next_profile);
	fclose(fp);
	return c_counter;
}

void profile_getname(char* name)
{
	if( fp == NULL)
		return ;
	
	fp = fopen(fpath,"r");
	fsetpos(fp,&next_profile_name_pos);

	char c;
	int count_chars = 0;
	do {
		c = fgetc(fp);
		if (c==EOL || c==TAB || c==L_BRACKET)
			continue;
		if (c==' ' && count_chars ==0)
			continue;

		count_chars++;
		*name = c;
		name++;
		
	}while(c != L_BRACKET);

	fclose(fp);
}

int profile_getnumberofparams()
{
	if( fp == NULL)
		return 0;
	
	fp = fopen(fpath,"r");
	fsetpos(fp,&next_profile);

	char c;
	int nparameters=0;
	do {
	   c = fgetc(fp);
	   if (c == EOL)
	   {
		   nparameters++;
	   }
	   if ( c==EOF)
		   break;
	} while ( c != R_BRACKET );
	fclose(fp);

	return nparameters;
}
