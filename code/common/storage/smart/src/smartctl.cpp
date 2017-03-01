/*
 * smartctl.cpp
 *
 * Home page of code is: http://smartmontools.sourceforge.net
 *
 * Copyright (C) 2002-8 Bruce Allen <smartmontools-support@lists.sourceforge.net>
 * Copyright (C) 2000 Michael Cornwell <cornwell@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * (for example COPYING); if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This code was originally developed as a Senior Thesis by Michael Cornwell
 * at the Concurrent Systems Laboratory (now part of the Storage Systems
 * Research Center), Jack Baskin School of Engineering, University of
 * California, Santa Cruz. http://ssrc.soe.ucsc.edu/
 *
 */


#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif
#if defined(__FreeBSD_version) && (__FreeBSD_version < 500000)
#include <unistd.h>
#endif

#if defined(__QNXNTO__) 
#include <unistd.h>
#endif


#include "int64.h"
#include "atacmds.h"
#include "ataprint.h"
#include "extern.h"
#include "knowndrives.h"
#include "scsicmds.h"
#include "scsiprint.h"
#include "smartctl.h"
#include "utility.h"
#include "smart_dbg.h"

//add by kangzhenhua 2011.02.22
T_DmPdSmartInfo *g_tSmartInfo = NULL;

#ifdef NEED_SOLARIS_ATA_CODE
extern const char *os_solaris_ata_s_cvsid;
#endif
//extern const char *atacmdnames_c_cvsid, *atacmds_c_cvsid, *ataprint_c_cvsid, *knowndrives_c_cvsid, *os_XXXX_c_cvsid, *scsicmds_c_cvsid, *scsiprint_c_cvsid, *utility_c_cvsid;
//const char* smartctl_c_cvsid="$Id: smartctl.cpp,v 1.169 2008/03/04 22:09:47 ballen4705 Exp $"
//ATACMDS_H_CVSID ATAPRINT_H_CVSID CONFIG_H_CVSID EXTERN_H_CVSID INT64_H_CVSID KNOWNDRIVES_H_CVSID SCSICMDS_H_CVSID SCSIPRINT_H_CVSID SMARTCTL_H_CVSID UTILITY_H_CVSID;

// This is a block containing all the "control variables".  We declare
// this globally in this file, and externally in other files.
smartmonctrl *con=NULL;

// to hold onto exit code for atexit routine
extern int exitstatus;

// Track memory use
extern int64_t bytes;
#if 0
void printslogan(){
#ifdef HAVE_GET_OS_VERSION_STR
  const char * ver = get_os_version_str();
#else
  const char * ver = SMARTMONTOOLS_BUILD_HOST;
#endif
  pout("smartctl version %s [%s] Copyright (C) 2002-8 Bruce Allen\n", PACKAGE_VERSION, ver);
  pout("Home page is " PACKAGE_HOMEPAGE "\n\n");
  return;
}

void PrintOneCVS(const char *a_cvs_id){
  char out[CVSMAXLEN];
  printone(out,a_cvs_id);
  pout("%s",out);
  return;
}

void printcopy(){
  const char *configargs=strlen(SMARTMONTOOLS_CONFIGURE_ARGS)?SMARTMONTOOLS_CONFIGURE_ARGS:"[no arguments given]";

  pout("smartctl comes with ABSOLUTELY NO WARRANTY. This\n");
  pout("is free software, and you are welcome to redistribute it\n");
  pout("under the terms of the GNU General Public License Version 2.\n");
  pout("See http://www.gnu.org for further details.\n\n");
  pout("CVS version IDs of files used to build this code are:\n");
  PrintOneCVS(atacmdnames_c_cvsid);
  PrintOneCVS(atacmds_c_cvsid);
  PrintOneCVS(ataprint_c_cvsid);
  PrintOneCVS(knowndrives_c_cvsid);
  PrintOneCVS(os_XXXX_c_cvsid);
#ifdef NEED_SOLARIS_ATA_CODE
  PrintOneCVS(os_solaris_ata_s_cvsid);
#endif
  PrintOneCVS(scsicmds_c_cvsid);
  PrintOneCVS(scsiprint_c_cvsid);
  PrintOneCVS(smartctl_c_cvsid);
  PrintOneCVS(utility_c_cvsid);
  pout("\nsmartmontools release " PACKAGE_VERSION " dated " SMARTMONTOOLS_RELEASE_DATE " at " SMARTMONTOOLS_RELEASE_TIME "\n");
  pout("smartmontools build host: " SMARTMONTOOLS_BUILD_HOST "\n");
  pout("smartmontools build configured: " SMARTMONTOOLS_CONFIGURE_DATE "\n");
  pout("smartctl compile dated " __DATE__ " at "__TIME__ "\n");
  pout("smartmontools configure arguments: %s\n", configargs);
  return;
}

