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

#include "log.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char* LOG_FILE = "wdcd.log";

void log_message(char* message)
{
	FILE *logfile;
	logfile=fopen(LOG_FILE,"a");
	if(!logfile) 
	{
		printf("log file error\n");
		return;
	}
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	char timestamp[80];
	strftime (timestamp,80,"%H:%M:%S",timeinfo);

	fprintf(logfile,"[%s] %s\n",timestamp,message);
	fclose(logfile);
}
