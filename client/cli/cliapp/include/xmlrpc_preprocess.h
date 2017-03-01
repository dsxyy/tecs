/*
xmlrpc命令入参和出参预处理程序，用于入参、出参复杂的命令进行特殊处理
*/
#ifndef _XMLRPC_PREPROCESS_H_
#define _XMLRPC_PREPROCESS_H_

#include "pub_oam_cfg_event.h"
#include <xmlrpc-c/base.hpp>

bool ParamInPreProcess(MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist);
bool ParamOutPreProcess(DWORD dwCmdId, xmlrpc_c::value result, MSG_COMM_OAM *pCliMsg);
void LoadPreprocessFuncs();
	
extern BYTE g_aucParaDataBuf[1024];

#endif
