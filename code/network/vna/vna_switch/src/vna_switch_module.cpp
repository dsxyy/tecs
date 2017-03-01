/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_switch_module.cpp
* �ļ���ʶ��
* ����ժҪ��vna�Ͻ���ģ�����
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
******************************************************************************/
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_function.h"
#include "vnet_phyport_manager.h"
#include "vnet_vnic.h"
#include "vna_switch_module.h"
#include "vnet_evb_mgr.h"

namespace zte_tecs
{


CSwitchDev::CSwitchDev()
{
    m_nType     = 0;
    m_bState    = TRUE;
    m_nBondMode = 0;
    m_nEvbMode  = EN_EVB_MODE_NORMAL;
    m_nMaxVnics = 0;
}
CSwitchDev::~CSwitchDev()
{
    
}

/*******************************************************************************
* �������ƣ� IsVectorEqua
* ���������� �Ƚ�����vector�Ƿ���ȫƥ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSwitchDev::IsVectorEqua(vector<string> new_vec, vector<string> old_vec)
{
    if(new_vec.size() != old_vec.size())
    {
        return -1;
    }
    vector<string>::iterator it = new_vec.begin();
    for(; it != new_vec.end(); ++it)
    {
        if(find(old_vec.begin(),old_vec.end(),*it) == old_vec.end())
        {
            return -1;
        }
    }
    return 0;
}

/*******************************************************************************
* �������ƣ� IsPortJoinBridge
* ���������� �ж϶˿��Ƿ�����������ţ��������������ɾ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSwitchDev::IsPortJoinBridge(const string &strBr, const string &strPort)
{
    string strBridge;
    if(VNET_PLUGIN_TRUE == VNetIsPortInBridge(strPort, strBridge))
    {
        if(strBr != strBridge)
        {
            if(VNET_PLUGIN_SUCCESS != VNetDelNetIfOVS(strBridge, strPort))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::IsPortJoinBridge: VNetDelNetIfOVS <%s-%s> failed.\n", 
                      strBridge.c_str(), strPort.c_str());
                return -1;
            }
        }   
    }   
    return 0;
}

/*******************************************************************************
* �������ƣ� IsPortJoinBond
* ���������� �ж϶˿��Ƿ����bond���������bond��ɾ��bond
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSwitchDev::IsPortJoinBond(const string &strPort)
{
    string strBond;
    if(VNET_PLUGIN_TRUE == VNetIsPortInBondOVS(strPort, strBond))
    {
        if(VNET_PLUGIN_SUCCESS != VNetDelBondOVS(strBond))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::IsPortJoinBond: call VNetDelBondOVS delete <%s> failed.\n", strBond.c_str());
            return -1;
        }
    }  
    return 0;
}

/*******************************************************************************
* �������ƣ� IsDvsConsistent
* ���������� ������ú�ʵ����Ч��DVS�Ƿ�һ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSwitchDev::IsDvsConsistent(const string &strBridge, const string &strBond, 
            const string &strBondMode, const vector<string> &vecBondMem)
{
    string strTmpBridge;
    if(VNET_PLUGIN_SUCCESS != VNetGetBridgeOfIf(strBond, strTmpBridge))
    {
        VNET_LOG(VNET_LOG_INFO, "CSwitchDev::IsDvsConsistent: VNetGetBridgeOfIf [%s] failed.\n", strBond.c_str());
        return -1;
    } 
    //���Ų�һ��
    if(strBridge != strTmpBridge)
    {
        VNET_LOG(VNET_LOG_INFO, "CSwitchDev::IsDvsConsistent: Bridge<%s> isn't consistent with <%s>.\n", 
                   strBridge.c_str(), strTmpBridge.c_str());
        return -1;    
    }
    else
    {
        string strTmpMode;
        vector<string> vecTmpSlave;
        VNetGetBondSlavesOVS(strBond, vecTmpSlave);
        VNetGetBondModeOVS(strBond, strTmpMode);
        //Bond��һ��
        if((IsVectorEqua(vecBondMem, vecTmpSlave)) || (strBondMode != strTmpMode))
        {
            VNET_LOG(VNET_LOG_INFO, "CSwitchDev::IsDvsConsistent: Bond<%s> isn't consistent.\n", strBond.c_str());
            return -1;
        }
    }
    return 0;
}

int32 CSwitchDev::IsVtepPortConsistent(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    if((GetDVSVtepPort() == cSwitchCfg.m_strVtepPort) &&
        (GetDVSVtepPortIP() == cSwitchCfg.m_strVtepIp) &&
        (GetDVSVtepPortMask() == cSwitchCfg.m_strVtepMask))
    {
        return 0;
    }
    return -1;
}
    
/*******************************************************************************
* �������ƣ� CreateBonding
* ���������� ����bond������������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/                  
int32 CSwitchDev::CreateBonding(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    string strBondName = cSwitchCfg.m_strBondName;
    string strBondMode = to_string<int32>(cSwitchCfg.m_nBondMode,dec);
        
    vector<string> vecBond;
    VNetGetAllBondingsOVS(vecBond);
    
    //����Ϊ�գ���Ҫ����һ������
    if(strBondName.empty()) 
    {
        uint32 dwBondNum = 2; //��ʱ��bond2��ʼ����
        while(1)
        {
            strBondName = "bond" + to_string<uint32>(dwBondNum++, dec);
            if(find(vecBond.begin(), vecBond.end(), strBondName) == vecBond.end())
            {
                break;
            }
        }
        cSwitchCfg.m_strBondName = strBondName;
    }
    
    if(find(vecBond.begin(), vecBond.end(), strBondName) == vecBond.end()) 
    {
        //ȫ�´���bond�������뵽������
        if(VNET_PLUGIN_SUCCESS != VNetAddBondOVS(cSwitchCfg.m_strName, strBondName, strBondMode, cSwitchCfg.m_vecPort))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CreateBonding: call VNetAddBond create [%s] failed.\n", strBondName.c_str());
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Create ovs %s failed.", strBondName.c_str());
            return -1;
        }              
    }
    else
    {
        //������ǰ��ɾ��ͬ������
        if(0 != DeleteBridge(cSwitchCfg.m_strName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CreateBonding: call DeleteBridge return fail.\n");
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete the explict bridge(%s) failed.", cSwitchCfg.m_strName.c_str());
            return -1;
        }
        //�Ѿ�������һ����bond�������ǻ���bond
        if(VNET_PLUGIN_SUCCESS != VNetRenameDVS(cSwitchCfg.m_strMixedBridge, cSwitchCfg.m_strName, strBondName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CreateBonding: call VNetRenameDVS rename [%s-%s] failed.\n", 
                         cSwitchCfg.m_strMixedBridge.c_str(), cSwitchCfg.m_strName.c_str());
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Rename bridge from %s to %s for mixed bond failed.", 
                               cSwitchCfg.m_strMixedBridge.c_str(), 
                               cSwitchCfg.m_strName.c_str());
            return -1;
        }  
    }
    
    return 0;
}

/*******************************************************************************
* �������ƣ� CheckPortIsValid
* ���������� �˿���Ч�Լ�飬Ϊ����������׼��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/                  
int32 CSwitchDev::CheckPortIsValid(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    string strBridge   = cSwitchCfg.m_strName;
    string strBondName = cSwitchCfg.m_strBondName;
    string strBondMode = to_string<int32>(cSwitchCfg.m_nBondMode,dec);
    
    /* ��Щ�������Ҫ��?
       ����1����bond���˿����
       ����2��bond��bond��Ա��ģʽ��ƥ��
       �������ֻҪ�˿ڱ�ռ�ñ����ͷţ����߶Ͽ�������˿ڣ�����ɾ��bond
    */
    
