/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：SavePrtclCfg.h
* 文件标识：
* 内容摘要：在线配置存盘
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20080307
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __CLI_XMLRPC_H__
#define __CLI_XMLRPC_H__

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

extern WORD32 tCliMsgBufMutexSem;

#define MAX_METHOD_NUM                      1000
#define MAX_METHOD_NAME_LEN            50
#define MAX_METHOD_PARAM_NUM         255
typedef struct tagMethodConfig
{
    DWORD dwCmdId;
    BOOLEAN bIsNo;
    CHAR    aucMethodName[MAX_METHOD_NAME_LEN];
}T_MethodConfig;

extern void DisposeXmlRpcCmd(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN LoadClientConfigInfo();
extern bool CliLoginAuth(TYPE_LINK_STATE *pLinkState);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __SAVE_PROTOCOL_CONFIG_H__*/



