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

NOTE: All the LDM functions were taken from the LDM pqutil source 
code Original code copyright: Copyright 1995, University Corporation 
for Atmospheric Research See top level COPYRIGHT file for copying 
and redistribution conditions.

Copy of C functions from LDM library for use with DEV queue monitor.

*/
/* ----------------------------------------------------------- */
#ifdef __cplusplus /* Allow use with C++ */
extern "C" { 
#endif

#ifndef __LDM_FUNCTIONS_H__
#define __LDM_FUNCTIONS_H__

// STDLIB includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/signal.h>

// LDM includes
#include "ldm.h"
#include "ldmprint.h"
#include "atofeedt.h"
#include "globals.h"
#include "inetutil.h"
#include "md5.h"
#include "pq.h"
#include "ulog.h"
#include "RegularExpressions.h"

char *ldms_prod_info(char *buf, size_t bufsize, const prod_info *infop, int doSignature);
int parse_time(char *in_string, timestampt *result);
int set_stdin(int vmin);
int reset_stdin();
static int display_watch(const prod_info *infop, const void *datap,
			 void *xprod, size_t size_notused, void *notused);
void init_options(pqueue *pq, prod_class *clssp, product *prod,
		  pq_match *dir, char *pattern); 
void watch_queue(pqueue *pq, prod_class  *clssp);

#endif  /* __LDM_FUNCTIONS_H__ */

#ifdef __cplusplus /* Allow use with C++ */
  }
#endif
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/