    string strBrIf; //ʵ�������µ�uplink�˿�
    vector<string>::iterator itPort;
    if(1 == cSwitchCfg.m_vecPort.size())
    {
        //��������1���˿ں��Ŷ�ƥ�䣬�����ɹ�
        VNetGetPhyPortFromBrOVS(strBridge, strBrIf);
        if(!strBrIf.empty() && (strBrIf == cSwitchCfg.m_vecPort.front()))
        {
            VNET_LOG(VNET_LOG_INFO, "CSwitchDev::CheckPortIsValid: %s has existed, create success! \n", strBridge.c_str());
            cSwitchCfg.m_strSwIf = strBrIf;
            VNetFormatString(cSwitchCfg.m_strRetInfo, "%s has existed, create success.", strBridge.c_str());
            return ERROR_SWITCH_ALREADY_EXIST;
        }
    }
    else
    {
        if(strBondName.empty())
        {
            //���ڵ�һ�δ���bond���������˿ڻ��õ����(���ǻ���ͨ��ͬʱ�·�)
            for(itPort = cSwitchCfg.m_vecPort.begin(); itPort != cSwitchCfg.m_vecPort.end(); ++itPort)
            {
                string strTmpBond;
                if(VNET_PLUGIN_TRUE == VNetIsPortInBondOVS(*itPort, strTmpBond))
                {
                    if(0 == IsDvsConsistent(strBridge, strTmpBond, strBondMode, cSwitchCfg.m_vecPort))
                    {
                        //�����ú�ʵ����ȫһ��
                        cSwitchCfg.m_strBondName = strTmpBond;
                        cSwitchCfg.m_strSwIf     = strTmpBond;
                        cSwitchCfg.m_nSwIfType   = PORT_TYPE_BOND;
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "%s has existed.", strBridge.c_str());
                        return ERROR_SWITCH_ALREADY_EXIST;
                    }

                    if(!cSwitchCfg.m_strMixedBridge.empty() &&
                        (0 == IsDvsConsistent(cSwitchCfg.m_strMixedBridge, strTmpBond, strBondMode, cSwitchCfg.m_vecPort)))
                    {
                        //�������
                        cSwitchCfg.m_strBondName = strTmpBond;
                        cSwitchCfg.m_strSwIf     = strTmpBond;
                        cSwitchCfg.m_nSwIfType   = PORT_TYPE_BOND;
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "Switch port is mixed.");
                        return ERROR_SWITCH_PORT_ISMIXED;
                    }

                    //else ���ó�ͻ��ɾ��bond
                    if(VNET_PLUGIN_SUCCESS != VNetDelBondOVS(strTmpBond))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CheckPortIsValid: call <VNetDelBondOVS> delete explict bond(%s) failed.\n", strTmpBond.c_str());
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete conflict bond(%s) failed.", strTmpBond.c_str());
                        return ERROR_SWITCH_PORT_ISCONFLICT;  
                    } 
                }
            }   
        }
        else
        {
            //�ظ����������߶˿ڻ������
            vector<string> vecBond;
            VNetGetAllBondingsOVS(vecBond);
            vector<string>::iterator itBond = vecBond.begin();
            for(; itBond != vecBond.end(); ++itBond)
            {
                if(*itBond == strBondName)
                {   
                    if(0 == IsDvsConsistent(strBridge, strBondName, strBondMode, cSwitchCfg.m_vecPort))
                    {
                        //�����ú�����Ч��SDVS��ȫһ��
                        cSwitchCfg.m_strSwIf = strBondName;
                        cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND;
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "%s has existed[2].", strBridge.c_str());
                        return ERROR_SWITCH_ALREADY_EXIST;                   
                    }
                    if(!cSwitchCfg.m_strMixedBridge.empty() && 
                        (0 == IsDvsConsistent(cSwitchCfg.m_strMixedBridge, strBondName, strBondMode, cSwitchCfg.m_vecPort)))
                    {
                        //�����ú�����Ч��EDVS��ȫһ��
                        cSwitchCfg.m_strSwIf = strBondName;
                        cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND;
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "Switch port is mixed[2].");
                        return ERROR_SWITCH_PORT_ISMIXED;            
                    }
                    //����������������ɾ��bond
                    if(VNET_PLUGIN_SUCCESS != VNetDelBondOVS(*itBond))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CheckPortIsValid: delete explict bond(%s) failed.\n", (*itBond).c_str());
                        VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete conflict bond(%s) failed.", (*itBond).c_str());
                        return ERROR_SWITCH_PORT_ISCONFLICT;  
                    } 
                }
            }
            //��δ��������һ����bond�����ж˿ڼ�顣
        }
    }
    
    //�˿ڼ�������������bond��ֱ������
    for(itPort = cSwitchCfg.m_vecPort.begin(); itPort != cSwitchCfg.m_vecPort.end(); ++itPort)       
    {
        if(SOFT_VIR_SWITCH_TYPE == cSwitchCfg.m_nType)
        {
            if(IsPortJoinBridge(strBridge, *itPort))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CheckPortIsValid: call <IsPortJoinBridge> failed.\n");
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete conflict port(%s) from bridge failed.", (*itPort).c_str());
                return ERROR_SWITCH_PORT_ISCONFLICT;
            }
        }
        if(IsPortJoinBond(*itPort))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CheckPortIsValid: call <IsPortJoinBond> failed.\n");
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete conflict port(%s) from bond failed.", (*itPort).c_str());
            return ERROR_SWITCH_PORT_ISCONFLICT;
        }
    }
    
    //��������µķǷ��˿�
    strBrIf.clear();
    VNetGetPhyPortFromBrOVS(strBridge, strBrIf);
    if(!strBrIf.empty())
    {
        if(VNET_PLUGIN_SUCCESS != VNetDelNetIfOVS(strBridge, strBrIf))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CheckPortIsValid: call VNetDelNetIfOVS delete conflict <%s-%s> failed.\n", 
                      strBridge.c_str(), strBrIf.c_str());
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete conflict bridge(%s) port failed.", strBridge.c_str());
            return ERROR_SWITCH_PORT_ISCONFLICT;
        }
    }    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� AddBridgeNetIf
