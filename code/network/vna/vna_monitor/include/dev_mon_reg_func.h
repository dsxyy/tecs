/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_mon_reg_func.h
* �ļ���ʶ��
* ����ժҪ��dev�����ش������Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#ifndef DEV_MON_REG_FUNC_INCLUDE_H__
#define DEV_MON_REG_FUNC_INCLUDE_H__

namespace zte_tecs
{
//�������ʱ����
#define DEV_MON_DEV_INTERVAL            60
#define DEV_MON_BRIDGE_INTERVAL         60
#define DEV_MON_PORT_INTERVAL           60
#define DEV_MON_VNIC_INIT_INTERVAL      5       //VNIC���ڸտ�ʼ��ؼ��
#define DEV_MON_VNIC_NOMAL_INTERVAL     60      //VNIC�����������óɹ���ʱ����

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


/*��ȡ��ش�����ָ��*/
TDevMonFuncReg *GetMonMTUFuncReg(int32 &nSize);         //��ȡ��������豸MTU���Դ�����
TPortMonFuncReg *GetMonLoopFuncReg(int32 &nSize);         //��ȡ������ڵ�Loop���Դ�����
TPortMonFuncReg *GetMonBridgeFuncReg(int32 &nSize);      //��ȡ������ڵ����ҽ��������Դ�����
TDVSMonFuncReg *GetMonUpLinkFuncReg(int32 &nSize);     //��ȡ����������ж˿����Դ�����
TVnicMonFuncReg *GetMonVnicFuncReg(int32 &nSize);

/*�����ش�����*/
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

//���ܺ���
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

