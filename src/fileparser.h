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

#ifndef WDCD_FILE_PARSER_H 
#define WDCD_FILE_PARSER_H 1


/**
 * Read number of profiles in file under specified path.
 *
 * @return number of profiles of -1 if failed to read file
 */
int cfgfile_getnumberofprofiles(char* path);

/**
 * shifts internal pointer to next profile in cfg file.
 * returns 0 if no more profiles, positive number otherwise.
 */
int cfgfile_profile_readnext();

/**
 * returns name of profile that was read by cfgfile_readnext_profile
 * profile name is put into name if available.
 */
void  profile_getname(char* name);

/**
 * read next parameter from currently selected profile.
 * profile is selected by calling cfgfile_profile_readnext.
 * 
 * returns number of read characters or 0 if no characters read.
 */
//void profile_getattributes(char* attrs, int* nattrs);
int profile_getnext_parameter(char* param);

int profile_getnumberofparams();
#endif /* fileparser.h */
