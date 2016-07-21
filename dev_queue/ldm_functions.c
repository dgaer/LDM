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

NOTE: All the LDM functions were taken from the LDM pqutil source 
code Original code copyright: Copyright 1995, University Corporation 
for Atmospheric Research See top level COPYRIGHT file for copying 
and redistribution conditions.

Copy of C functions from LDM library for use with DEV queue monitor.

*/
/* ----------------------------------------------------------- */

#include "ldm_functions.h"

int parse_time(char *in_string, timestampt *result) 
{
  if (!strcmp(in_string,"0")) { /* set to beginning of the epoch */
    *result = TS_ZERO;
    return(0);
  }
  else if (!strcmp(in_string, "EOT")) { /* set to end of the epoch */
    *result = TS_ENDT;
    return(0);
  }
  else if (!strcmp(in_string,"NOW")) { /* set to current time */
    gettimeofday(result,NULL);
    return(0);
  }
  else { /* parse the date-time string */
    int     ret_val;
    struct tm       time_conv;
    
    ret_val = sscanf(in_string,"%4d%2d%2d%2d%2d", &time_conv.tm_year,
		     &time_conv.tm_mon, &time_conv.tm_mday,
		     &time_conv.tm_hour, &time_conv.tm_min);
    if (ret_val != 5) { /* bad input */
      print_message("ERROR: Bad time string: %s", in_string, NULL);
      return(1);
    }
    time_conv.tm_sec = 0;
    time_conv.tm_year -= 1900;
    time_conv.tm_mon--;
    time_conv.tm_isdst = -1;
    
    result->tv_sec = mktime(&time_conv);
    
    return(0);
  }
}

char *ldms_prod_info(char *buf, size_t bufsize, const prod_info *infop, int doSignature)
{
  int conv;
  size_t len = 0;

  if(buf == NULL) {
    buf = ldm_tprintbuf;
    bufsize = sizeof(ldm_tprintbuf);
  }

  if((doSignature && bufsize < 33 + 50 + KEYSIZE) || bufsize < 50 + KEYSIZE) return NULL;

  (void) memset(buf, 0, bufsize);

  if(doSignature) {
    conv = sprint_signaturet(&buf[len], bufsize, infop->signature);
    len += (size_t)conv;
    bufsize -= (size_t)conv;
    buf[len++] = ' ';
    buf[len] = 0;
    bufsize--;
  }

  conv = sprintf(&buf[len],"%8u ", infop->sz);
  len += (size_t)conv;
  bufsize -= (size_t)conv;
  
  conv = sprint_timestampt(&buf[len], bufsize, &infop->arrival);
  len += (size_t)conv;
  bufsize -= (size_t)conv;
  
  conv = sprintf(&buf[len]," %7s %03u  %s",
		 s_feedtypet(infop->feedtype),
		 infop->seqno,
		 infop->ident);
  return buf;
}

int set_stdin(int vmin) {

  struct termios buf;
  
  if (tcgetattr(STDIN_FILENO, &save_termios) < 0)
    return(-1);
  
  buf = save_termios;
  
  /* echo off, canonical mode off, extended input processing off, signal chars
     off */
  
  buf.c_lflag &= ~(ICANON | IEXTEN | ECHONL | ISIG);
  
  /* no SIGINT on BREAK, input parity check off, don't strip 8th bit on input,
     output flow control off */
  
  buf.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
  
  /* clear size bits, parity checking off */
  
  buf.c_cflag &= ~(CSIZE | PARENB);
  
  /* set 8 bits/char */
  
  buf.c_cflag |= CS8;
  
  /* 1 byte at a time and no timer */
  
  buf.c_cc[VMIN] = vmin;
  buf.c_cc[VTIME] = 0;
  
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &buf) < 0)
    return(-1);
  
  return(0);
}

int reset_stdin()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &save_termios) < 0) {
    return(-1);
  }
  
  return(0);
}

static int display_watch(const prod_info *infop, const void *datap,
			 void *xprod, size_t size_notused, void *notused) 
{
  unsigned int oldMask = setulogmask(0u);
  (void)setulogmask(oldMask | LOG_MASK(LOG_INFO));
  printf("%s\n", ldms_prod_info(NULL, 0, infop, ulogIsDebug()));
  (void)setulogmask(oldMask);
  return ENOERR;
}

