/************************************/
/*********** Start of File **********/
/************************************/
/* ----------------------------------------------------------- */
/* C Source Code File */
/* C Compiler Used: GNU, Intel */
/* Produced By: Douglas.Gaer@noaa.gov */
/* File Creation Date: 04/01/2011 */
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

Help functions.

*/
/* ----------------------------------------------------------- */

void print_version(void)
{
  printf("\n%s version %s\n", program_name, version_string);
  printf("Produced by: %s\n", produced_by);
  printf("Project: %s\n", project_name);
}

void print_help(void)
{
  print_version();
  printf("Usage: %s [args]\n", service_name);
  printf("args:\n");
  printf("     -h                 Print help message and exit\n");
  printf("     -v                 Turn on verbose messaging to stderr\n");
  printf("     -d                 Enable debug mode default level -d1\n");
  printf("     -d#                Set debug log level -d1, d2, d3, etc\n");
  printf("     -l                 Log to default log file\n");
  printf("     -l\"mylogfile.log\"  Specify log file to use\n");
  printf("     -o\"outputDIR\"      Specify an output DIR to use\n");
  printf("     -q\"queuefile\"      Specify an LDM queue file to use\n");
  printf("     -t                 Write text group files\n");
  printf("\n");
}

/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
