/*********************************************************************
* 版权所有 (C)2006, 深圳市中兴通讯股份有限公司。
*
* 文件名称： xmlagent.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： 
* 作    者： 齐龙兆——3G平台
*创建日期: 2008/9/10
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef _XMLAGENT_H_
#define _XMLAGENT_H_


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

#define MAX_NODETEXT_BUF_SIZE (WORD16)255
#define MAX_NODEATTR_BUF_SIZE (WORD16)255
#define MAX_SIZE_OPJID     (WORD16)65535
#define MAX_COUNT_TRM  256
#define MAX_SIZE_HANDSHANK_COUNT (WORD16)3
typedef struct tagT_TYPE_OMMRegister_REQ
{
    WORD16 wComponentId;/*服务器组件号*/
    WORD16 wTRMNum;/*需注册终端编号个数*/
    WORD16 awTRMId[MAX_COUNT_TRM];/*向平台OAM注册的TRM数组*/
}OMMRegisterReq,OMMUnRegisterReq;/*网管终端注册或注销请求*/

typedef struct tagT_TYPE_OMMRegister_ACK
{
    WORD16 wComponentId;/*服务器组件号*/
    WORD16 wTRMNum;/*需注册终端编号个数*/
    WORD16 awTRMId[MAX_COUNT_TRM];/*向平台OAM注册的TRM数组*/
    WORD16 awOR[MAX_COUNT_TRM];/*操作结果*/
}OMMRegisterAck,OMMUnRegisterAck;

typedef struct tagT_TYPE_OMMHandShank_REQ
{
    WORD16 wComponentId;/*服务器组件号*/
}OMMHandShankReq;/*网管服务器与平台OAM握手请求*/

typedef struct tatT_TYPE_OMMHandShank_ACK
{
    WORD16 wComponentId;/*服务器组件号*/
    WORD16 wTRMNum;/*连接终端个数*/    
    WORD16 aucTRMId[MAX_COUNT_TRM];/*当前OMC连接TRM数组*/    
}OMMHandShankAck;/*网管服务器与平台OAM握手应答*/

extern void ReceiveFromOmm(LPVOID ptrMsgBuf);/*OMM发送消息到OAM*/
extern void SendErrStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrOutputResult);/*向网管发送错误XML字符串*/
extern void SendStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *sOutputResult,BOOLEAN bOR,BOOLEAN bEof);/*向网管发送XML字符串*/
extern void RecvFromProtocolToOMM(LPVOID ptrMsgBuf);/*OAM发送消息到OMM*/
extern void RecvFromOP(LPVOID pMsgPara);/*接收业务返回消息*/
extern void ReceiveRegisterMsgFromOmm(LPVOID ptrMsgBuf);/*网管端XML注册请求*/
extern void ReceiveUnRegisterMsgFromOmm(LPVOID ptrMsgBuf);/*网管端XML注销请求*/
extern void ReceiveHandShakeMsgFromOmm(LPVOID ptrMsgBuf);/*网管端XML握手请求*/
extern void ReceiveLinkCfgMsgFromOmm(LPVOID ptrMsgBuf);/*接收网管端握手请求*/
extern void ReceiveCeaseCmdMsgFromOmm(LPVOID ptrMsgBuf);/*接收网管命令终止请求*/

extern void XML_SetCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_KillCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_RecvPlanInfoFromApp(LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void XML_DealPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void SendPLATAckToOMM(MSG_COMM_OAM *ptOamMsgBuf);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __PARA_CNVT_H__ */


