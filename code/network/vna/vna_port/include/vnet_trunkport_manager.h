/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_trunkport_manager.h
* �ļ���ʶ��
* ����ժҪ��
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��2��21��
*******************************************************************************/
#ifndef VNET_TRUNKPORT_MANAGER_H
#define VNET_TRUNKPORT_MANAGER_H
#include "vnet_portinfo.h"
#include "vnet_trunkport_class.h"
#include "sky.h"

using namespace std;
namespace zte_tecs {

class CTrunkPortManager:public CNetDevMgr 
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static CTrunkPortManager *GetInstance()
    {
        if (NULL == m_instance)
        {
            m_instance = new CTrunkPortManager();
        }
        return m_instance;
    }

    virtual ~CTrunkPortManager();

    int32 GetTrunkInfo(vector<CBondReport> &vTrunkInfo);
    void LookupAllTrunk();
	
    CTrunkPortClass *AddTrunk(const string &strTrunkName);
    int32 DelTrunk(const string &strTrunkName);
    CTrunkPortClass* GetTrunk(const string &strTrunkName);

    //�����ӿ�
    int32 CreateBond(const string &strBondName,const string &strMode, vector<string> &ifs);
    int32 DelBond(const string &strBondName);
    int32 SetBondMode(const string &strBondName, int32 nBondMode);
    int32 GetBondPara(const string strBondName,string& strMode,string& strMiimon,
                                     string& strLacprate, string& strMiistatus, vector<string>& vSlaves);
    int32 GetBondMember(const string &strBondName, int32 nBondMode, vector<string> &ifs);

    int32 GetPortMapInfo(map<string, int32> &mapPortInfo);	

    void DoMonitor(CNetDevVisitor *dev_visit);
    void DoMonitor(CPortVisitor *dev_visit);

    void DbgShow();

/*******************************************************************************
* 2. protected section
*******************************************************************************/

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    static CTrunkPortManager *m_instance;
    map<string, CTrunkPortClass*>  m_mapTrunks; 
    Mutex m_mutex;
    
    CTrunkPortManager():m_mapTrunks(){m_mutex.Init();}

    DISALLOW_COPY_AND_ASSIGN(CTrunkPortManager);
};
} /* end namespace zte_tecs */

#endif // VNET_TRUNKPORT_MANAGER_H
