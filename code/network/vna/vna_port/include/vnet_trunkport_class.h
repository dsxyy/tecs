/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_trunkport_class.h
* �ļ���ʶ��
* ����ժҪ��TRUNK�˿��ඨ��ͷ�ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��2��19��
*******************************************************************************/
#ifndef  VNET_TRUNKPORT_CLASS_H
#define  VNET_TRUNKPORT_CLASS_H

#include "pub.h"
#include "errcode.h"
#include "phy_port_pub.h"
#include "sky.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {


/*
* TRUNK�˿ڹ����ʵ����
*/
class CTrunkPortClass:public CPortNetDev
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    const string  &GetTrunkName()
    {
        return m_strName;
    }

    const int32  GetTrunkMode()
    {
        return m_nMode;
    }

    int32 GetTrunkInfo(const string strTrunkName, CBondInfo &cInfo);

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

    void DbgShow();

    CTrunkPortClass() 
    {
        m_nMode = -1; 
        m_tLacp.aggregator_id = "";
        m_tLacp.partner_mac = "";
        m_tLacp.status = 0;
        m_tLacp.nic_fail.clear();
        m_tLacp.nic_suc.clear();
        m_tBackup.active_nic = "";
        m_tBackup.other_nic.clear();
    }

    virtual ~CTrunkPortClass() {};
/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:    

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    string m_strName;
    int32  m_nMode;
    
    T_BOND_LACP_INFO   m_tLacp;  // m_nMode = 4 ��Ч
    T_BOND_BACKUP_INFO m_tBackup;// m_nMode = 1 ��Ч

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(CTrunkPortClass);
};
} // namespace zte_tecs

#endif  /* VNET_TRUNKPORT_CLASS_H */

