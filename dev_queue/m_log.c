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

Logging functions.

*/
/* ----------------------------------------------------------- */

void get_syslog_time(char sbuf[255])
// Function used to make a Syslog time string. Returns a
// null terminated time string in the following format: 
// Month day HH:MM:SS
{
  time_t STime;
  struct tm *TimeBuffer;

  if(sbuf == NULL) return;

  char month[25]; char day[25];
  char hour[25]; char minutes[25]; char seconds[25];
  time(&STime);

#if defined (__WIN32__)
  TimeBuffer = localtime(&STime);
#elif defined (__UNIX__) && defined (__REENTRANT__)
  // Using the UNIX localtime_r() function instead of localtime()
  localtime_r(&STime, TimeBuffer);
#else // Use localtime() by default
  TimeBuffer = localtime(&STime);
#endif

  strftime(month, 25, "%b", TimeBuffer);
  strftime(day, 25, "%d", TimeBuffer);
  strftime(hour, 25, "%H", TimeBuffer);
  strftime(minutes, 25, "%M", TimeBuffer);
  strftime(seconds, 25, "%S", TimeBuffer);

  memset(sbuf, 0, 255);
  sprintf(sbuf, "%s %s %s:%s:%s", month, day, hour, minutes, seconds);
}

void print_message(const char *s1, const char *s2, const char *s3)
{
  print_block(s1, s2, s3, 0, 0);
}

void print_blob(const char *s1, const char *s2, const char *s3)

{
  print_block(s1, s2, s3, 0, 1);
}

void print_lines(const char *s1, const char *s2, const char *s3)
{
  print_block(s1, s2, s3, 1, 0);
}

int print_block(const char *mesg1, const char *mesg2, const char *mesg3, 
		int lines, int blob)
// Non-Thread safe print function used to print and log a console message
{
  if(mesg1 == NULL) return 0;
  char syslogtime[255];
  char output_message[4096];
  char prefix[255];
  FILE *fptr;
  unsigned bytes = 0;

  get_syslog_time(syslogtime);
  memset(output_message, 0, sizeof(output_message));
  memset(prefix, 0, sizeof(prefix));

  sprintf(prefix, "%s %s:", syslogtime, service_name);

  if(lines) {
    if(mesg1 != NULL) sprintf(output_message, "%s %s\n", prefix, mesg1);
    if(mesg2 != NULL) sprintf(output_message, "%s %s\n", prefix, mesg2);
    if(mesg3 != NULL) sprintf(output_message, "%s %s\n", prefix, mesg3);
  }
  else if(blob) {
    sprintf(output_message, "%s Message Block <<\n", prefix);
    if(mesg1) strcat(output_message, mesg1);
    if(mesg2) strcat(output_message, mesg2); 
    if(mesg3) strcat(output_message, mesg3); 
    strcat(output_message, "\n>>\n");
  }
  else {
    sprintf(output_message, "%s ", prefix);
    if(mesg1) strcat(output_message, mesg1);
    if(mesg2) {
      strcat(output_message, " ");
      strcat(output_message, mesg2); 
    }
    if(mesg3) {
      strcat(output_message, " ");
      strcat(output_message, mesg3); 
    }
    strcat(output_message, "\n");
  }

  if(enable_logging == 1) {
    fptr = fopen(logfile_name, "a+b");
    if(fptr) {
      bytes = fwrite((const unsigned char *)output_message, 1, strlen(output_message), fptr);
      if(bytes != strlen(output_message)) {
	sprintf(output_message, "%s ERROR: Error writing to log file %s\n", prefix, logfile_name);
	fprintf(stderr, "%s", output_message);
	fflush(stderr);
	enable_logging = 0;
	sprintf(output_message, "%s INFO: logging has been disabled\n", prefix);
	fprintf(stderr, "%s", output_message);
	fflush(stderr);
      }
      else {
	fflush(fptr);
      }
      fclose(fptr);
    }
    else { // Could not write to log file so echo to the stderr
      sprintf(output_message, "%s ERROR: Error opening log file %s\n", prefix, logfile_name);
      fprintf(stderr, "%s", output_message);
      fflush(stderr);
      enable_logging = 0;
      sprintf(output_message, "%s INFO: logging has been disabled\n", prefix);
      fprintf(stderr, "%s", output_message);
      fflush(stderr);
    }
  }

  //  if(verbose) {
    fprintf(stderr, "%s", output_message);
    fflush(stderr);
    // }

  return 1;
}
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
