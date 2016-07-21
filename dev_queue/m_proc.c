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

Process functions.

*/
/* ----------------------------------------------------------- */

int StopProc() 
{
  print_message("Stopping process...", NULL, NULL);
  cont_loop = 0;
  stop_proc = 1;
  if(pq != NULL) {
    print_message("DEBUG: Closing LDM queue in StopProc() function", NULL, NULL);
    pq_close(pq);
    pq = NULL;
  }
  return 1;
}

void ExitProc()
{
  print_message("Exiting process...", NULL, NULL);
  exit(error_flag);
}

void termination_handler(int signum)
// UNIX signal hander used to handle process termination signals  
{
  sigset_t mask_set;
  sigset_t old_set;

  if(signum == SIGSEGV) {
    signal(SIGSEGV, termination_handler); // Reset the signal handler again
    sigfillset(&mask_set); // Make any further signals while in handler
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_message("Process received segmentation violation", NULL, NULL);
    num_seg_violations++;
    if(num_seg_violations > 3) {
      StopProc();
      ExitProc();
    }
    sigprocmask(SIG_SETMASK, &old_set, NULL); // Restore the old signal mask2
    return;
  }
  if(signum == SIGBUS) {
    signal(SIGBUS, termination_handler);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_message("Process received bus violation", NULL, NULL);
    num_seg_violations++;
    if(num_seg_violations > 3) StopProc();
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    ExitProc();
  }
  if(signum == SIGINT) {
    signal(SIGINT, SIG_IGN); // Log first and ignore all others
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_message("Process interrupted with Ctrl-C", NULL, NULL);
    StopProc();
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    ExitProc();
  }
  if(signum == SIGQUIT) {
    signal(SIGQUIT, SIG_IGN);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_lines("Process interrupted Ctrl-backslash",
		"No action taken", NULL);
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    return;
  }
  if(signum == SIGTERM) {
    signal(SIGTERM, termination_handler);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_message("Process terminated by kill command", NULL, NULL);
    StopProc();
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    ExitProc();
  }
  if(signum == SIGHUP) {
    signal(SIGHUP, SIG_IGN);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_lines("Process received hangup",
		"No action taken", NULL);
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    return;
  }
  if(signum == SIGKILL) {
    signal(SIGKILL, termination_handler);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_message("Process terminated by SIGKILL", NULL, NULL);
    StopProc();
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    ExitProc();
  }
  if(signum == SIGTSTP) {
    signal(SIGTSTP, SIG_IGN);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_lines("Process received suspend from Ctrl-Z",
		"No action taken", NULL);
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    return;
  }
  if(signum == SIGABRT) {
    signal(SIGABRT, SIG_IGN);
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set); 
    print_lines("Process received SIGABRT",
		"No action taken", NULL);
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    return;
  }
}

/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
