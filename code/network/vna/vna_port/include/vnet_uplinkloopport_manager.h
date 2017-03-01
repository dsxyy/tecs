/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_uplinkloopport_manager.h
* �ļ���ʶ��
* ����ժҪ��
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��3��7��
*******************************************************************************/
#ifndef VNET_UPLINKLOOPPORT_MANAGER_H
#define VNET_UPLINKLOOPPORT_MANAGER_H
#include "vnet_uplinkloopport_class.h"
#include "vnet_portinfo.h"

using namespace _GLIBCXX_STD;

namespace zte_tecs
{

class CUplinkLoopPortManager
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/

public:
    static CUplinkLoopPortManager *GetInstance();

    virtual ~CUplinkLoopPortManager();

    int32 GetUplinkLoopPortInfo(vector <CUplinkLoopPortReport> &phyportInfo);

    void LookupUplinkLoopPort();

    void GetPortWorkPara();

    void DbgShow();

    /*******************************************************************************
    * 2. protected section
    *******************************************************************************/
protected:

    /*******************************************************************************
    * 3. private section
    *******************************************************************************/
private:

    map<string, CUplinkLoopPortClass*> m_mapPorts;
    static CUplinkLoopPortManager *m_instance;
    Mutex m_mutex;

    CUplinkLoopPortManager()
    {
        m_mutex.Init();
    }

    DISALLOW_COPY_AND_ASSIGN(CUplinkLoopPortManager);
};
} /* end namespace zte_tecs */

#endif // VNET_UPLINKLOOPPORT_MANAGER_H
