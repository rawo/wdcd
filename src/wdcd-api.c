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

#include "profile.h"
#include "wdcd-api.h"
#include "log.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>


#define XSETWACOM_COMMAND "xsetwacom"

typedef struct _LoadedProfile
{

	int profileIndex;
	bool loaded; 

} LoadedProfile;

//keeps track of all profiles
//and additionally keep track of loaded/unloaded information
LoadedProfile* loadedConfigs;

//allways points to the first LoadedProfile struct
//in the allocated memory pointed by loadedConfigs
LoadedProfile* firstLoadedConfig;

//keeps number of loaded configs
//is equal to all available profiles
int nconfigs;
int internal_nprofiles;

/**
 * Execute xsetwacom for specified device name and parameter
 * Format for cmd is as follows: 
 * xwacom --set "[devicename]" "parameter name" "value"
 *
 * paramValue format "[devicename]" "parameter value" "value"
 * 
 * For instance "Wacom Intuos 3 pad" "Button 1" "key c"
 *
 * @param deviceName - pointer to device name
 * @param paramValue - device parameter
 */
void runXSetWacom(char* deviceName, char* paramValue);

void internal_setLoadStatusForProfile(int profileIndex,bool loaded)
{
	loadedConfigs = firstLoadedConfig;
	for(int i = 0; i<nconfigs; i++) 
	{
		if (loadedConfigs[i].profileIndex == profileIndex)
		{
			loadedConfigs[i].loaded=loaded;
		}
	}
}

void internal_setProfileLoadedForDevice(int profileIndex)
{
	internal_setLoadStatusForProfile(profileIndex,true);
}


void internal_setProfileUnloadedForDevice(int profileIndex)
{
	internal_setLoadStatusForProfile(profileIndex,false);
}

void internal_setupLoadedProfiles(void* profiles,int* nprofiles)
{
	loadedConfigs = (LoadedProfile*)malloc(*nprofiles*sizeof(LoadedProfile));

	firstLoadedConfig = loadedConfigs;

	nconfigs = *nprofiles;

	for (int i = 0; i<*nprofiles; i++)
	{
		loadedConfigs->profileIndex = i;
		loadedConfigs->loaded = false;
		loadedConfigs++;
	}

}

void internal_freeLoadedConfigs()
{
	loadedConfigs = firstLoadedConfig;
	free(loadedConfigs);
}

bool internal_profileIsLoaded(int profileIndex)
{

	loadedConfigs = firstLoadedConfig;
	for(int i = 0; i<nconfigs; i++) 
	{
		if (loadedConfigs[i].profileIndex == profileIndex)
		{
			return loadedConfigs[i].loaded;
		}
	}

	return false;
}
// -----------  API impl --------------
void wdcd_listDevices(XDeviceInfo* devices,int *ndevices)
{
    int i=0;
    for(i = 0; i < *ndevices;i++) {
		char message[50];
        sprintf(message, "device name: %s",devices->name);
		log_message(message);
        devices++;
    }
       
}

void runXSetWacom(char* deviceName, char* parameter) 
{
    char cmd[100];
    sprintf(cmd,"%s --set  \"%s\" %s 1>/tmp/wdcd.out.log 2>>/tmp/wdcd.err.log",XSETWACOM_COMMAND,deviceName,parameter);
    int ret = system(cmd);
	char msg[100];
	sprintf(msg,"Loaded param %s for device %s with result: %d",parameter,deviceName,ret);
	log_message(msg);
}
 
void exitIfNull(void* profiles)
{
	if (profiles == NULL) {
		printf("No profiles available. Exit code -1\n");
		exit(-1);
	}
}	
 
void wdcd_readProfilesOrExit(char* path, void** profiles,int* nprofiles)
{
	readProfiles(path,profiles, nprofiles);
	exitIfNull(profiles);
	
	internal_nprofiles = *nprofiles;
	internal_setupLoadedProfiles(profiles,nprofiles);
}
 
void listProfiles(void* profiles, int nprofiles)
{
	for (int i = 0; i<nprofiles; i++)
	{
		char *name = profile_getName(profiles,i);
		printf("%s\n",name);
		int nparameters = profile_getNumberOfParameters(profiles,i);
		char *param_value;
		for (int j = 0; j<nparameters; j++)
		{
			param_value = profile_getDeviceParameter(profiles,i,j);
			printf("%s\n",param_value);
		}
		
	}

}

bool wdcd_profileExistsForDevice(char* deviceName, XDeviceInfo* devices,int *ndevices)
{
    int i=0;
    for(i = 0; i < *ndevices;i++) {
        if(strcmp(devices->name,deviceName)==0) {
            return 1;
        }
        devices++;
    }
    return 0;
       
}

int wdcd_profileIsLoadedForDevice(char* name)
{
	return 1; 
}


void wdcd_loadProfile(int profileIndex,void* profiles)
{
	if(internal_profileIsLoaded(profileIndex))
	{
		return;
	}

	int nparameters = profile_getNumberOfParameters(profiles,profileIndex);
	char *param_value;
	char* deviceName = profile_getName(profiles,profileIndex);

	for (int j = 0; j<nparameters; j++)
	{
		param_value = profile_getDeviceParameter(profiles,profileIndex,j);
		runXSetWacom(deviceName,param_value);
	}  

	internal_setProfileLoadedForDevice(profileIndex);
	
	char message[100];
	sprintf(message,"Loaded config profile for device: %s",deviceName);
	log_message(message);
}

void wdcd_unloadProfile(int profileIndex)
{
	internal_setProfileUnloadedForDevice(profileIndex);
}

void wdcd_freeProfiles(void* profiles) {
	freeProfiles(profiles);
	internal_freeLoadedConfigs();
	log_message("Profiles have been released");
}



int wdcd_verifySystemRequirements()
{
    char cmd[100];
    sprintf(cmd,"%s 1>/dev/null 2>/dev/null",XSETWACOM_COMMAND);

	int retVal = system(cmd);

	if(retVal ==0) 
	{
		sprintf(cmd,"Command %s not found in system.",XSETWACOM_COMMAND);
		log_message(cmd);
	}

    return retVal;
}

char* wdcd_version() 
{
    return WDCD_VERSION;
}
