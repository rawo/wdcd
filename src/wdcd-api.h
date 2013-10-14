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

#ifndef WDCD_WACOM_DEVICE_API_H
#define WDCD_WACOM_DEVICE_API_H 1

#include <X11/Xlib.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>
#include <stdbool.h>

#define WDCD_VERSION "0.1"

/**
 * printf all devices found in x-server
 */
void wdcd_listDevices(XDeviceInfo* devices,int *ndevices);

/**
 * Attempts to find specified deviceName in list of devices
 * Returns true if deviceName found, false otherwise.
 */
bool wdcd_profileExistsForDevice(char* deviceName, XDeviceInfo* devices,int *ndevices);


/**
 * calls exit(0) if profiles is NULL
 */
void exitIfNull(void* profiles);

void listProfiles(void* __profiles, int nprofiles);

/**
 * Attempts to load profiles from file to memory.
 * Exits if laoding resulted with empty profiles array.
 *
 * @param path - path to profiles file
 * @param profiles - this will have a list of readed profiles
 */
void wdcd_readProfilesOrExit(char* path, void** profiles,int* nprofiles);

/**
 * Attempts to load profile for specified device name.
 * Profile is searched within specified profiles.
 * If given profile name is already loaded then this method
 * has no effect
 *
 * If profile is not found then this method has no effect
 * @profileIndex - index of profile 
 * @profiles - struct of profiles
 *
 */
void wdcd_loadProfile(int profileIndex ,void* profiles);

/**
 */
void wdcd_unloadProfile(int profileIndex);


void wdcd_freeProfiles(void* profiles);

/**
 * Verifies system requirements.
 * Return 1 if requirements meet, 0 otherwise.
 * See logs for details.
 */
int wdcd_verifySystemRequirements();


char* wdcd_version();
#endif /* wdcd-api.h */
