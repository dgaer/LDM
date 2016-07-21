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

DEV queue functions.

*/
/* ----------------------------------------------------------- */

int reset_globals(void)
{
  seqnum = 0;
  tty_flag = 0; 
  memset(ldm_tprintbuf, 0 , sizeof(ldm_tprintbuf));
  memset(&save_termios, 0, sizeof(save_termios));
  memset(orighost, 0, sizeof(orighost));
  memset(prod_ident, 0, sizeof(prod_ident));
  fdtype = ANY;
  cont_loop = 0;
  has_error = 0;
  pq = NULL;
  return 1;
}

int write_ldm_queue(char *ldm_queue_path)
{
  int aflags = PQ_DEFAULT;
  int status = ENOERR;
#ifdef _LDAD_
  prod_class clss;
#else
  prod_class_t clss;
#endif
  
  product prodrec;
  pq_match cdir = (pq_match)0;
  char pattern[128];
  prod_spec spec;
  timestampt  tvout;
  prod_class  *clssp;
  char sbuf[255];

  int ready = 0;
  fd_set readfds;
  struct timeval timeo;
  int pq_fd = 0;

  memset(sbuf, 0, sizeof(sbuf));
  
  pq_fd = open(ldm_queue_path, O_RDONLY, 0444);
  if(pq_fd < 0) {
    print_message("ERROR - Cannot open queue file", ldm_queue_path, NULL);
    strerror_r(errno, sbuf, sizeof(sbuf));
    print_message("ERROR -", sbuf, NULL);
    return 0;
  }
  
  memset(&clss,  0, sizeof(clss));
  memset(&prodrec, 0, sizeof(prodrec));
  memset(&cdir, 0, sizeof(cdir));
  memset(pattern, 0, sizeof(pattern));
  memset(&spec, 0, sizeof(spec));
  memset(&sbuf, 0, sizeof(sbuf));

  reset_globals();
  putenv((char *)"TZ=GMT");

  clss.psa.psa_val = &spec;
  clss.psa.psa_len = 1;

  print_message("Starting dev queue watch function", NULL, NULL);
  print_message("Setting LDM queue ldm_queue_path to", ldm_queue_path, NULL);
  
  status = pq_open(ldm_queue_path, aflags, &pq);

  if(debug) {
    sprintf(sbuf, "DEBUG: pq_open() status = %d", status);
    print_message(sbuf, NULL, NULL);
    sprintf(sbuf, "DEBUG: ENOERR = %d", ENOERR);
    print_message(sbuf, NULL, NULL);
  }
  
  if (status != ENOERR) {
    if (PQ_CORRUPT == status) {
      print_message("The product-queue is inconsistent", ldm_queue_path, NULL);
    }
    else {
      print_message("pq_open:", ldm_queue_path, strerror(status));
    }

    if(debug) {
      sprintf(sbuf, "DEBUG: Exit failure = %d", EXIT_FAILURE); 
      print_message(sbuf, NULL, NULL);
    }
    
    error_flag = EXIT_FAILURE;
    return 0;
  }
  if(debug) print_message("DEBUG: pq_open success:", ldm_queue_path, NULL);

  if(debug) print_message("DEBUG: init_dev_queue_options() call", NULL, NULL);
  if(!init_dev_queue_options(pq, &clss, &prodrec, &cdir, pattern)) return 0;

  clssp = &clss;
  clssp->from = TS_ZERO;
  clssp->to = TS_ENDT;
  if(debug) {
    print_message("DEBUG: From time set to", ctime(&clssp->from.tv_sec), NULL);
    print_message("DEBUG: To time set to", ctime(&clssp->to.tv_sec), NULL);
  }
  pq_last(pq, clssp, &tvout);

  cont_loop = 1;
  
  while(cont_loop) {
    FD_ZERO(&readfds);
    FD_SET(pq_fd, &readfds);
    timeo.tv_sec = 1;
    timeo.tv_usec = 0;
    ready = select(pq_fd, &readfds, 0, 0, &timeo);
    
    // Select code for I/O wait to avoid using 100% of CPU in while loop
    if (ready < 0) {
      if (errno == EINTR) {
	errno = 0;
	continue;
      }
      print_message("ERROR - Select timeout error", NULL, NULL);
      return 0;
    }
 
    // Read the LDM queue
    status = pq_sequence(pq, TV_GT, clssp, write_watch_files, 0);
    
    if (status == 0) {
      continue;
    }
    else if (status == PQUEUE_END){
      if((debug == 1) && (debug_level >= 3)) print_message("DEBUG: End of queue", NULL, NULL);
      continue;
    }
    else {
      sprintf(sbuf, "ERROR - Error processing ldm queue: %d",status);
      print_message(sbuf, NULL, NULL);
      print_message("ERROR - pq_sequence error:", strerror(status), NULL);
      has_error = 1;
      break;
    }
  }

  if(stop_proc == 0) {
    if(pq != NULL ){
      print_message("DEBUG: Closing LDM queue in write_ldm_queue() function", NULL, NULL);
      pq_close(pq);
      pq = NULL;
    }
  }

  return (has_error == 1) ? 1 : 0;
}