void UsageSummary(){
  pout("\nUse smartctl -h to get a usage summary\n\n");
  return;
}
#endif

/*  void prints help information for command syntax */
void Usage (void){
  printf("Usage: smartctl [options] device\n\n");
  printf("============================================ SHOW INFORMATION OPTIONS =====\n\n");
#ifdef HAVE_GETOPT_LONG
  printf(
"  -h, --help, --usage\n"
"         Display this help and exit\n\n"
"  -V, --version, --copyright, --license\n"
"         Print license, copyright, and version information and exit\n\n"
"  -i, --info                                                       \n"
"         Show identity information for device\n\n"
"  -a, --all                                                        \n"
"         Show all SMART information for device\n\n"
  );
#else
  printf(
"  -h        Display this help and exit\n"
"  -V        Print license, copyright, and version information\n"
"  -i        Show identity information for device\n"
"  -a        Show all SMART information for device\n\n"
  );
#endif
  printf("================================== SMARTCTL RUN-TIME BEHAVIOR OPTIONS =====\n\n");
#ifdef HAVE_GETOPT_LONG
  printf(
"  -q TYPE, --quietmode=TYPE                                           (ATA)\n"
"         Set smartctl quiet mode to one of: errorsonly, silent, noserial\n\n"
"  -d TYPE, --device=TYPE\n"
"         Specify device type to one of: ata, scsi, marvell, sat, 3ware,N\n\n"
"  -T TYPE, --tolerance=TYPE                                           (ATA)\n"
"         Tolerance: normal, conservative, permissive, verypermissive\n\n"
"  -b TYPE, --badsum=TYPE                                              (ATA)\n"
"         Set action on bad checksum to one of: warn, exit, ignore\n\n"
"  -r TYPE, --report=TYPE\n"
"         Report transactions (see man page)\n\n"
"  -n MODE, --nocheck=MODE                                             (ATA)\n"
"         No check if: never, sleep, standby, idle (see man page)\n\n"
  );
#else
  printf(
"  -q TYPE   Set smartctl quiet mode to one of: errorsonly, silent,    (ATA)\n"
"                                               noserial\n"
"  -d TYPE   Specify device type to one of: ata, scsi, 3ware,N\n"
"  -T TYPE   Tolerance: normal, conservative,permissive,verypermissive (ATA)\n"
"  -b TYPE   Set action on bad checksum to one of: warn, exit, ignore  (ATA)\n"
"  -r TYPE   Report transactions (see man page)\n"
"  -n MODE   No check if: never, sleep, standby, idle (see man page)   (ATA)\n\n"
  );
#endif
  printf("============================== DEVICE FEATURE ENABLE/DISABLE COMMANDS =====\n\n");
#ifdef HAVE_GETOPT_LONG
  printf(
"  -s VALUE, --smart=VALUE\n"
"        Enable/disable SMART on device (on/off)\n\n"
"  -o VALUE, --offlineauto=VALUE                                       (ATA)\n"
"        Enable/disable automatic offline testing on device (on/off)\n\n"
"  -S VALUE, --saveauto=VALUE                                          (ATA)\n"
"        Enable/disable Attribute autosave on device (on/off)\n\n"
  );
#else
  printf(
"  -s VALUE  Enable/disable SMART on device (on/off)\n"
"  -o VALUE  Enable/disable device automatic offline testing (on/off)  (ATA)\n"
"  -S VALUE  Enable/disable device Attribute autosave (on/off)         (ATA)\n\n"
  );
#endif
  printf("======================================= READ AND DISPLAY DATA OPTIONS =====\n\n");
#ifdef HAVE_GETOPT_LONG
  printf(
"  -H, --health\n"
"        Show device SMART health status\n\n"
"  -c, --capabilities                                                  (ATA)\n"
"        Show device SMART capabilities\n\n"
"  -A, --attributes                                                         \n"
"        Show device SMART vendor-specific Attributes and values\n\n"
"  -l TYPE, --log=TYPE\n"
"        Show device log. TYPE: error, selftest, selective, directory,\n"
"                               background, scttemp[sts,hist]\n\n"
"  -v N,OPTION , --vendorattribute=N,OPTION                            (ATA)\n"
"        Set display OPTION for vendor Attribute N (see man page)\n\n"
"  -F TYPE, --firmwarebug=TYPE                                         (ATA)\n"
"        Use firmware bug workaround: none, samsung, samsung2,\n"
"                                     samsung3, swapid\n\n"
"  -P TYPE, --presets=TYPE                                             (ATA)\n"
"        Drive-specific presets: use, ignore, show, showall\n\n"
  );
#else
  printf(
"  -H        Show device SMART health status\n"
"  -c        Show device SMART capabilities                             (ATA)\n"
"  -A        Show device SMART vendor-specific Attributes and values    (ATA)\n"
"  -l TYPE   Show device log. TYPE: error, selftest, selective, directory,\n"
"                                   background, scttemp[sts,hist]\n"
"  -v N,OPT  Set display OPTion for vendor Attribute N (see man page)   (ATA)\n"
"  -F TYPE   Use firmware bug workaround: none, samsung, samsung2,      (ATA)\n"
"                                         samsung3, swapid\n"
"  -P TYPE   Drive-specific presets: use, ignore, show, showall         (ATA)\n\n"
  );
#endif
  printf("============================================ DEVICE SELF-TEST OPTIONS =====\n\n");
#ifdef HAVE_GETOPT_LONG
  printf(
"  -t TEST, --test=TEST\n"
"        Run test. TEST: offline short long conveyance select,M-N\n"
"                        pending,N afterselect,[on|off] scttempint,N[,p]\n\n"
"  -C, --captive\n"
"        Do test in captive mode (along with -t)\n\n"
"  -X, --abort\n"
"        Abort any non-captive test on device\n\n"
);
#else
  printf(
"  -t TEST   Run test. TEST: offline short long conveyance select,M-N\n"
"                            pending,N afterselect,[on|off] scttempint,N[,p]\n"
"  -C        Do test in captive mode (along with -t)\n"
"  -X        Abort any non-captive test\n\n"
  );
#endif
  print_smartctl_examples();
  return;
}

