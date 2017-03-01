/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_monitor.cpp
* �ļ���ʶ��
* ����ժҪ����������Ķ����ļ�
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
#include <typeinfo>
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "vna_switch_module.h"
#include "vnet_vnic.h"
#include "vna_vnic_pool.h"


namespace zte_tecs
{

/************************************************************
* �������ƣ� VisitDev
* ���������� ��������豸MTU�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CDevMonMTU::VisitDev(CNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TDevMonFuncReg *pMonFuncReg = GetMonMTUFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};

/************************************************************
* �������ƣ� VisitDev
* ���������� ��������豸�������Ժ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CPortMonBridge::VisitPort(CPortNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TPortMonFuncReg *pMonFuncReg = GetMonBridgeFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};  

/************************************************************
* �������ƣ� VisitBridge
* ���������� ������ŵ����ж˿ں���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CPortMonLoop::VisitPort(CPortNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TPortMonFuncReg *pMonFuncReg = GetMonLoopFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};

/************************************************************
* �������ƣ� VisitBridge
* ���������� ������ŵ����ж˿ں���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CBridgeMonUplink::VisitBridge(CSwitchDev *dev_entity)
{ 
    int32 nSize = 0;

    TDVSMonFuncReg *pMonFuncReg = GetMonUpLinkFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};  

void CVnicVisitor::VisitPort(CVnicPortDev *dev_entity)
{
    //cout << "enter CVnicVisitor::VisitPort" <<endl;

    int32 nSize = 0;

    TVnicMonFuncReg *pMonFuncReg = GetMonVnicFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};


}


