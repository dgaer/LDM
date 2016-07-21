/************************************/
/*********** Start of File **********/
/************************************/
/* ----------------------------------------------------------- */
/* C Source Code File */
/* C Compiler Used: GNU, Intel */
/* Produced By: Douglas.Gaer@noaa.gov */
/* File Creation Date: 04/14/2011 */
/* Date Last Modified: 07/21/2016 */
/* - */
/* Version control: 2.03 */
/* Support Team: */
/* Contributors: */
/* ----------------------------------------------------------- */
/* ------------- Program Description and Details ------------- */
/* ----------------------------------------------------------- */
/*
This program was produced by Douglas.Gaer@noaa.gov for the purpose of
displaying LDM queue data on internal Web servers. This software can
be freely modified and distributed, but without a warranty of any
kind. Use for any purpose is not guaranteed. All third party libraries
used to build this application are subject to the licensing agreement
stated within the source code and any documentation supplied with the
third party library.

File utilities ported from my C++ library to C in order to work
with the LDM libary. NOTE: The LDM library will not link C++ 
object files so must use C code for LDM projects that require 
linking to the LDM library. 

*/
/* ----------------------------------------------------------- */

#include "file_utils.h"

// -----------------------------------------------------------
// BEGIN: Windows and UNIX Compatibility macros
// -----------------------------------------------------------
#ifndef __S_ISTYPE
#define	__S_ISTYPE(mode, mask) (((mode) & 0170000) == (mask))
#endif
#ifndef S_ISDIR
#define	S_ISDIR(mode)  __S_ISTYPE((mode), 0040000)
#endif
#ifndef S_ISCHR
#define	S_ISCHR(mode)  __S_ISTYPE((mode), 0020000)
#endif
#ifndef S_ISBLK
#define	S_ISBLK(mode)  __S_ISTYPE((mode), 0060000)
#endif
#ifndef S_ISREG
#define	S_ISREG(mode)  __S_ISTYPE((mode), 0100000)
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(mode) __S_ISTYPE((mode), 0010000)
#endif
#ifndef S_ISLNK
#define S_ISLNK(mode)  __S_ISTYPE((mode), 0120000)
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(mode) __S_ISTYPE((mode), 0140000)
#endif
#ifndef S_ISUID
#define	S_ISUID 04000
#endif
#ifndef S_ISGID
#define	S_ISGID	02000
#endif
#ifndef S_ISVTX
#define S_ISVTX	01000
#endif
#ifndef S_IRUSR
#define	S_IRUSR	0400
#endif
#ifndef S_IWUSR
#define	S_IWUSR	0200
#endif
#ifndef S_IXUSR
#define	S_IXUSR	0100
#endif
#ifndef S_IRWXU
#define	S_IRWXU	(0400|0200|0100)
#endif
#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif
#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif
#ifndef S_IEXEC
#define S_IEXEC S_IXUSR
#endif
#ifndef S_IRGRP
#define	S_IRGRP (S_IRUSR >> 3)
#endif
#ifndef S_IWGRP
#define	S_IWGRP (S_IWUSR >> 3)
#endif
#ifndef S_IXGRP
#define	S_IXGRP (S_IXUSR >> 3)
#endif
#ifndef S_IRWXG
#define	S_IRWXG (S_IRWXU >> 3)
#endif
#ifndef S_IROTH
#define	S_IROTH (S_IRGRP >> 3)
#endif
#ifndef S_IWOTH
#define	S_IWOTH (S_IWGRP >> 3)
#endif
#ifndef S_IXOTH
#define	S_IXOTH (S_IXGRP >> 3)
#endif
#ifndef S_IRWXO
#define	S_IRWXO (S_IRWXG >> 3)
#endif
// -----------------------------------------------------------
// BEGIN: Windows and UNIX Compatibility macros
// -----------------------------------------------------------

// ----------------------------------------------------------- //
// Standalone platform independent file/directory functions    //
// ----------------------------------------------------------- //
int file_utils_exists(const char *name)
// Tests to see if the specified directory or file name 
// exists. Returns true if the file or directory exists. 
{
  struct stat buf;
  return stat(name, &buf) == 0;
}

int file_utils_isfile(const char *fname)
// Returns true if the specified name is any kind of file 
// or false if this is a directory.
{
  return ((file_utils_isdirectory(fname) == 0) && (file_utils_exists(fname))); 
}

int file_utils_isdirectory(const char *dir_name) 
// Returns true if the specified name is a directory
{
  struct stat buf;

#if defined (__LINUX__)
  if(lstat(dir_name, &buf) != 0) return 0;
#if defined (S_ISDIR)
  return S_ISDIR(buf.st_mode);
#else
  if((buf.st_mode & S_IFMT) == S_IFDIR) return 1;
#endif
#endif // __LINUX__

  if(stat(dir_name, &buf) != 0) return 0;

#if defined (_S_IFMT) && defined(_S_IFDIR)
  if((buf.st_mode & _S_IFMT) == _S_IFDIR) return 1;
#else
  if((buf.st_mode & S_IFMT) == S_IFDIR) return 1;
#endif
  
 return 0;
}

