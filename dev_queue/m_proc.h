/************************************/
/*********** Start of File **********/
/************************************/
/* ----------------------------------------------------------- */
/* C Header File */
/* C Compiler Used: GNU, Intel */
/* Produced By: Douglas.Gaer@noaa.gov */
/* File Creation Date: 04/01/2011 */
/* Date Last Modified: 07/21/2016 */
/* - */
/* Version control: 2.03 */
/* Support Team: */
/* Contributors: */
/* ----------------------------------------------------------- */
/* ---------- Include File Description and Details ----------- */
/* ----------------------------------------------------------- */
/*
This program was produced by Douglas.Gaer@noaa.gov for the purpose of
displaying LDM queue data on internal Web servers. This software can
be freely modified and distributed, but without a warranty of any
kind. Use for any purpose is not guaranteed. All third party libraries
used to build this application are subject to the licensing agreement
stated within the source code and any documentation supplied with the
third party library.

Process functions.

*/
/* ----------------------------------------------------------- */
#ifdef __cplusplus /* Allow use with C++ */
extern "C" { 
#endif

#ifndef __M_PROC_H__
#define __M_PROC_H__

// Signal I/O process functions
int num_seg_violations = 0;
void termination_handler(int signum);

// Main process functions
int StopProc();
void ExitProc();

#endif  /* __M_PROC_H__ */

#ifdef __cplusplus /* Allow use with C++ */
  }
#endif
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/

