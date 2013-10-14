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

#ifndef WDCD_WACOM_DEVICE_PROFILE_H
#define WDCD_WACOM_DEVICE_PROFILE_H 1
/**
 * This is simple abstraction layer for
 * wdcd configuration file access.
 *
 * This interface may heavely change in feature.
 *
 * Basic usage:
 * 1. readProfiles
 * 2. get number of profiles
 * 3. iterate through profiles
 * 3.1 do something according to profile
 *
 */
/**
 * Read profiles from specified path.
 * If path not found or file has bad format
 * then profiles points to 0 and nprofiles has 0 value.
 *
 * @param path - path to file with profiles
 * @param nprofiles - shall return number of profiles
 * @param profiles  - return pointer to profiles
 *
 */
void readProfiles(char* path,void** profiles, int* nprofiles);

/**
 * Free memory allocated with readProfiles method.
 * Specified pointer must be the one returned by readProfiles.
 *
 * @param profiles - pointer to profiles which should be freed
 */
void freeProfiles(void* profiles);


/**
 * Convienient use of free and read profiles.
 * At first this method frees specified profiles.
 * Then it reads new profiles from path.
 * After reloading specified pointer to profiles
 * becomes invalid.
 *
 * @param profiles - pointer to profiles that should be reloaded
 * @param path - path to file from which profiles should be reloaded
 *
 * @return pointer to newly readed profiles
 */
//void* reloadProfile(void** profiles,char* path);

/**
 * Return pointer to char* with profile name.
 * if profileIndex not found then return 0
 * Name is delimited with \0.
 *
 * @returns pointer to profile name, or 0 if profile not found.
 */
char* profile_getName(void* __profiles, int profileIndex);


/**
 * Return parameter value specified by paramIndex.
 * Given parameter is found by paramIndex and profileIndex within profiles.
 *
 * @return pointer to parameter value
 */
char* profile_getDeviceParameter(void* profiles, int profileIndex,int paramIndex);

/**
 * Return number of parameters available in specified profileIndex.
 */
int profile_getNumberOfParameters(void* profiles, int profileIndex);

#endif /* profile.h */
