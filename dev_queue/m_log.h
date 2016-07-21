/************************************/
/*********** Start of File **********/
/************************************/
/* ----------------------------------------------------------- */
/* C Header File */
/* C Compiler Used: GNU, Intel */
/* Produced By: Douglas.Gaer@noaa.gov */
/* File Creation Date: 04/01/2011 */
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

Logging functions.

*/
/* ----------------------------------------------------------- */
#ifdef __cplusplus /* Allow use with C++ */
extern "C" { 
#endif

#ifndef __M_LOG_H__
#define __M_LOG_H__

void get_syslog_time(char sbuf[255]);
void print_message(const char *s1, const char *s2, const char *s3);
void print_blob(const char *s1, const char *s2, const char *s3);
void print_lines(const char *s1, const char *s2, const char *s3);
int print_block(const char *mesg1, const char *mesg2, const char *mesg3, 
		int lines, int blob);


#endif  /* __M_LOG_H__ */

#ifdef __cplusplus /* Allow use with C++ */
  }
#endif
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/

