/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mon_reg_func.h
* 文件标识：
* 内容摘要：dev监控相关处理函数的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#ifndef DEV_MON_REG_FUNC_INCLUDE_H__
#define DEV_MON_REG_FUNC_INCLUDE_H__

namespace zte_tecs
{
//监控任务时间间隔
#define DEV_MON_DEV_INTERVAL            60
#define DEV_MON_BRIDGE_INTERVAL         60
#define DEV_MON_PORT_INTERVAL           60
#define DEV_MON_VNIC_INIT_INTERVAL      5       //VNIC网口刚开始监控间隔
#define DEV_MON_VNIC_NOMAL_INTERVAL     60      //VNIC网口属性设置成功后时间间隔

#define TYPE_INFO(A) &(typeid(A))

typedef struct tagDevMonFuncReg
{
    const type_info *m_TypeInfo;
    void (*m_FuncPort)(CNetDev *);
}TDevMonFuncReg;

typedef struct tagPortMonFuncReg
{
    const type_info *m_TypeInfo;
    void (*m_FuncPort)(CPortNetDev *);
}TPortMonFuncReg;

typedef struct tagDVSMonFuncReg
{
    const type_info *m_TypeInfo;
    void (*m_FuncPort)(CSwitchDev *);
}TDVSMonFuncReg;

typedef struct tagVnicMonFuncReg
{
    const type_info *m_TypeInfo;
    void (*m_FuncPort)(CVnicPortDev *);
}TVnicMonFuncReg;


/*获取监控处理函数指针*/
TDevMonFuncReg *GetMonMTUFuncReg(int32 &nSize);         //获取监控网络设备MTU属性处理函数
TPortMonFuncReg *GetMonLoopFuncReg(int32 &nSize);         //获取监控网口的Loop属性处理函数
TPortMonFuncReg *GetMonBridgeFuncReg(int32 &nSize);      //获取监控网口的所挂接网桥属性处理函数
TDVSMonFuncReg *GetMonUpLinkFuncReg(int32 &nSize);     //获取监控网桥上行端口属性处理函数
TVnicMonFuncReg *GetMonVnicFuncReg(int32 &nSize);

/*定义监控处理函数*/
void CMonitorMTUPhyPort(CNetDev *dev_entity);
void CMonitorMTUBondPort(CNetDev *dev_entity);
void CMonitorMTUVnicPort(CNetDev *dev_entity);
void CMonitorMTUSDVS(CNetDev *dev_entity);
void CMonitorMTUEDVS(CNetDev *dev_entity);
void CMonitorLoopPhyPort(CPortNetDev *dev_entity);
void CMonitorLoopBondPort(CPortNetDev *dev_entity);
void CMonitorBridgePhyPort(CPortNetDev *dev_entity);
void CMonitorBridgeBondPort(CPortNetDev *dev_entity);
void CMonitorBridgeVnicPort(CPortNetDev *dev_entity);
void CMonitorUpLinkSDVS(CSwitchDev *dev_entity);
void CMonitorUpLinkEDVS(CSwitchDev *dev_entity);
void CMonitorVnic(CVnicPortDev *dev_entity);

//功能函数
int32 CMonitorEmbedVnic(CVnicPortDev *dev_entity);
int32 CMonitorSoftVnic(CVnicPortDev *dev_entity);
void CheckVepaFlowOfPort(CVnicPortDev *dev_entity,int32 op_flag);
int32 CheckVlanOfPCI(string strPCIOrder, CVnicPortDev *dev_entity);
int32 CheckMacOfPCI(string strPCIOrder, string strMACAdress);
int32 CheckBrOfPort(string strPortName, string strBridgeName, int32 nVlan);
int32 CheckVlanOfPort(string strPortName, CVnicPortDev *dev_entity);
int32 CheckMtuOfPort(string strPortName, int32 nMTU);
int32 MonitorVnic(string strVnicIf, CVnicPortDev *dev_entity);

}
#endif // #ifndef DEV_MON_REG_FUNC_INCLUDE_H__