/* Returns a pointer to a static string containing a formatted list of the valid
   arguments to the option opt or NULL on failure. Note 'v' case different */
const char *getvalidarglist(char opt) {
  switch (opt) {
  case 'q':
    return "errorsonly, silent, noserial";
  case 'd':
    return "ata, scsi, marvell, sat, 3ware,N, hpt,L/M/N cciss,N";
  case 'T':
    return "normal, conservative, permissive, verypermissive";
  case 'b':
    return "warn, exit, ignore";
  case 'r':
    return "ioctl[,N], ataioctl[,N], scsiioctl[,N]";
  case 's':
  case 'o':
  case 'S':
    return "on, off";
  case 'l':
    return "error, selftest, selective, directory, background, scttemp[sts|hist]";
  case 'P':
    return "use, ignore, show, showall";
  case 't':
    return "offline, short, long, conveyance, select,M-N, pending,N, afterselect,[on|off], scttempint,N[,p]";
  case 'F':
    return "none, samsung, samsung2, samsung3, swapid";
  case 'n':
    return "never, sleep, standby, idle";
  case 'v':
  default:
    return NULL;
  }
}

/* Prints the message "=======> VALID ARGUMENTS ARE: <LIST> \n", where
   <LIST> is the list of valid arguments for option opt. */
void printvalidarglistmessage(char opt) {
  char *s;
  
  if (opt=='v')
    s=create_vendor_attribute_arg_list();
  else
    s=(char *)getvalidarglist(opt);
  
  if (!s) {
    pout("Error whilst constructing argument list for option %c", opt);
    return;
  }
 
  if (opt=='v'){
    pout("=======> VALID ARGUMENTS ARE:\n\thelp\n%s\n<=======\n", s);
    free(s);
  }
  else {
  // getvalidarglist() might produce a multiline or single line string.  We
  // need to figure out which to get the formatting right.
    char separator = strchr(s, '\n') ? '\n' : ' ';
    pout("=======> VALID ARGUMENTS ARE:%c%s%c<=======\n", separator, (char *)s, separator);
  }

  return;
}

