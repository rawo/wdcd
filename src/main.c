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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "wdcd-api.h"
#include "profile.h"
#include "cmd.h"
#include "log.h"
 
/**
 * WDCD - Wacom Device Configuration Daemon
 *
 * Tool that automatically applies parameters to Wacom device 
 * according to profile found in conf file.
 *
 * Parameters are applied for each newly found device
 * that match profile from config.
 *
 * For instance:
 * Having plugged in USB Wacom Intuos 3 will result
 * in appling configuration parameters found in corresponding profile.
 * 
 */
#define PROFILE_LOAD_INTERVAL 1 //seconds

#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"wdcd.lock"

void *__gxx_personality_v0 =0;

void wdcd_signal_callback_handler(int signum);
void daemonize();

XDeviceInfo* devices_top_ptr;
void* profiles = 0;

extern char* options_path_to_cfg;
extern int   options_run_daemon;

int main (int argc,char**argv)
{
	if (wdcd_verifySystemRequirements() == 0) 
	{
		printf("System requirements have not been meet.\n");
		printf("See log file for details.\n"); 
		exit(-1);
	}


	cmd_loadOptions(argc,argv);
	if (options_path_to_cfg == 0)
	{
		cmd_usage();
		exit(-1);
	}

	signal(SIGINT,wdcd_signal_callback_handler); /* ctrl + c*/

	if (options_run_daemon)
	{
		daemonize();
	}

	Display *display;
	int ndevices;
	int nprofiles;
	char message[50];

	wdcd_readProfilesOrExit(options_path_to_cfg,&profiles,&nprofiles);
	display = XOpenDisplay(NULL);

	sprintf(message,"found %i profile(s)",nprofiles);
	log_message(message);

	XDeviceInfo* devices;
	while(1)
	{
		devices = XListInputDevices(display,&ndevices);
		devices_top_ptr = devices;

		for (int profileIndex = 0; profileIndex < nprofiles; profileIndex++)
		{
			char *name = profile_getName(profiles,profileIndex);

			if (wdcd_profileExistsForDevice(name,devices,&ndevices))
			{
				wdcd_loadProfile(profileIndex,profiles);
			} else {
				wdcd_unloadProfile(profileIndex);
			}
		}

		sleep(PROFILE_LOAD_INTERVAL);
		XFreeDeviceList(devices_top_ptr);
	}
 
	wdcd_freeProfiles(profiles);
	return 0;
}

void wdcd_signal_callback_handler(int sig)
{
	switch(sig) {
	case SIGHUP:
		log_message("hangup signal catched");
		XFreeDeviceList(devices_top_ptr);
		wdcd_freeProfiles(profiles);
		exit(sig);
		break;
	case SIGTERM:
		log_message("terminate signal catched");
		XFreeDeviceList(devices_top_ptr);
		wdcd_freeProfiles(profiles);
		exit(sig);
		break;
	case SIGINT:
		log_message("interrupt signal catched");
		XFreeDeviceList(devices_top_ptr);
		wdcd_freeProfiles(profiles);
		exit(sig);
		break;
	}
}

void daemonize()
{
	int i,lfp;
	char str[10];
	if(getppid()==1) return; /* already a daemon */
	i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	//for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	chdir(RUNNING_DIR); /* change running directory */
	lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); /* can not open */
	//if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTERM,wdcd_signal_callback_handler); /* catch kill signal */
	signal(SIGHUP,wdcd_signal_callback_handler); /* catch hangup signal */
}