* ���������� ������Žӿ�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� (�������Žӿ�)
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/ 
int32 CSwitchDev::AddBridgeNetIf(const string &strBridge, const string &strBrIf)
{    
    string strInIf;
    VNetGetPhyPortFromBrOVS(strBridge, strInIf); 
    
    //������ͬ���пڣ����ظ�����
    if(strBrIf == strInIf)
    {
        return 0;
    }
    if(!strInIf.empty())
    {
        if(VNET_PLUGIN_SUCCESS != VNetDelNetIfOVS(strBridge, strInIf))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::DelBridgeNetIf: call VNetDelNetIfOVS delete <%s-%s> failed.\n", 
                      strBridge.c_str(), strInIf.c_str());
            return -1;
        }
    }
    //��ӳ�Ա��
    if(VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(strBridge, strBrIf))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::DelBridgeNetIf: call VNetAddNetIfOVS add <%s-%s> failed.\n", 
                  strBridge.c_str(), strBrIf.c_str());
        return -1;
    }        

    return 0;
}

/*******************************************************************************
* �������ƣ� CreateBridge
* ���������� ��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/                  
int32 CSwitchDev::CreateBridge(const string &strBridge, const string &strMixBridge)
{    
    vector<string> vecBridge;
    VNetGetAllBridges(vecBridge);
    vector<string>::iterator iter;
    iter = find(vecBridge.begin(), vecBridge.end(), strBridge);
    if(iter == vecBridge.end())
    {
        //���Ų����ڣ��´���һ���������жϻ��ã�vnm�·���ʱ�򶼻���ϻ����������ڽű��з�ֹ�ظ�����
        if(VNET_PLUGIN_SUCCESS != VNetCreateBridge(strBridge))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::CreateBridge: call VNetCreateBridge create <%s> failed.\n", strBridge.c_str());
            return -1;
        }
    }
    return 0;
}

/*******************************************************************************
* �������ƣ� DelBridge
* ���������� ɾ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� (����)
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/ 
int32 CSwitchDev::DeleteBridge(const string &strBridge)
{
    vector<string> vecBridge;
    VNetGetAllBridges(vecBridge);
    vector<string>::iterator iter = vecBridge.begin();
    for(; iter != vecBridge.end(); ++iter)
    {
        if((*iter) == strBridge)
        {
            //ɾ������
            if(VNET_PLUGIN_SUCCESS != VNetDeleteBridge(strBridge))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteBridge: call VNetDeleteBridge delete <%s> failed.\n", strBridge.c_str());
                return -1;
            }       
        }
    }
    return 0;
}

/*******************************************************************************
* �������ƣ� ProcEvbPort
* ���������� ��ӻ�ɾ��evb port
* ���ʵı� ��
* �޸ĵı� ��
* ��������� bFlag = TRUE;  ���   
*            bFlag = FALSE; ɾ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/ 
int32 CSwitchDev::ProcEvbPort(const string &strBridge, BOOL bFlag)
{
    CEvbMgr *pEvbMgr = CEvbMgr::GetInstance();
    if(NULL == pEvbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::ProcEvbPort: CEvbMgr::GetInstance() is NULL.\n");
        return -1;
    }   
    
    if(TRUE == bFlag)
    {
        //���ý������ԣ���ֹlldp���Ķ���
        if(VNET_PLUGIN_SUCCESS != VNetSetBrideForwordLLDP(strBridge))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::ProcEvbPort: call VNetSetBrideForwordLLDP set <%s> lldp flag failed.\n", strBridge.c_str());
            return -1;
        }  
        
        if(-1 != pEvbMgr->GetEvbIssPortNo(strBridge))
        {
            if(0 != pEvbMgr->UnsetEvbIssPort(strBridge))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::ProcEvbPort: call UnsetEvbIssPort first delete <%s> evbport failed.\n", strBridge.c_str());
                return -1;
            }
        }
        if(0 != pEvbMgr->SetEvbIssPort(strBridge))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::ProcEvbPort: call SetEvbIssPort create <%s> evbport failed.\n", strBridge.c_str());
            return -1;
        }
    }
    else
    {
        if(0 != pEvbMgr->UnsetEvbIssPort(strBridge))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::ProcEvbPort: call UnsetEvbIssPort delete <%s> evbport failed.\n", strBridge.c_str());
            return -1;
        }
    }
    return 0;
}

/*******************************************************************************
* �������ƣ� CreateSwitch
* ���������� ����Ӳ�����⽻��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� cSwitchCfg --����������Ϣ
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CEmbDVSDev::CreateSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)
{    
    int32 nRet = SWITCH_OPER_SUCCESS;
    
    /* 1�����˿ڣ�Ϊ�˿ڼ���������׼�� */
    nRet = CheckPortIsValid(cSwitchCfg);
    if(ERROR_SWITCH_PORT_ISMIXED == nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::CreateSwitch: call CheckPortIsValid return %d.\n", nRet);
        return SWITCH_OPER_SUCCESS;
    }
    
    if(ERROR_SWITCH_PORT_ISCONFLICT == nRet) 
    {        
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::CreateSwitch: call CheckPortIsValid return %d.\n", nRet);
        return nRet;        
    }
    
    /* 2����һ���Ѿ�����ɾ���ֱ�Ӽ���uplink�˿ڣ��Ѿ����ڻ���õĽ������ظ����� */
    if(SWITCH_OPER_SUCCESS == nRet) 
    {
        if(1 == cSwitchCfg.m_vecPort.size())
        {
            cSwitchCfg.m_strSwIf = cSwitchCfg.m_vecPort.front();
        }
        else
        {
            //����edvs��bond
            if(CreateBridge(cSwitchCfg.m_strName, cSwitchCfg.m_strMixedBridge))
            { 
                VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::CreateSwitch: call CreateBridge return fail.\n");
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Create bridge(%s) failed.", cSwitchCfg.m_strName.c_str());
                return ERROR_SWITCH_ADD_BRIDGE_FAIL;
            }   
            if(CreateBonding(cSwitchCfg))
            { 
                VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::CreateSwitch: call CreateBonding return fail.\n"); 
                return ERROR_SWITCH_ADD_BOND_FAIL;
            }     
            cSwitchCfg.m_strSwIf   = cSwitchCfg.m_strBondName;
            cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND; 
        }
    }

    /* 3�����ý������ԣ��˿����� */
    nRet = SetSwitchAttr(cSwitchCfg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::CreateSwitch: call SetSwitchAttr return fail.\n");
        return ERROR_SWITCH_SET_ATTR_FAIL;
    }
    
    cSwitchCfg.m_nProgress = 100;
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� DeleteSwitch
* ���������� ɾ��Ӳ�����⽻��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CEmbDVSDev::DeleteSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    //ɾ����bond������
    if((0 == cSwitchCfg.m_nIsMixed)&&(!cSwitchCfg.m_strBondName.empty()))
    {
        if(0 != DeleteBridge(cSwitchCfg.m_strName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::DeleteSwitch: call DeleteBridge return fail.\n");
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete bridge(%s) failed.", cSwitchCfg.m_strName.c_str());
            return ERROR_SWITCH_DEL_BRIDGE_FAIL;
        }
    }
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� DeleteSwitchPort
* ���������� ɾ��Ӳ�����⽻����ĳ�����ж˿�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� 
*             
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CEmbDVSDev::DeleteSwitchPort(CSwitchCfgMsgToVNA &cSwitchCfg, vector<string> &vecNewData)
{
    string strSwName   = cSwitchCfg.m_strName;
    string strBondName = cSwitchCfg.m_strBondName;
    string strBondMode  = to_string<int32>(cSwitchCfg.m_nBondMode,dec);
    
    vecNewData.clear();
    
    //ֻ��bond�ҷǻ�������²Ŵ���
    if((!strBondName.empty()) && (0 == cSwitchCfg.m_nIsMixed))
    {
        string strBrIf; 
        VNetGetPhyPortFromBrOVS(strSwName, strBrIf); 
        if(strBrIf.empty())
        {
            //������û��uplink�ڻ��߽��������ڣ���Ϊ�˿��Ѿ�ɾ�������سɹ�
            VNET_LOG(VNET_LOG_INFO, "CEmbDVSDev::DeleteSwitchPort: <%s> didn't have uplink port.\n", strSwName.c_str());
            return SWITCH_OPER_SUCCESS;
        }
        else if(strBrIf == strBondName)
        {
            //��ȡbond��Ա��
            if(VNET_PLUGIN_SUCCESS != VNetGetBondSlavesOVS(strBondName, vecNewData))
            {
                 VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::DeleteSwitchPort: call VNetGetBondSlavesOVS <%s> failed.\n", strBondName.c_str());
                 VNetFormatString(cSwitchCfg.m_strRetInfo, "Get (%s)'s slaves failed.", strBondName.c_str());
                 return ERROR_SWITCH_GET_BONDSLAVE_FAIL;
            }
        }
        else
        {
            //ֻ��һ�������
            vecNewData.push_back(strBrIf);
        }
        
        //ȥ��Ҫɾ���˿ڣ��ҳ�ʣ��Ķ˿ڼ���
        vector<string>::iterator itDelPort = cSwitchCfg.m_vecPort.begin(); 
        for(; itDelPort != cSwitchCfg.m_vecPort.end(); ++itDelPort)
        {
            vector<string>::iterator itOldPort = vecNewData.begin();
            for(; itOldPort != vecNewData.end(); ++itOldPort)
            {
                if(*itDelPort == *itOldPort)
                {
                    vecNewData.erase(itOldPort);
                    break;
                }
            }
        }

        if(vecNewData.empty() || (1 == vecNewData.size()))
        {
            //ɾ��bond��bridge
            int32 nRet = DeleteSwitch(cSwitchCfg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::DeleteSwitchPort: call DeleteSwitch return %d.\n", nRet);
            }
            return nRet;
        }
        else
        {
            //��Ȼ���ж������ڣ�����bond
            if(VNET_PLUGIN_SUCCESS != VNetAddBondOVS(strSwName, strBondName, strBondMode, vecNewData))
            {
                VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::DeleteSwitchPort: call VNetAddBondOVS [%s] create %s failed.\n", strBondName.c_str());
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Create bond(%s) again failed.", strBondName.c_str());
                return ERROR_SWITCH_ADD_BOND_FAIL;
            }   
            cSwitchCfg.m_strSwIf = strBondName;
            cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND;
        }
    }

    //������������
    SetSwitchAttr(cSwitchCfg); 
     
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� SetSwitchAttr
* ���������� ���ý�������(�����˿�����)
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CEmbDVSDev::SetSwitchAttr(CSwitchCfgMsgToVNA &cSwitchCfg)
{
#if 0
    //����MTU(bond/physical)
    if(VNET_PLUGIN_SUCCESS != VNetSetMtu(cSwitchCfg.m_strSwIf, cSwitchCfg.m_cPGInfo.GetMtu()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::SetSwitchAttr: VNetSetMtu<%s, %d> failed.\n", 
                 cSwitchCfg.m_strSwIf.c_str(), cSwitchCfg.m_cPGInfo.GetMtu());   
        return -1;
    }
#endif
    return 0; 
}

/*******************************************************************************
* �������ƣ� IsSwitchValid
* ���������� ������Ч�Լ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� bFlag = TRUE ����= FALSE ����
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CEmbDVSDev::IsSwitchValid(CVNetVnicDetail &cVnicDetail, BOOL bFlag)
{
    int32  nVLAN = 0;
    string strMAC;
    string strPCI = cVnicDetail.GetPciOrder();

    if (strPCI.empty())
    {
        return 0;
    }

    CPhyPortManager * pPortMgr = CPhyPortManager::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::IsSwitchValid: CPhyPortManager::GetInstance() failed.\n"); 
        return -1;
    }

    if(TRUE == bFlag)
    {
        //����ʱ���pci�Ƿ�ʹ��
        if(0 != pPortMgr->IsPciAssignable(strPCI))
        {
            VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::IsSwitchValid: PCI<%s> is invalid.\n", strPCI.c_str()); 
            return -1;
        }
        
        if (0 != cVnicDetail.GetAssMac().size())
        {
            strMAC = cVnicDetail.GetAssMac();
        }
        else
        {
            strMAC = cVnicDetail.GetMacAddress();
        }

        if(EN_SWITCHPORT_MODE_TRUNK == cVnicDetail.GetSwitchportMode())
        {
            nVLAN  = cVnicDetail.GetNativeVlan();
        }
        else
        {
            //����native vlanʱ��vlan��0
            if(GetDVSNativeVlan() != cVnicDetail.GetAccessCvlan())
            {
                nVLAN  = cVnicDetail.GetAccessCvlan();
            }
        }
    
    }
    else
    {
        strMAC = VNET_SRIOV_RANDOM_MAC;
        nVLAN  = 0;
    }
    
    //����mac
    if(0 != pPortMgr->SetVFPortMac(strPCI, strMAC))
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::IsSwitchValid: SetVFPortMac<%s: %s> failed.\n", strPCI.c_str(), strMAC.c_str()); 
        return -1;        
    }
    
    //����vlan
    if(0 != pPortMgr->SetVFPortVlan(strPCI, nVLAN))
    {
        VNET_LOG(VNET_LOG_ERROR, "CEmbDVSDev::IsSwitchValid: SetVFPortVlan<%s: %d> failed.\n", strPCI.c_str(), nVLAN); 
        return -1;        
    }    
    return 0;
}

int32  CEmbDVSDev::SetSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    return 0;
}

int32  CEmbDVSDev::DelSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    return 0;
}


/*******************************************************************************
* �������ƣ� CreateSwitch
* ���������� ����������⽻��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� cSwitchCfg --����������Ϣ
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/ 
int32 CSoftDVSDev::CreateSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    
    /* 1���������� */
    nRet = CreateBridge(cSwitchCfg.m_strName, cSwitchCfg.m_strMixedBridge);
    if(nRet)
    { 
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call CreateBridge return fail.\n");
        VNetFormatString(cSwitchCfg.m_strRetInfo, "Create bridge(%s) failed.", cSwitchCfg.m_strName.c_str());
        return ERROR_SWITCH_ADD_BRIDGE_FAIL;
    }    
    
    /* 2�����˿ڣ�Ϊ�˿ڼ���������׼�� */
    nRet = CheckPortIsValid(cSwitchCfg);
    if(ERROR_SWITCH_PORT_ISCONFLICT == nRet) 
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call CheckPortIsValid return %d.\n", nRet);
        return nRet;        
    }

    /* 3���ڶ����Ѿ�����ɾ���ֱ�Ӽ���uplink�˿ڣ��Ѿ����ڵĽ������ظ����� */
    if(ERROR_SWITCH_ALREADY_EXIST != nRet)
    {
        if(1 == cSwitchCfg.m_vecPort.size())
        {
            cSwitchCfg.m_strSwIf = cSwitchCfg.m_vecPort.front();
            if(VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(cSwitchCfg.m_strName, cSwitchCfg.m_strSwIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call VNetAddNetIfOVS add %s to %s failed.\n", 
                          cSwitchCfg.m_strSwIf.c_str(), cSwitchCfg.m_strName.c_str());
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Add bridge(%s)'s uplinkport(%s) failed.", 
                          cSwitchCfg.m_strName.c_str(), cSwitchCfg.m_strSwIf.c_str());
                return ERROR_SWITCH_ADD_UPLINK_FAIL;
            }
        }
        else
        {
            if(CreateBonding(cSwitchCfg))
            { 
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call CreateBonding return fail.\n"); 
                return ERROR_SWITCH_ADD_BOND_FAIL;
            }     
            cSwitchCfg.m_strSwIf   = cSwitchCfg.m_strBondName;
            cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND; //�޸�Ϊbond���� 
        }
    }

    /* 4������EVB�˿� */
    nRet = ProcEvbPort(cSwitchCfg.m_strName, TRUE);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call ProcEvbPort add evbport failed.\n");
        VNetFormatString(cSwitchCfg.m_strRetInfo, "Create EVB port for bridge(%s) failed.", cSwitchCfg.m_strName.c_str());
        return ERROR_SWITCH_ADD_EVBPORT_FAIL;
    }
 
    /* 5�����ý������ԣ��˿����� */
    nRet = SetSwitchAttr(cSwitchCfg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call SetSwitchAttr return fail.\n");
        return ERROR_SWITCH_SET_ATTR_FAIL;
    }

    /* 6�����ý���SDN Controller */
    nRet = SetSwitchSdnController(cSwitchCfg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: call SetSwitchSdnController failed.\n");
        return ERROR_SWITCH_SET_ATTR_FAIL;
    }
    
    cSwitchCfg.m_nProgress = 100;
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� DeleteSwitch
* ���������� ɾ��������⽻��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSoftDVSDev::DeleteSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    string strSwName   = cSwitchCfg.m_strName;
    string strBondName = cSwitchCfg.m_strBondName;

    // ��ʱ����˳��,����ʱ�޸�
    if(0 != DelSwitchSdnController(cSwitchCfg))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: DelBridge OpenFlow Controller failed.\n", strSwName.c_str());
        return ERROR_SWITCH_DEL_BRIDGE_FAIL;
    }

    //��ɾ��EVB PORT
    if(0 != ProcEvbPort(strSwName, FALSE))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: call ProcEvbPort delete evbport failed.\n");
        VNetFormatString(cSwitchCfg.m_strRetInfo, "Destroy EVB port from bridge(%s) failed.",strSwName.c_str());
        return ERROR_SWITCH_DEL_EVBPORT_FAIL;
    }
    
    //ɾ��VTEP PORT
    if(cSwitchCfg.m_cPGInfo.GetIsolateType())
    {
        CPhyPortManager *pPortMgr = CPhyPortManager::GetInstance();
        if (pPortMgr)
        {
            if(0 != pPortMgr->DeleteVtepPort(cSwitchCfg.m_strName))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: call DeleteVtepPort to delete %s's vtep port failed.\n", 
                    cSwitchCfg.m_strName.c_str());   
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete %s's vtep port failed.",cSwitchCfg.m_strName.c_str());

                return ERROR_SWITCH_DEL_VTEP_FAIL;
            }
        }
    }
    
    if(cSwitchCfg.m_strMixedBridge.empty())
    {
        //ɾ������  
        if(0 != DeleteBridge(strSwName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: call DeleteBridge return fail.\n");
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete bridge(%s) failed.", strSwName.c_str());
            return ERROR_SWITCH_DEL_BRIDGE_FAIL;
        } 
    }
    else
    {
        //������ǰ��ɾ��ͬ������
        if(0 != DeleteBridge(cSwitchCfg.m_strMixedBridge))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: call DeleteBridge return fail.\n");
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete the mixed bridge(%s) failed.", cSwitchCfg.m_strMixedBridge.c_str());
            return ERROR_SWITCH_DEL_BRIDGE_FAIL;
        }
        
        //�ĳ�edvs����
        if(VNET_PLUGIN_SUCCESS != VNetRenameDVS(strSwName, cSwitchCfg.m_strMixedBridge, strBondName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitch: call VNetRenameDVS rename [%s-%s] failed.\n", 
                         cSwitchCfg.m_strMixedBridge.c_str(), strSwName.c_str());
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Rename bridge from %s to %s for mixed bond failed.", 
                               strSwName.c_str(), 
                               cSwitchCfg.m_strMixedBridge.c_str());            
            return ERROR_SWITCH_RENAME_BRIDGE_FAIL;
        } 
    }
       
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� DeleteSwitchPort
* ���������� ɾ��������ĳ�����ж˿�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
*             
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/ 
int32 CSoftDVSDev::DeleteSwitchPort(CSwitchCfgMsgToVNA &cSwitchCfg, vector<string> &vecNewData)
{
    int32  nRet = SWITCH_OPER_SUCCESS;
    string strSwName   = cSwitchCfg.m_strName;
    string strBondName = cSwitchCfg.m_strBondName;
    string strBondMode = to_string<int32>(cSwitchCfg.m_nBondMode,dec);
    
    vecNewData.clear();
    string strBrIf; 
    VNetGetPhyPortFromBrOVS(strSwName, strBrIf); 
    if(strBrIf.empty())
    {
        //������û��uplink�ڣ���Ϊ�˿��Ѿ�ɾ�������سɹ�
        VNET_LOG(VNET_LOG_INFO, "CSoftDVSDev::DeleteSwitchPort: <%s> didn't have uplink port.\n", strSwName.c_str());
        return SWITCH_OPER_SUCCESS;
    }
    else if(strBrIf == strBondName)
    {
        //��ȡbond��Ա��
        if(VNET_PLUGIN_SUCCESS != VNetGetBondSlavesOVS(strBondName, vecNewData))
        {
             VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitchPort: call VNetGetBondSlavesOVS <%s> failed.\n", strBondName.c_str());
             VNetFormatString(cSwitchCfg.m_strRetInfo, "Get (%s)'s slaves failed.", strBondName.c_str());
             return ERROR_SWITCH_GET_BONDSLAVE_FAIL;
        }
    }
    else
    {
        //ֻ��һ�������
        vecNewData.push_back(strBrIf);
    }

    //ȥ��Ҫɾ���˿ڣ��ҳ�ʣ��Ķ˿ڼ���
    vector<string>::iterator itDelPort = cSwitchCfg.m_vecPort.begin(); //�Ѿ���鲻��Ϊ��
    for(; itDelPort != cSwitchCfg.m_vecPort.end(); ++itDelPort)
    {
        vector<string>::iterator itOldPort = vecNewData.begin();
        for(; itOldPort != vecNewData.end(); ++itOldPort)
        {
            if(*itDelPort == *itOldPort)
            {
                vecNewData.erase(itOldPort);
                break;
            }
        }
    }

    if(vecNewData.empty())
    {
        //���ж˿ڶ�ɾ���ˣ�ɾ������
        nRet = DeleteSwitch(cSwitchCfg);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchDev::DeleteSwitchPort: call DeleteSwitch return %d.\n", nRet);
        }
        return nRet;
    }
    else if(1 == vecNewData.size())
    {
        //һ������ڼ��뽻��
        if(0 != AddBridgeNetIf(strSwName, vecNewData.front()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitchPort: call AddBridgeNetIf add %s to %s failed.\n", 
                         vecNewData.front().c_str(), strSwName.c_str());  
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Add switch(%s)'s uplinkport(%s) failed.", 
                         vecNewData.front().c_str(), strSwName.c_str());
            return ERROR_SWITCH_ADD_UPLINK_FAIL;
        }  
        cSwitchCfg.m_strSwIf = vecNewData.front();
        cSwitchCfg.m_strBondName.clear();
    }
    else
    {
        //��Ȼ���ж������ڣ�����bond
        if(VNET_PLUGIN_SUCCESS != VNetAddBondOVS(strSwName, strBondName, strBondMode, vecNewData))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitchPort: call VNetAddBondOVS [%s] create %s failed.\n", strBondName.c_str());
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Create bond(%s) again failed.", strBondName.c_str());
            return ERROR_SWITCH_ADD_BOND_FAIL;
        }   
        cSwitchCfg.m_strSwIf   = strBondName;
        cSwitchCfg.m_nSwIfType = PORT_TYPE_BOND; 
    }

    /* ����EVB�˿� */
    nRet = ProcEvbPort(strSwName, TRUE);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DeleteSwitchPort: call ProcEvbPort add evbport failed.\n");
        VNetFormatString(cSwitchCfg.m_strRetInfo, "Create EVB port for bridge(%s) again failed.", strSwName.c_str());
        return ERROR_SWITCH_ADD_EVBPORT_FAIL;
    }

    //������������
    SetSwitchAttr(cSwitchCfg); 
       
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* �������ƣ� SetSwitchAttr
* ���������� ���ý�������(�����˿�����)
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSoftDVSDev::SetSwitchAttr(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    //��ȡ�����
    vector<string> vecPort;
    if(!cSwitchCfg.m_strBondName.empty())
    {
        VNetGetBondSlavesOVS(cSwitchCfg.m_strBondName, vecPort);
    }
    else
    {
        vecPort.push_back(cSwitchCfg.m_strSwIf);
    }

    //����uplink�˿�MTU
    vector<string>::iterator itPort = vecPort.begin();
    for(; itPort != vecPort.end(); ++itPort)
    {        
        uint32 nOldMTU = 0;
        VNetGetMtu((*itPort).c_str(), nOldMTU); 
        if((int32)nOldMTU != cSwitchCfg.m_cPGInfo.GetMtu())
        {
            if(VNET_PLUGIN_SUCCESS != VNetSetMtu((*itPort), cSwitchCfg.m_cPGInfo.GetMtu()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call VNetSetMtu set<%s, %d> failed.\n", 
                         (*itPort).c_str(), cSwitchCfg.m_cPGInfo.GetMtu()); 
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Setting port(%s) MTU(%d) failed.", 
                         (*itPort).c_str(), cSwitchCfg.m_cPGInfo.GetMtu());
                return -1;
            }   
        }
    }

    //����VLAN TRUNK
    if(cSwitchCfg.m_cPGInfo.GetSwithportMode() == EN_SWITCHPORT_MODE_TRUNK)
    {
        map<uint32,uint32> mapTrunk;
        vector<CTrunkVlanRange> vecVlanRange = cSwitchCfg.m_cPGInfo.GetTrunkVlanRange();
        vector<CTrunkVlanRange>::iterator it_range = vecVlanRange.begin();
        for(; it_range != vecVlanRange.end(); ++it_range)
        {
            mapTrunk.insert(make_pair(it_range->m_nVlanBegin, it_range->m_nVlanEnd));
        }

        if(VNET_PLUGIN_SUCCESS != VNetTrunkWork(cSwitchCfg.m_strSwIf, mapTrunk, cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call VNetTrunkWork set %s nativevlan %d failed.\n", 
                        cSwitchCfg.m_strSwIf.c_str(), cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum()); 
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Setting port(%s) native VLAN(%d) failed.", 
                        cSwitchCfg.m_strSwIf.c_str(), cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum());
            return -1;
        }

        if(VNET_PLUGIN_SUCCESS != VNetTrunkWork(cSwitchCfg.m_strName, mapTrunk, cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call VNetTrunkWork set %s nativevlan %d failed.\n", 
                        cSwitchCfg.m_strName.c_str(), cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum()); 
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Setting bridge(%s) native VLAN(%d) failed.", 
                        cSwitchCfg.m_strName.c_str(), cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum());
            return -1;
        }
    }   

    //EVBģʽ����
    if(cSwitchCfg.m_nEvbMode != GetDVSEvbMode())
    {
        CEvbMgr *pEvbMgr = CEvbMgr::GetInstance();
        if(NULL != pEvbMgr)
        {
            if(0 != pEvbMgr->SetEvbMode(cSwitchCfg.m_strName, cSwitchCfg.m_nEvbMode))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call SetEvbMode<%s, %d> failed.\n", 
                        cSwitchCfg.m_strName.c_str(), cSwitchCfg.m_nEvbMode);
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Setting bridge(%s) EVB mode(%d) failed.", 
                        cSwitchCfg.m_strName.c_str(), cSwitchCfg.m_nEvbMode);
                return -1;
            }
        }
    }
    
    CPhyPortManager *pPortMgr = CPhyPortManager::GetInstance(); 
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: CPhyPortManager::GetInstance() is NULL.\n");   
        return -1;
    }
    //VTEP���̴���
    string strOldVtep = GetDVSVtepPort();
    if(strOldVtep.empty() || (0 != IsVtepPortConsistent(cSwitchCfg)))
    {
        if(0 != pPortMgr->DeleteVtepPort(cSwitchCfg.m_strName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call DeleteVtepPort failed.\n");   
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Delete Vtep port %s failed.",strOldVtep.c_str());
            return -1;
        }
    }
    
    if(EN_ISOLATE_TYPE_VLAN != cSwitchCfg.m_cPGInfo.GetIsolateType())
    {
        if(FALSE == VNetIsVxlanSupport())
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: Vxlan isn't supported in current openvswitch version.\n");  
            VNetFormatString(cSwitchCfg.m_strRetInfo, "Vxlan isn't supported in current openvswitch version.");
            return -1;
        }
        if(0 != IsVtepPortConsistent(cSwitchCfg))
        {
            if(0 != pPortMgr->CreateVtepPort(cSwitchCfg.m_strVtepPort, cSwitchCfg.m_strName, cSwitchCfg.m_strSwIf,
                           cSwitchCfg.m_strVtepIp, cSwitchCfg.m_strVtepMask, cSwitchCfg.m_cPGInfo.GetTrunkNatvieVlanNum()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchAttr: call CreateVtepPort failed.\n");   
                VNetFormatString(cSwitchCfg.m_strRetInfo, "Create Vtep port failed [%s %s %s %s].",cSwitchCfg.m_strVtepPort.c_str(),
                    cSwitchCfg.m_strName.c_str(),cSwitchCfg.m_strVtepIp.c_str(),cSwitchCfg.m_strVtepMask.c_str());
                return -1;
            }
        }
    }

    return 0; 
}

