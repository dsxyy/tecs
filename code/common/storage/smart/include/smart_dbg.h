/*
 * smart_dbg.h
 * ¥Ú”°øÿ÷∆ 
 *
 */

#ifndef SMART_DBG_H_
#define SMART_DBG_H_
#include <stdio.h>

extern bool  g_smart_dbg_prn;
extern void Smart_Log(const char *format, ...);
extern void Smart_Vlog(const char *fmt, va_list list);

#define dm_debug Smart_Log

#endif