void reset_ldm_product_info(LDM_product_info *pinfo)
{
  if(pinfo == NULL) return;
  memset(pinfo->sig, 0 , sizeof(pinfo->sig));
  memset(pinfo->org, 0 , sizeof(pinfo->org));
  memset(pinfo->id, 0 , sizeof(pinfo->id));
  memset(pinfo->feedtype, 0, sizeof(pinfo->feedtype));
  memset(pinfo->str_time, 0 , sizeof(pinfo->str_time));
  memset(pinfo->output_file_name, 0 , sizeof(pinfo->output_file_name));
  pinfo->epoch_time = (time_t)0;
  pinfo->seqnum = 0;
  pinfo->size = 0;
}

int write_group_file(LDM_product_info *pinfo)
{
  char fname[4096];
  char sbuf[255];
  char subdir[255];
  unsigned bytes = 0;
  unsigned i;
  FILE *outfile;
  static const char group_file_ext[] = "_group_file.txt";
  
  memset(fname, 0, sizeof(fname));
  memset(sbuf, 0, sizeof(sbuf));
  memset(subdir, 0, sizeof(subdir));

  strcpy(fname, output_dir);
  strcat(fname, "/");

  /* Set the feedtype subdirectory */
  strncpy(subdir, pinfo->feedtype, (sizeof(subdir)-1));
  for(i = 0; i < strlen(subdir); i++) {
    if(subdir[i] == '|') subdir[i] = '_';
  }
  strcat(fname, subdir);
  strcat(fname, "/");

  /* Check to ensure the DIR exists */
  if(file_utils_mkdir((const char *)fname) != 0) {
    print_message("ERROR: Cannot make output DIR", fname, NULL);
    return 0;
  }

  /* Set the file name and . extenstion */
  strcat(fname, pinfo->output_file_name);
  strcat(fname, group_file_ext);
  
  outfile = fopen(fname, "w+b");
  if(!outfile) {
    print_message("ERROR - Could not create group file", fname, NULL);
    return 0;
  }

  sprintf(sbuf, "EpochTime:%d\n", (int)pinfo->epoch_time);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  sprintf(sbuf, "TimeString:%s\n", pinfo->str_time);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "Signature:%s\n", pinfo->sig);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "Origin:%s\n", pinfo->org);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "Feedtype:%s\n", pinfo->feedtype);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "SeqNumber:%d\n", pinfo->seqnum);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "Size:%u\n", (unsigned)pinfo->size);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }

  sprintf(sbuf, "ID:%s\n", pinfo->id);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }

  sprintf(sbuf, "GroupKey:%s\n", pinfo->output_file_name);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }

  if(fclose(outfile) != 0) {
    print_message("ERROR - Could not close output file", fname, NULL);
    return 0;
  }

  return 1;
}

