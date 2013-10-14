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

#include "cmd.h"
#include "wdcd-api.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

extern char *optarg;
extern int optind, opterr, optopt;

char* options_path_to_cfg = 0;
int   options_run_daemon  = 0;

void cmd_usage()
{
	printf("Wacom Device Configuration Daemon, Version: %s\n",wdcd_version());
	printf(" usage: wdcd -c [full path to config file] [-d]\n");
    printf("  -c  start with specified config file with profiles. Mandatory.\n"); 
    printf("  -d  run as daemon\n"); 
}


void cmd_loadOptions(int argc,char**argv)
{
	int c;
	while ((c = getopt (argc, argv, "dc:")) != -1)
	{
	switch (c)
		 {
			 case 'c':
				 options_path_to_cfg = optarg;
				 break;
			 case 'd':
				 options_run_daemon=1;
				 break;
		 }
	}
}