/*******************************************************************************
* �������ƣ� IsSwitchValid
* ���������� ������Ч�Լ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
int32 CSoftDVSDev::IsSwitchValid(CVNetVnicDetail &cVnicDetail, BOOL bFlag)
{
    if(TRUE == bFlag)
    {
        string strDVS = cVnicDetail.GetSwitchName();

        if (strDVS.empty())
        {
            return 0;
        }
            
        //�����Ƿ����
        vector<string> vecBridge;
        VNetGetAllBridges(vecBridge);  
        vector<string>::iterator iter = vecBridge.begin();
        for(; iter != vecBridge.end(); ++iter)
        {
            if(0 == strDVS.compare(*iter))
            {
                return 0;
            }
        } 
        VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::IsSwitchValid: Bridge<%s> is invalid.\n", strDVS.c_str());  
        return 0;    
    }
    return 0;
}

/*******************************************************************************
* �������ƣ� GetSoftDVS
* ���������� �ϱ�������⽻��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     �� ��
*******************************************************************************/
void CSoftDVSDev::GetSoftDVS(vector<CSwitchReportInfo> &vecDVSReport)
{
    vector<string> vecBridge;
    VNetGetAllBridges(vecBridge);  
    vector<string>::iterator iter = vecBridge.begin();
    for(; iter != vecBridge.end(); ++iter)
    {
        CSwitchReportInfo cReport;
        cReport.m_strSwName  = *iter;
        cReport.m_nSwType    = SOFT_VIR_SWITCH_TYPE;

        VNetGetPhyPortFromBrOVS(*iter, cReport.m_strSwIf);

        //��ȡbond��Ա��
        if(!cReport.m_strSwIf.empty())
        {
            vector<string> vecBond;
            VNetGetAllBondingsOVS(vecBond);
            if(find(vecBond.begin(), vecBond.end(), cReport.m_strSwIf) != vecBond.end())
            {
                VNetGetBondSlavesOVS(cReport.m_strSwIf, cReport.m_vecPort);
            }
            else
            {
                cReport.m_vecPort.push_back(cReport.m_strSwIf);
            }
        }
        vecDVSReport.push_back(cReport);
    }
}