static int write_watch_files(const prod_info *infop, const void *datap,
			     void *xprod, size_t size_notused, void *notused) 
{
  unsigned str_len, array_len, i, j;
  unsigned int oldMask = setulogmask(0u);

  (void)setulogmask(oldMask | LOG_MASK(LOG_INFO));
  
  if(verbose){
    printf("%s\n", ldms_prod_info(NULL, 0, infop, ulogIsDebug()));
    fflush(stdout);
  }
  
  LDM_product_info pinfo;
  reset_ldm_product_info(&pinfo);

  pinfo.epoch_time = infop->arrival.tv_sec;
  sprint_timestampt(&pinfo.str_time[0], (sizeof(pinfo.str_time)-1), &infop->arrival);
  sprint_signaturet(&pinfo.sig[0], (sizeof(pinfo.sig)-1), infop->signature);
  if(infop->origin != NULL) {
    sprintf(&pinfo.org[0], "%s", infop->origin);
  }
  sprint_feedtypet(&pinfo.feedtype[0], (sizeof(pinfo.feedtype)-1), infop->feedtype); 
  pinfo.seqnum = infop->seqno;
  sprintf(&pinfo.id[0], "%s", infop->ident);
  pinfo.size = infop->sz;

  strncpy(pinfo.output_file_name, pinfo.id, (sizeof(pinfo.output_file_name)-1));
  str_len = strlen(pinfo.output_file_name);
  array_len = strlen(bad_file_chars);
  for(i = 0; i < str_len; i++) {
    for(j = 0; j < array_len; j++) {
      if(pinfo.output_file_name[i] == bad_file_chars[j]) {
	pinfo.output_file_name[i] = replacement_file_char;
	break; 
      } 
    }
    if(isdigit(pinfo.output_file_name[i])) {
      pinfo.output_file_name[i] = replacement_file_digit;
    }
  }

  if(verbose) {
    printf("Epoch Time:  %d\n", (int)pinfo.epoch_time);
    printf("Time String: %s\n", pinfo.str_time);
    printf("Signature:   %s\n", pinfo.sig);
    printf("Origin:      %s\n", pinfo.org);
    printf("Feedtype:    %s\n", pinfo.feedtype);
    printf("Seq Number:  %d\n", pinfo.seqnum);
    printf("ID:          %s\n", pinfo.id);
    printf("Size:        %u\n", (unsigned)pinfo.size);
    printf("Group Key:   %s\n", pinfo.output_file_name);
    printf("\n");
    fflush(stdout);
  }

  if(write_txt_group_files) write_group_file(&pinfo);
  if(write_xml_group_files) write_group_xml_file(&pinfo);
  
  (void)setulogmask(oldMask);
  error_flag = ENOERR;

  return ENOERR;
}

int init_dev_queue_options(pqueue *pq, prod_class *clssp, product *prod,
			   pq_match *dir, char *pattern) 
{
  timestampt tv;
  char sbuf[255];

  if(debug) print_message("DEBUG: inside init_dev_queue_options() function", NULL, NULL);
  memset(sbuf, 0, sizeof(sbuf));

  /* set the product class structure */
  clssp->psa.psa_val->feedtype = fdtype;
  print_message("Feedtype set to", s_feedtypet(clssp->psa.psa_val->feedtype), NULL);
  
  strcpy(pattern,".*");
  clssp->psa.psa_val->pattern = pattern;
  regcomp(&clssp->psa.psa_val->rgx, clssp->psa.psa_val->pattern,
	  REG_EXTENDED|REG_NOSUB);
  print_message("Pattern set to", pattern, NULL);
  
  clssp->from = TS_ZERO;
  if(verbose) {
    print_message("From time set to", ctime(&clssp->from.tv_sec), NULL);
  }
  
  clssp->to = TS_ENDT;
  if(verbose) {
    print_message("To time set to", ctime(&clssp->to.tv_sec), NULL);
  }
  
  /* take care of the queue cursor */
  parse_time((char *)"NOW", &tv);
  pq_cset(pq, &tv);
  if(verbose) {
    print_message("Cursor set to", ctime(&tv.tv_sec), NULL);
  }
  *dir = TV_GT;
  if(verbose) {
    print_message("Cursor direction set to TV_GT", NULL, NULL);
  }
  
  /* set up the product information record */
  strcpy(orighost,(char *)ghostname());
  prod->info.origin = orighost;
  parse_time((char *)"NOW", &tv);
  prod->info.arrival = tv;
  prod->info.feedtype = EXP;
  prod->info.seqno = seqnum++;
  strcpy(prod_ident, "TEST PRODUCT");
  prod->info.ident = prod_ident;
  if(verbose) {
    print_message("info.origin:", prod->info.origin, NULL);
    print_message("info.arrival:", ctime(&prod->info.arrival.tv_sec), NULL);
    print_message("info.feedtype: EXP", NULL, NULL);
    sprintf(sbuf, "info.seqno: %d", prod->info.seqno);
    print_message(sbuf, NULL, NULL);
    print_message("info.ident:", prod->info.ident, NULL);
  }
  
  /* allocate an MD% context */
  md5ctxp = new_MD5_CTX();
  if (md5ctxp == NULL) {
    print_message("ERROR: init_dev_queue_options: new_MD5_CTX failed", NULL, NULL);
    error_flag = 6;
    return 0;
  }
  
  return 1;
}

