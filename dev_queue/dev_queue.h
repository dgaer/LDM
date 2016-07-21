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

LDM utility used to read the LDM queue and write info files
for LDM product groups.

*/
/* ----------------------------------------------------------- */
#ifdef __cplusplus /* Allow use with C++ */
extern "C" { 
#endif

#ifndef __DEV_QUEUE_H__
#define __DEV_QUEUE_H__

#include "ldm_functions.h"
#include "file_utils.h"
#include "m_help.h"
#include "m_proc.h"
#include "m_config.h"
#include "m_log.h"

// Data structure used to hold formatted product information
struct LDM_product_info {
  time_t epoch_time;  // Epoch time of product arrival
  char str_time[255]; // LDM time string of product arrival
  char sig[255];      // Signature
  char org[255];      // Origin
  char feedtype[255]; // Feedtype
  int seqnum;         // Sequence number
  char id[255];       // Identification   
  unsigned long size; // Size of product

  // Our non-LDM data memebers
  char output_file_name[255]; // Unique file for product set
  
};
typedef struct LDM_product_info LDM_product_info;

/* DEV queue functions */
int reset_globals(void);
int write_ldm_queue(char *ldm_queue_path);
static int write_watch_files(const prod_info *infop, const void *datap,
			     void *xprod, size_t size_notused, void *notused);
void reset_ldm_product_info(LDM_product_info *pinfo);
static int write_watch_files(const prod_info *infop, const void *datap,
			     void *xprod, size_t size_notused, void *notused) ;
int init_dev_queue_options(pqueue *pq, prod_class *clssp, product *prod,
			   pq_match *dir, char *pattern);
int write_group_file(LDM_product_info *pinfo);
  int write_group_xml_file(LDM_product_info *pinfo);

#endif  /* __DEV_QUEUE_H__ */

#ifdef __cplusplus /* Allow use with C++ */
  }
#endif
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/

