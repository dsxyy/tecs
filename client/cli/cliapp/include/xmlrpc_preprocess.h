/*
xmlrpc������κͳ���Ԥ�������������Ρ����θ��ӵ�����������⴦��
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