int write_group_xml_file(LDM_product_info *pinfo)
{
  char fname[4096];
  char sbuf[255];
  char subdir[255];
  unsigned bytes = 0;
  unsigned i;
  FILE *outfile;
  static const char group_file_ext[] = "_group_file.xml";
  static const char xml_header[] = "<?xml version=\"1.0\"?>";
  static const char xml_open_tag[] = "<LDMproduct>";
  static const char xml_close_tag[] = "</LDMproduct>";
  
  memset(fname, 0, sizeof(fname));
  memset(sbuf, 0, sizeof(sbuf));
  memset(subdir, 0, sizeof(subdir));

  strcpy(fname, output_dir);
  strcat(fname, "/");

  /* Set the feedtype subdirectory */
  strncpy(subdir, pinfo->feedtype, (sizeof(subdir)-1));
  for(i = 0; i < strlen(subdir); i++) {
    if(subdir[i] == '|') subdir[i] = '_';
  }
  strcat(fname, subdir);
  strcat(fname, "/");

  /* Check to ensure the DIR exists */
  if(file_utils_mkdir((const char *)fname) != 0) {
    print_message("ERROR: Cannot make output DIR", fname, NULL);
    return 0;
  }

  /* Set the file name and . extenstion */
  strcat(fname, pinfo->output_file_name);
  strcat(fname, group_file_ext);
  
  outfile = fopen(fname, "w+b");
  if(!outfile) {
    print_message("ERROR - Could not create group XML file", fname, NULL);
    return 0;
  }

  sprintf(sbuf, "%s\n\n", xml_header);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }

  sprintf(sbuf, "%s\n", xml_open_tag);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "    <EpochTime>%d</EpochTime>\n", (int)pinfo->epoch_time);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  sprintf(sbuf, "    <TimeString>%s</TimeString>\n", pinfo->str_time);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <Signature>%s</Signature>\n", pinfo->sig);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <Origin>%s</Origin>\n", pinfo->org);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <Feedtype>%s</Feedtype>\n", pinfo->feedtype);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <SeqNumber>%d</SeqNumber>\n", pinfo->seqnum);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <Size>%u</Size>\n", (unsigned)pinfo->size);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }


  sprintf(sbuf, "     <ProductIDs>\n");
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  sprintf(sbuf, "       <ID>%s</ID>\n", pinfo->id);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  sprintf(sbuf, "     </ProductIDs>\n");
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <GroupKey>%s</GroupKey>\n", pinfo->output_file_name);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "     <ProductName />\n");
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }

  sprintf(sbuf, "     <ProductDesc />\n");
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  sprintf(sbuf, "%s\n", xml_close_tag);
  bytes = fwrite((const unsigned char *)sbuf, 1, strlen(sbuf), outfile);
  if(bytes != strlen(sbuf)) {
    print_message("ERROR - Could not write to output file", fname, NULL);
    fclose(outfile);
    return 0;
  }
  
  if(fclose(outfile) != 0) {
    print_message("ERROR - Could not close output file", fname, NULL);
    return 0;
  }
  
  return 1;
}
/* ----------------------------------------------------------- */
/************************************/
/************ End of File ***********/
/************************************/
