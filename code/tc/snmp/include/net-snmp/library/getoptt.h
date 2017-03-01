#ifndef _GETOPTT_H_
#define _GETOPTT_H_ 1

#ifdef __cplusplus
extern          "C" {
#endif

NETSNMP_IMPORT int   getoptt(int, char *const *, const char *);
NETSNMP_IMPORT char *optarg;
NETSNMP_IMPORT int   opttind, optterr, optopt, optreset;

#ifdef __cplusplus
}
#endif
#endif
