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

LDM utility used to read the LDM queue and write info files
for LDM product groups.

*/
/* ----------------------------------------------------------- */

#include "dev_queue.h"

// Program information
static const char version_string[] = "2.04";
static const char produced_by[] = "Douglas.Gaer@noaa.gov";

#if defined (_LDAD_)
static const char program_name[] = "LDAD DEV queue info write";
static const char project_name[] = "LDAD LDM Cluster Utility";
#elif defined (_LDAD64_)
static const char program_name[] = "LDAD 64-bit DEV queue info write";
static const char project_name[] = "LDAD 64-bit LDM Cluster Utility";
#else
static const char program_name[] = "DEV queue info write";
static const char project_name[] = "SRH LDM Cluster Utility";
#endif

static const char service_name[] = "dev_queue";

// Global constants
static const char ldm_null[] = "(null)";


#if defined (_LDAD_) || defined (_LDAD64_)
static const char default_queue_path[] = "/usr/local/ldm/data/ldm.pq";
static const char default_output_dir[] = "/usr/local/ldm/data/dev_queue";
static const char default_logfile_name[] = "/usr/local/ldm/logs/dev_queue.log";
#else
static const char default_queue_path[] = "/usr/local/ldm/var/queues/ldm.pq";
static const char default_output_dir[] = "/usr/local/ldm/data/dev_queue";
static const char default_logfile_name[] = "/usr/local/ldm/var/logs/dev_queue.log";
#endif


static const char bad_file_chars[] = { ' ', '~', '`', '!', '@', '#', '$', '%', '^', '&', '*', 
				       '(', ')', '-', '+', '=', '{', '}', '[', ']', '\"', '\'', ':', 
				       ';', '<', '>', ',', '|', '\\', '?', '/', '\0' };
static const char replacement_file_char = '_';
static const char replacement_file_digit = '0';

// Global variables
int seqnum = 0;
int tty_flag = 0; 
static struct termios save_termios;
static  MD5_CTX *md5ctxp = NULL;
feedtypet fdtype = ANY;
char orighost[HOSTNAMESIZE + 1];
char prod_ident[KEYSIZE+1];
static char ldm_tprintbuf[4096];
int cont_loop = 0;
int has_error = 0;
pqueue *pq;
int stop_proc = 0;
int error_flag = 0;
int write_txt_group_files = 0;
int write_xml_group_files = 1;

// Global program configuration
char output_dir[255];
char queue_path[255];
char logfile_name[255];
int enable_logging = 0;
int verbose = 0;
int debug = 0;
int debug_level = 1;

int main(int argc, char *argv[])
{
  int c;
  char sbuf[255];

  // NOTE: Linux will kill a process that attempts to write to a pipe 
  // with no readers. This application may send data to client socket 
  // that has disconnected. If we do not tell the kernel to explicitly 
  // ignore the broken pipe the kernel will kill this process.
  signal(SIGPIPE, SIG_IGN);

  // Handle UNIX termination signals
  signal(SIGINT, termination_handler);  // Ctrl-C or "kill INT pid"
  signal(SIGQUIT, termination_handler); // Ctrl-backslash
  signal(SIGTERM, termination_handler); // Kill command "kill pid"
  signal(SIGHUP, termination_handler);  // Hangup if pstty closes with process running
  signal(SIGKILL, termination_handler); // Used by kill -9
  signal(SIGTSTP, termination_handler); // Ctrl-Z, resume with fg command
  signal(SIGABRT, termination_handler); // Ctrl-backslash
    
  // Allow sockets to close before any core dumps    
  signal(SIGBUS, termination_handler);  // Misaligned memory access
  signal(SIGSEGV, termination_handler); // Illegal memory access


  memset(output_dir, 0, sizeof(output_dir));
  memset(queue_path, 0, sizeof(queue_path));
  memset(logfile_name, 0, sizeof(logfile_name));
  memset(sbuf, 0, sizeof(sbuf));

  /* Set our default paths and file names */
  strncpy(output_dir, default_output_dir, (sizeof(output_dir)-1));
  strncpy(queue_path, default_queue_path, (sizeof(queue_path)-1));
  strncpy(logfile_name, default_logfile_name, (sizeof(logfile_name)-1));

  if(argc > 1 ) {
    opterr = 0;
    while ((c = getopt(argc, argv, "vd::hl::q:o:")) != -1) {
      switch (c) 
	{
	  case 'v':
	    verbose = 1;
	    break;
	  case 'd':
	    debug = 1;
	    if(optarg != NULL) {
	      debug_level = atoi(optarg);
	    }
	    break;
	  case 'l':
	    enable_logging = 1;
	    if(optarg != NULL) {
	      strncpy(logfile_name, optarg, (sizeof(logfile_name)-1));
	    }
	    break;
	  case 'q':
	    if(optarg == NULL) {
	      fprintf(stderr, "ERROR: A queue path is required with the -q option\n");
	      return 1;
	    }
	    strncpy(queue_path, optarg, (sizeof(queue_path)-1));
	    break;
	  case 'o':
	    if(optarg == NULL) {
	      fprintf(stderr, "ERROR: An ouput DIR is required with the -o option\n");
	      return 1;
	    }
	    strncpy(output_dir, optarg, (sizeof(output_dir)-1));
	    break;
	  case 'h':
	    print_help();
	    return 1;
	  case 't':
	    write_txt_group_files = 1;
	    return 1;
	  default:
	    break;
	}
    }
  }
    
  print_version();
  print_message("Starting", NULL, NULL);
  if(verbose) print_message("verbose mode enabled", NULL, NULL);
  if(debug) {
    print_message("debug mode enabled", NULL, NULL);
    sprintf(sbuf, "debug level %d", debug_level);
    print_message(sbuf, NULL, NULL);
  }
  if(enable_logging) {
    print_message("log file enabled", NULL, NULL);
    print_message("logging to", logfile_name, NULL);
  }

  if(!file_utils_exists((const char *)queue_path)) {
    print_message("ERROR: Cannot find LDM queue", queue_path, NULL);
    return 1;
  }
  
  if(file_utils_mkdir((const char *)output_dir) != 0) {
    print_message("ERROR: Cannot make output DIR", output_dir, NULL);
    return 1;
  }

  write_ldm_queue((char *)queue_path);

  return error_flag;
}

// Include source code files
#include "ldm_functions.c"
#include "file_utils.c"
#include "dev_functions.c"
#include "m_help.c"
#include "m_proc.c"
#include "m_config.c"
#include "m_log.c"

/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/