/*******************************************************************************
* �������ƣ� SetSwitchSdnController
* ���������� ���ý���SDN Controller
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/9          V1.0       xubb         �� ��
*******************************************************************************/
int32 CSoftDVSDev::SetSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)
{ 
    if((cSwitchCfg.m_cPGInfo.GetIsSdn() == 1) && (0 != cSwitchCfg.m_strSdnCfgUuid.size()))
    {
        if(0 != cSwitchCfg.m_strName.size() 
            && 0 != cSwitchCfg.m_strProto.size() 
            && 0 != cSwitchCfg.m_strIP.size() 
            && 0 != cSwitchCfg.m_nPort)
        {
            if(VNET_PLUGIN_SUCCESS != VNetSetSdnController(cSwitchCfg.m_strName, cSwitchCfg.m_strProto, cSwitchCfg.m_strIP, cSwitchCfg.m_nPort))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::SetSwitchSdnController: Setting SDVS sdn controller failed.\n"); 
                return -1;
            }
        }
    }    
    
    return 0; 
}

/*******************************************************************************
* �������ƣ� DelSwitchSdnController
* ���������� ɾ������SDN Controller
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/9          V1.0       xubb         �� ��
*******************************************************************************/
int32 CSoftDVSDev::DelSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)
{
    if(1 == cSwitchCfg.m_cPGInfo.GetIsSdn() && 0 != cSwitchCfg.m_strName.size())
    {
        if(VNET_PLUGIN_SUCCESS != VNetDelSdnController(cSwitchCfg.m_strName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DelSwitchSdnController: Del SDVS openflow controller failed.\n"); 
            return -1;
        }
    }   

    return 0; 
}

}