void file_utils_makeDOSpath(char *path)
// Make a DOS directory path by changing all
// forward slash path separators with back
// slashes.
{
  unsigned len = strlen(path);
  while(len--) {
    char c = *path;
    switch(c) {
      case '/' :
	*path = '\\';
	break;
      default:
	break;
    }
    path++;
  }
}

int file_utils_hasdriveletter(const char *dir_name, char *letter)
// WIN32 function used to parse the drive letter from the
// specified directory. Returns false if the path does not
// contain a drive letter. If a drive letter is found it will
// be passed back in the "letter" variable.
{
  // Check for null pointers
  if(!dir_name) return 0; 
  if(!letter) return 0;

  // Reset the drive letter variable
  *letter = 0;

  // Test the string for the minimum number of characters
  if(strlen(dir_name) < 2) return 0;

  if(dir_name[1] != ':') return 0; // No drive letter found

  // Get the drive letter following the first colon
  *letter = dir_name[0];

  return 1;
}

int file_utils_is_unc_path(const char *path)
// Returns true if this is a UNC path.
// UNC path = \\hostname\resource\location
{
  // 09/07/2006: Added to handle UNC paths

  if(!path) return 0;
  if(strlen(path) < 2) return 0;
  
  if(path[0] == '\\') {
    if(path[1] == '\\') {
      return 1;
    }
  }
  return 0;
}

void file_utils_makeUNIXpath(char *path)
// Make a UNIX directory path by changing all
// back slash path separators with forward
// slashes.
{
  unsigned len = strlen(path);
  while(len--) {
    char c = *path;
    switch(c) {
      case '\\' :
	*path = '/';
	break;
      default:
	break;
    }
    path++;
  }
}

int file_utils_mkdir(const char *dir_name)
// Make the specified directory (with sub-directories) 
// if the directory if it does not exist. Returns a 
// non-zero value if an error occurs. UNIX file systems 
// will use 755 permissions when new directories are 
// created.
{
  char *path = (char *)dir_name;
  char path_sep;
  char sbuf[file_utils_MAX_PATH_LENGTH];
  unsigned i = 0; int rv;  
  unsigned len;
  char *s;
  int num_paths = 0;
  
#if defined (__WIN32__)
  char drive_letter = 0;
  int has_drive_letter = 0;
  int unc_path = 0;
#endif
  
  if(file_utils_exists(dir_name)) {
    // Directory exists, return with no error reported
    return 0;
  }

#if defined (__WIN32__)
  has_drive_letter = file_utils_hasdriveletter(dir_name, &drive_letter);
  unc_path = file_utils_is_unc_path(dir_name);
  file_utils_makeDOSpath(path);
  path_sep = '\\';
#elif defined (__UNIX__)
  file_utils_makeUNIXpath(path);
  path_sep = '/';
#else
#error You must define a file system: __WIN32__ or __UNIX__
#endif

  // The mkdir function can only create one new directory per call
  // so the complete directory path can only be created one sub-
  // directory at a time.
  for(i = 0; i < file_utils_MAX_PATH_LENGTH; i++) sbuf[i] = '\0';

  len = strlen(path); s = path;
  if(len > file_utils_MAX_PATH_LENGTH) len = file_utils_MAX_PATH_LENGTH;

  for(i = 0; i < len; i++, s++) {

    // Get the whole directory path
    if(i == (len-1)) { memmove(sbuf, path, len); break; }

#if defined (__WIN32__)
    if((has_drive_letter) && (i == 1)) { // Skip over the drive letter
      i++; s++; 
      if(*s == path_sep) { i++; s++; }
    }
    if((unc_path) && (i == 0)) {
      i++; s++; 
      if(*s == path_sep) { i++; s++; }
    }
    if((unc_path) && (i == 1)) {
      i++; s++; 
      if(*s == path_sep) { i++; s++; }
    }
#endif

    if((*s == path_sep) && (i > 1)) { // Step past the root directory
      memmove(sbuf, path, i);
      num_paths++;

#if defined (__WIN32__)
      // 09/07/2006: Modified to handle UNC paths
      // UNC path = \\hostname\resource\location
      if(unc_path) {
	rv = 0;
	if(num_paths > 2) {
	  if(!file_utils_exists(sbuf)) {
	    if(mkdir(sbuf) != 0) return 1;
	  }
	}
      }
      else {
	if(!file_utils_exists(sbuf)) {
	  if(mkdir(sbuf) != 0) return 1;
	}
      }
#elif defined (__UNIX__)
      if(!file_utils_exists(sbuf)) {
	if(mkdir(sbuf,
		 S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0) return 1;
      }
#else
#error You must define a file system: __WIN32__ or __UNIX__
#endif
    }
  }

  // Get rid of any terminating path separators
  if(sbuf[len-1] == path_sep) sbuf[len-1] = '\0';

  // Make the complete directory path if needed
  if(!file_utils_exists(sbuf)) {
#if defined (__WIN32__)
    rv = mkdir(sbuf);
#elif defined(__UNIX__)
    rv = mkdir(sbuf,
	       S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
#else
#error You must define a file system: __WIN32__ or __UNIX__
#endif
    if(rv != 0) {
      return 1;
    }
  }      

  return 0;
}

/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