void init_options(pqueue *pq, prod_class *clssp, product *prod,
		  pq_match *dir, char *pattern) 
{
  printf("DEBUG: inside init_options() function\n");

  timestampt tv;
  
  /* set the product class structure */
  clssp->psa.psa_val->feedtype = fdtype;
  printf("Feedtype set to %s\n", s_feedtypet(clssp->psa.psa_val->feedtype));
  
  strcpy(pattern,".*");
  clssp->psa.psa_val->pattern = pattern;
  regcomp(&clssp->psa.psa_val->rgx, clssp->psa.psa_val->pattern,
	  REG_EXTENDED|REG_NOSUB);
  printf("Pattern set to \".*\"\n");
  
  clssp->from = TS_ZERO;
  printf("From time set to %s\n", ctime(&clssp->from.tv_sec));
  
  clssp->to = TS_ENDT;
  printf("To time set to %s\n", ctime(&clssp->to.tv_sec));
  
  /* take care of the queue cursor */
  
  parse_time((char *)"NOW", &tv);
  pq_cset(pq, &tv);
  printf("Cursor set to %s\n", ctime(&tv.tv_sec));
  
  *dir = TV_GT;
  printf("Cursor direction set to TV_GT\n");
  
  /* set up the product information record */
  
  strcpy(orighost,(char *)ghostname());
  prod->info.origin = orighost;
  parse_time((char *)"NOW", &tv);
  prod->info.arrival = tv;
  prod->info.feedtype = EXP;
  prod->info.seqno = seqnum++;
  strcpy(prod_ident, "TEST PRODUCT");
  prod->info.ident = prod_ident;
  printf("info.origin: %s\n", prod->info.origin);
  printf("info.arrival: %s\n", ctime(&prod->info.arrival.tv_sec));
  printf("info.feedtype: EXP\n");
  printf("info.seqno: %d\n", prod->info.seqno);
  printf("info.ident: %s\n", prod->info.ident);
  
  /* allocate an MD% context */
  
  md5ctxp = new_MD5_CTX();
  if (md5ctxp == NULL) {
    printf("init_options: new_MD5_CTX failed\n");
    exit(6);
  }
  
} /*init_options*/


void watch_queue(pqueue *pq, prod_class  *clssp) 
{
  char        ch;
  int         status;
  int         keep_at_it = 1;
  timestampt  tvout;
  
  /* stuff for the select call */
  int         ifd = fileno(stdin);
  int         ready;
  int         width = (1 << ifd);
  fd_set      readfds;
  struct timeval      timeo;
  
  /* we need the cursor time of the end of the queue */
  
  clssp->from = TS_ZERO;
  clssp->to = TS_ENDT;
  printf("From time set to %s\n", ctime(&clssp->from.tv_sec));
  printf("To time set to %s\n", ctime(&clssp->to.tv_sec));
  pq_last(pq, clssp, &tvout);
  
  if (tty_flag) {
    printf("(Type ^D when finished)\n");
    if (set_stdin(0)) {
      printf("set_stdin: noncanonical mode set failed\n");
      return;
    }
  }
  
  while(keep_at_it) {
    FD_ZERO(&readfds);
    FD_SET(ifd, &readfds);
    timeo.tv_sec = 1;
    timeo.tv_usec = 0;
    ready = select(width, &readfds, 0, 0, &timeo);
    
    if (ready < 0) {
      if (errno == EINTR) {
	errno = 0;
	continue;
      }
      serror("select");
      exit(1);
    }
    
    if (ready > 0) {
      if  (FD_ISSET(ifd, &readfds)) {
	ch = getc(stdin);
	if (ch != '' && ch != '') {
	  while(1) {
	    status = pq_sequence(pq, TV_GT, clssp, display_watch, 0);
	    
	    if (status == 0)
	      continue;
	    else if (status == PQUEUE_END){
	      printf("End of queue\n");
	      break;
	    }
	    else {
	      printf("status: %d\n",status);
	      printf("pq_sequence: %s\n", strerror(status));
	      return;
	    }
	  }
	}
	else {
	  keep_at_it = 0;
	}
      }
    }
    else {
      while(1) {
	status = pq_sequence(pq, TV_GT, clssp, display_watch, 0);
	
	if (status == 0)
	  continue;
	else if (status == PQUEUE_END){
	  printf("End of queue\n");
	  break;
	}
	else {
	  printf("status: %d\n",status);
	  printf("pq_sequence: %s\n", strerror(status));
	  return;
	}
      }
    }
  }
  
  if (tty_flag) {
    if (reset_stdin()) printf("reset_stdin: failed\n");
  }
  
  return;
}

/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
