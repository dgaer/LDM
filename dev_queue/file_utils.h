/************************************/
/*********** Start of File **********/
/************************************/
/* ----------------------------------------------------------- */
/* C Header File */
/* C Compiler Used: GNU, Intel */
/* Produced By: Douglas.Gaer@noaa.gov */
/* File Creation Date: 04/14/2011 */
/* Date Last Modified: 05/31/2016 */
/* - */
/* Version control: SVN */
/* Support Team: */
/* Contributors: */
/* ----------------------------------------------------------- */
/* ---------- Include File Description and Details ----------- */
/* ----------------------------------------------------------- */
/*
This software and documentation was produced within NOAA
and is intended for internal agency use only. Please don't
distribute any part of this software or documentation without
first contacting the original author or NOAA agency that
produced this software. All third party libraries used to
build this application are subject to the licensing agreement
stated within the source code and any documentation supplied
with the third party library.

File utilities ported from my C++ library to C in order to work
with the LDM libary. NOTE: The LDM library will not link C++ 
object files so must use C code for LDM projects that require 
linking to the LDM library. 

*/
/* ----------------------------------------------------------- */
#ifdef __cplusplus /* Allow use with C++ */
extern "C" { 
#endif

#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

// Non-platform specific include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// Platform specific include files 
#if defined (__WIN32__)
// WIN32 include files for file/directory functions 
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <share.h>

#elif defined (__UNIX__)
// UNIX include files for file/directory functions for all
// UNIX variants. NOTE: Place specific UNIX variants above
// this directive.
#include <unistd.h> // UNIX standard function definitions
#include <dirent.h> // POSIX directory stream functions 
#else
#error You must define a target platform: __WIN32__ or __UNIX__
#endif

// 09/07/2006: Includes for utime calls
#if defined (__WIN32__)
#include <sys/utime.h>
#endif

#if defined (__LINUX__)
#include <utime.h>
#endif

// TODO: Have to add utime includes for HPUX and Solaris
// after testing set timestamp function.

#if defined (__64_BIT_DATABASE_ENGINE__) || defined (__ENABLE_64_BIT_INTEGERS__)
#include "gxdtyp64.h"

// Platform specific UNIX include files required for 
// 64-bit disk space calculations
#if defined (__UNIX__) && defined (__LINUX__)
#include <sys/statfs.h>
#elif defined (__UNIX__)
#include <sys/statvfs.h>
#endif
#endif

// Platform independent path and directory length definitions 
const unsigned file_utils_MAX_LINE_LENGTH = 1024; // Max line length
const unsigned file_utils_MAX_NAME_LENGTH = 255;  // Max length of a file name
const unsigned file_utils_MAX_DIR_LENGTH  = 1024; // Max length of a directory name
const unsigned file_utils_MAX_PATH_LENGTH = 1024; // Max absolute or relative path 

// Standalone platform indepentent file/directory functions    //
// ----------------------------------------------------------- //
int file_utils_exists(const char *name);
int file_utils_isdirectory(const char *dir_name);
int file_utils_isfile(const char *fname);
void file_utils_makeDOSpath(char *path);
void file_utils_makeUNIXpath(char *path);
int file_utils_hasdriveletter(const char *dir_name, char *letter);
int file_utils_mkdir(const char *dir_name);
int file_utils_is_unc_path(const char *path);

#endif  /* __FILE_UTILS__ */

#ifdef __cplusplus /* Allow use with C++ */
  }
#endif
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/