/* Takes command options and sets features to be run */ 
void ParseOpts (int argc, const char** argv, const char *device)
{
	char optchar = '\0';
	const char * optarg = NULL;	
 
  if(*(argv[1]+0) == '-')
  {
    optchar = *(argv[1]+1);
  }
  else
  {
    optchar = *(argv[1]+0);
  }
  
  optarg = argv[2];
  
  con->testcase = -1;
  
  switch(optchar)
  {
    /* add by kangzhenhua 2011.08.06 for power mode */
    #ifndef SMART_CTL_TOOL
    case 'p':
    {
        con->powermode = TRUE;
        break;
    }
    #endif
    /* end of power mode */
    case 'h':
    {
        dm_debug("can't offer now!\n"); 
        break;
    }
    case 'c':
    {
        con->generalsmartvalues = TRUE;
        break;
    }
    case 'i':
    {
        con->driveinfo = TRUE;
        break;
    }
    case 'H':
    {
        con->checksmart = TRUE;   
        break;
    }
    case 'A':
    {
        con->smartvendorattrib = TRUE;
        break;
    }
    case 'l':
    {
        if (!strcmp(optarg,"error")) 
        {
          con->smarterrorlog = TRUE;
        } 
        else if (!strcmp(optarg,"selftest")) 
        {
          con->smartselftestlog = TRUE;
        } 
        else if (!strcmp(optarg, "selective")) 
      	{
          con->selectivetestlog = TRUE;
        } 
        else if (!strcmp(optarg,"directory"))
      	{
          con->smartlogdirectory = TRUE;
        } 
        else if (!strcmp(optarg,"background"))
      	{
          con->smartbackgroundlog = TRUE;
        }
        else if (!strcmp(optarg,"scttemp"))
       	{
          con->scttempsts = con->scttemphist = TRUE;
        } 
        else if (!strcmp(optarg,"scttempsts")) 
      	{
          con->scttempsts = TRUE;
        } 
        else if (!strcmp(optarg,"scttemphist"))
      	{
          con->scttemphist = TRUE;
        } 
        else 
      	{
          dm_debug("bad arg for log capacity!!!\n");
        }
        break;
    }
    case 's':
    {
      if (!strcmp(optarg,"on")) 
      {
        con->smartenable  = TRUE;
        con->smartdisable = FALSE;
      } 
      else if (!strcmp(optarg,"off")) 
      {
        con->smartdisable = TRUE;
        con->smartenable  = FALSE;
      } 
      else 
      {
        dm_debug("bad arg for on/off SMART func!!!\n");
      }
        break;
    }
    case 'o':
    {
        if (!strcmp(optarg,"on")) 
        {
          con->smartautoofflineenable  = TRUE;
          con->smartautoofflinedisable = FALSE;
        } 
        else if (!strcmp(optarg,"off"))
        {
          con->smartautoofflinedisable = TRUE;
          con->smartautoofflineenable  = FALSE;
        } 
        else
        {
          dm_debug("bad arg for on/off offline SMART self test!!!\n");
        }
        break;
    }
    case 'S':
    {
        if (!strcmp(optarg,"on")) 
        {
          con->smartautosaveenable  = TRUE;
          con->smartautosavedisable = FALSE;
        } 
        else if (!strcmp(optarg,"off")) 
        {
          con->smartautosavedisable = TRUE;
          con->smartautosaveenable  = FALSE;
        } 
        else 
        {
          dm_debug("bad arg for on/off autosave SMART!!!\n");
        }
        break;
    }
    case 't':
    {
       con->checksmart = TRUE;
       if (!strcmp(optarg,"offline")) 
       {
         con->smartexeoffimmediate = TRUE;
         con->testcase             = OFFLINE_FULL_SCAN;
       } 
       else if (!strcmp(optarg,"short"))
       {
         con->smartshortselftest = TRUE;
         con->testcase           = SHORT_SELF_TEST;
       } 
       else if (!strcmp(optarg,"long")) 
       {
         con->smartextendselftest = TRUE;
         con->testcase            = EXTEND_SELF_TEST;
       } 
       else if (!strcmp(optarg,"conveyance"))
       {
         con->smartconveyanceselftest = TRUE;
         con->testcase            = CONVEYANCE_SELF_TEST;
       }
       else 
        {
          dm_debug("bad arg for SMART self test mode select!!!\n");
        }
        break;
    }
    case 'C':
    {
        
        break;
    }
    case 'X':
    {
        con->smartselftestabort = TRUE;
        con->testcase           = ABORT_SELF_TEST;
        break;
    }
    case 'a':
    {
      con->driveinfo          = TRUE;
      con->checksmart         = TRUE;
      con->generalsmartvalues = TRUE;
      con->smartvendorattrib  = TRUE;
      con->smarterrorlog      = TRUE;
      con->smartselftestlog   = TRUE;
      con->selectivetestlog   = TRUE;
      break;
    }
    default:
    {
        dm_debug("wrong arg for smartctl !!!\n"); 
        break;
    }
  }
      
	
  return;
}


