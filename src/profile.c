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
#include "profile.h"
#include "fileparser.h"

typedef struct _DeviceParameter
{
	char value[200];

} DeviceParameter;

typedef struct _DeviceParameters
{
	int nattributes;
	DeviceParameter* attribute;
	DeviceParameter* first;
} DeviceParameters;

typedef struct _DeviceProfile
{
	char name[50];
	DeviceParameters attributes;
} DeviceProfile;


typedef struct _Profiles
{
	int nprofiles;
	DeviceProfile* profile;
	DeviceProfile* first;
} Profiles;

void readProfiles(char* path,void** outProfiles, int* nprofiles)
{
	*nprofiles = cfgfile_getnumberofprofiles(path);

	Profiles* profiles = (Profiles*)malloc(sizeof(Profiles));
	profiles->profile = (DeviceProfile*)malloc(*nprofiles*sizeof(DeviceProfile));
	profiles->first = profiles->profile;

	for (int i = 0; i<*nprofiles ;i++)
	{
		cfgfile_profile_readnext();
		
		memset(&profiles->profile->name,0,sizeof(profiles->profile->name));

		profile_getname(profiles->profile->name);

		//setup profile device parameters
		int nparams = profile_getnumberofparams();
		DeviceParameters* attributes = &profiles->profile->attributes;
		attributes->attribute = malloc(nparams*sizeof(DeviceParameter));
		attributes->first = attributes->attribute;
		attributes->nattributes = nparams;

		for (int n = 0; n <nparams; n++)
		{
			profile_getnext_parameter(attributes->attribute->value);

			attributes->attribute++;
			
		}
		profiles->profile++;
	}
	profiles->nprofiles = *nprofiles;


	*outProfiles = profiles;
}

void freeProfiles(void* inProfiles)
{
	Profiles* _profiles = (Profiles*)inProfiles;
	_profiles->profile = _profiles->first;

	for(int i = 0; i < _profiles->nprofiles; i++)
	{
		DeviceProfile* p_ptr = _profiles->profile;
		DeviceParameters* attrs; 
		attrs = &p_ptr->attributes;
		//free all parameters in profile
		free(attrs->first);

		_profiles->profile++;
	}
	//free all profiles
 	free(_profiles->first);
	//free profiles container
	free(_profiles);
}


char *profile_getName(void* _profiles, int index)
{
	Profiles* profiles = (Profiles*)_profiles; 
	if (index > profiles->nprofiles)
		return NULL;
	
	//shift to begining
	profiles->profile = profiles->first;

	DeviceProfile* profile = profiles->profile+index;
	return profile->name;
}

char* profile_getDeviceParameter(void* __profiles, int index,int paramIndex)
{
	Profiles* profiles = (Profiles*)__profiles; 
	if (index > profiles->nprofiles)
		return 0;

	//shift to begining
	profiles->profile = profiles->first;
	DeviceProfile* profile = profiles->profile+index;

	DeviceParameters *attrs = &profile->attributes;
	attrs->attribute = attrs->first;
	DeviceParameter *parameter = attrs->attribute+paramIndex;

	return parameter->value;;
}

int profile_getNumberOfParameters(void* __profiles, int index)
{
	Profiles* profiles = (Profiles*)__profiles; 
	if (index > profiles->nprofiles)
		return 0;

	//shift to begining
	profiles->profile = profiles->first;
	DeviceProfile* profile = profiles->profile+index;

	DeviceParameters *attrs = &profile->attributes;
	return attrs->nattributes;

}
/* 
typedef struct _ProfileAttributes
{
	char* name;
	char* value;
} ProfileAttributes;

typedef struct _WacomDeviceProfile 
{
	int id;
	char* name;
	int nproperties;
	ProfileAttributes properties[];

} WacomDeviceProfile;

typedef struct _WacomDeviceProfiles
{
	const int nprofiles;
	WacomDeviceProfile* profiles;
} WacomDeviceProfiles;

WacomDeviceProfiles* readProfiles(char* path) 
{
	int numberOfProfiles = 1;
	int *nprofiles = &numberOfProfiles;
	WacomDeviceProfiles* profiles = (WacomDeviceProfiles*)malloc(sizeof(WacomDeviceProfile));
	profiles->profiles = (WacomDeviceProfile*)malloc((*nprofiles)*sizeof(WacomDeviceProfile));
	firstProfile = profiles->profiles;
	
	profiles->profiles->name = "First profile name1\0";
	profiles->profiles->id = 1;
	profiles->profiles->properties = (ProfileAttributes*)malloc(nproperties*sizeof(ProfileAttributes));


	return profiles;
}
*/