// Printing function (controlled by global con->dont_print) 
// [From GLIBC Manual: Since the prototype doesn't specify types for
// optional arguments, in a call to a variadic function the default
// argument promotions are performed on the optional argument
// values. This means the objects of type char or short int (whether
// signed or not) are promoted to either int or unsigned int, as
// appropriate.]
void pout(const char *fmt, ...){
#ifdef SMART_CTL_TOOL
  va_list ap;
  
  // initialize variable argument list 
  va_start(ap,fmt);
  if (con->dont_print){
    va_end(ap);
    return;
  }

  // print out
  vprintf(fmt,ap);
  va_end(ap);
  fflush(stdout);
#else
  va_list ap;

  va_start(ap,fmt);
  Smart_Vlog(fmt,ap);
  va_end(ap);
#endif  

  return;
}

// This function is used by utility.cpp to report LOG_CRIT errors.
// The smartctl version prints to stdout instead of syslog().
void PrintOut(int priority, const char *fmt, ...) {
  va_list ap;

  // avoid warning message about unused variable from gcc -W: just
  // change value of local copy.
  priority=0;

  va_start(ap,fmt);
  //vprintf(fmt,ap);
  Smart_Vlog(fmt,ap);
  va_end(ap);
  return;
}


/* Main Program */
int smartctl_main (int argc, const char **argv, T_DmPdSmartInfo *ptSmartInfo)
{
  int fd,retval=0;
  const char *device = argv[argc-1];
  smartmonctrl control;
  const char *mode=NULL;
  
  // define control block for external functions
  con=&control;  
  memset(con,0,sizeof(*con));
  //add by kangzhenhua 2011.02.22
  #ifndef SMART_CTL_TOOL
  g_tSmartInfo = ptSmartInfo;  
  #endif

  // Part input arguments
  ParseOpts(argc, argv, device);

  device = argv[argc-1];
  //printf("device = %s\n", device);

  //con->controller_type = CONTROLLER_SCSI;
  con->controller_type = guess_device_type(device);

  // set up mode for open() call.  SCSI case is:
  switch (con->controller_type) {
  case CONTROLLER_SCSI:
  case CONTROLLER_SAT:
    mode="SCSI";
    break;
  case CONTROLLER_3WARE_9000_CHAR:
    mode="ATA_3WARE_9000";
    break;
  case CONTROLLER_3WARE_678K_CHAR:
    mode="ATA_3WARE_678K";
    break;
  case CONTROLLER_CCISS:
    mode="CCISS";
    break;
  default:
    mode="ATA";
    break;
  } 
  
  // printf("mode %s\n", mode);
  
  // open device - SCSI devices are opened (O_RDWR | O_NONBLOCK) so the
  // scsi generic device can be used (needs write permission for MODE 
  // SELECT command) plus O_NONBLOCK to stop open hanging if media not
  // present (e.g. with st).  Opening is retried O_RDONLY if read-only
  // media prevents opening O_RDWR (it cannot happen for scsi generic
  // devices, but it can for the others).
  fd = deviceopen(device, mode);
  if (fd<0)
  {
    pout("open device error!\n");
    return FAILDEV;
  }

   // now call appropriate ATA or SCSI routine
  switch (con->controller_type) {
  case CONTROLLER_UNKNOWN:
    // we should never fall into this branch!
    pout("Smartctl: please specify device type with the -d option.\n");
    retval = FAILCMD;
    break;
  case CONTROLLER_SCSI:
    retval = scsiPrintMain(fd);
    if ((0 == retval) && (CONTROLLER_SAT == con->controller_type))
        retval = ataPrintMain(fd);
    break;
  case CONTROLLER_CCISS:
    // route the cciss command through scsiPrintMain. 
    // cciss pass-throughs will separeate from the SCSI data-path.
    retval = scsiPrintMain(fd);
    break;
  default:
    retval = ataPrintMain(fd);
    break;
  }
     
  deviceclose(fd);

  return retval;
}
