/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_msg.h
* 文件标识：
* 内容摘要：vnet_msg.h的定义文件,存放VNET中内部模块消息体定义。
* 当前版本：1.0
* 作    者：
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/8/16
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
******************************************************************************/

#if !defined(VNET_MSG_INCLUDE_H__)
#define VNET_MSG_INCLUDE_H__

#include "vnet_comm.h"
#include "serialization.h"
#include "vnet_vnic.h"
#include "vnet_portinfo.h"
#include <algorithm>

namespace zte_tecs
{
    // VNA 向VNM组播注册请求消息   
    class CVNARegModMsg : public Serializable
    {
    public:
        explicit CVNARegModMsg() {}
        virtual ~CVNARegModMsg() {}

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNARegModMsg);  
            WRITE_DIGIT(m_ucModRole);            
            WRITE_DIGIT(m_ucModState); 
            WRITE_STRING(m_strModApp);
            WRITE_STRING(m_strExtInfo);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNARegModMsg);
            READ_DIGIT(m_ucModRole);            
            READ_DIGIT(m_ucModState); 
            READ_STRING(m_strModApp);
            READ_STRING(m_strExtInfo);
            DESERIALIZE_END();
        }

        void Print(void)
        {
            cout << "Mod role: " << (int32) m_ucModRole 
                << ",application: " << m_strModApp 
                 << ",State: "<< (int32)m_ucModState <<endl;

            if(!m_strExtInfo.empty())     
            {
                cout << ",external information: " << m_strExtInfo <<".";
            }
            else
            {
                cout << ",external information: NULL.";
            }
            
            cout<<endl;
            
            return ;
        }

    public:
        int32 m_ucModRole;          //用于标识MODULE的角色        
        int32 m_ucModState;         //用于标记MODULE的状态
        string m_strModApp;         //用于记录MODULE的application
        string m_strExtInfo;        //用于标记MODULE的附加信息，比如HC所属的CC信息
    };

    //VNM通知VNA已注册接口消息
    class CVNARegModReqMsg: public Serializable
    {
     public:
        explicit CVNARegModReqMsg() {};
        virtual ~CVNARegModReqMsg() {};
        
     public:
       SERIALIZE
        {
            SERIALIZE_BEGIN(CVNARegModReqMsg);  
            WRITE_STRING(m_strVNAName);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNARegModReqMsg);
            READ_STRING(m_strVNAName);
            DESERIALIZE_END();
        };

        void Print()
        {
            cout << "CVNARegModReqMsg:" <<endl;
            cout << "   m_strVNAName     : " << m_strVNAName << endl;
        };
    public:
        string m_strVNAName; // VNA Application
    };

    //VNM组播自己优先级、主用、APP等信息消息
    class CVNMInfoMsg : public Serializable
    {
    public:
        explicit CVNMInfoMsg() { m_ucMasterOrSlave = VNM_SLAVE_STATE;}
        virtual ~CVNMInfoMsg() {} 

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNMInfoMsg); 
            WRITE_DIGIT(m_ucMasterOrSlave);            
            WRITE_DIGIT(m_ucPriority);
            WRITE_STRING(m_strApp);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNMInfoMsg);
            READ_DIGIT(m_ucMasterOrSlave);            
            READ_DIGIT(m_ucPriority);
            READ_STRING(m_strApp);
            DESERIALIZE_END();
        };
        
        void Print(void)
        {
            cout << "Application: " << m_strApp
                 << ", Priority: " << m_ucPriority 
                 << ", MS: " ;
            if(VNM_MASTER_STATE == m_ucMasterOrSlave)
            {
                cout << "Master" << endl;
            }
            else
            {
                cout << "Slave" << endl;
            }            
            
            return ;
        }

    public:
        int32 m_ucMasterOrSlave;
        int32 m_ucPriority;
        string m_strApp;        
    };

    //VNM通知VNA已取消注册接口消息
    class CVNAUnRegModReqMsg: public Serializable
    {
    public:
        explicit CVNAUnRegModReqMsg() {};
        virtual ~CVNAUnRegModReqMsg() {};
        
     public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNAUnRegModReqMsg);  
            WRITE_STRING(m_strVNAName);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNAUnRegModReqMsg);
            READ_STRING(m_strVNAName);
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CVNAUnRegModReqMsg:" <<endl;
            cout << "   m_strVNAName     : " << m_strVNAName << endl;
        };
                
    public:
        string m_strVNAName;
    };
    
    class CSwitchReportInfo: public Serializable
    {

    public:
        void Print()
        {
            cout << "CSwitchReportInfo:" <<endl;
            cout << "|--Switch name: " << m_strSwName << endl;
            cout << "|--Switch type: " << m_nSwType << endl;
            cout << "|--Switch port: " << m_strSwIf << endl;
        };
        
        string   m_strSwName;  /* switch name */
        int32    m_nSwType;    /* soft dvs */
        string   m_strSwIf;    /* 接入交换的端口，可能是bond，可能是ethx */
        vector<string>  m_vecPort;

        SERIALIZE 
        {
            SERIALIZE_BEGIN(CSwitchReportInfo);
            WRITE_STRING(m_strSwName);
            WRITE_DIGIT(m_nSwType);
            WRITE_STRING(m_strSwIf);
            WRITE_VALUE(m_vecPort);
            SERIALIZE_END();
        };
        
        DESERIALIZE  
        {
            DESERIALIZE_BEGIN(CSwitchReportInfo);
            READ_STRING(m_strSwName);
            READ_DIGIT(m_nSwType);
            READ_STRING(m_strSwIf);
            READ_VALUE(m_vecPort);
            DESERIALIZE_END();
        };
    };

    class CVNAReportMsg : public Serializable
    {
    public:    
        explicit CVNAReportMsg() {};
        virtual ~CVNAReportMsg() {};

    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNAReportMsg);  
            WRITE_STRING(m_strVNAApp);
            WRITE_STRING(m_strHostName);
            WRITE_OBJECT_VECTOR(m_vecRegistedModInfo);
            WRITE_OBJECT_VECTOR(m_vecSwitchInfo);
            WRITE_OBJECT(m_cPortInfo);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNAReportMsg);
            READ_STRING(m_strVNAApp);
            READ_STRING(m_strHostName);
            READ_OBJECT_VECTOR(m_vecRegistedModInfo);
            READ_OBJECT_VECTOR(m_vecSwitchInfo);
            READ_OBJECT(m_cPortInfo);
            DESERIALIZE_END();
        };

        CVNARegModMsg *FindRegMod(const string &cstrModName)
        {
            vector<CVNARegModMsg>::iterator itr = m_vecRegistedModInfo.begin();
            for(; itr != m_vecRegistedModInfo.end(); ++itr)
            {
                if(cstrModName == itr->m_strModApp)
                {
                    return &(*itr);
                }
            }
            
            return NULL;
        }
        
        void Print(void)
        {
           cout << "CVNAReportMsg"  << endl;
            cout << "VNA application: " << m_strVNAApp
                 << ",Host name: " << m_strHostName << endl;
           cout << endl;

           cout << "Module info list:" <<endl;
           
            vector<CVNARegModMsg>::iterator itr = m_vecRegistedModInfo.begin();
            for(; itr != m_vecRegistedModInfo.end(); ++itr)
            {
                cout << "    ";
                itr->Print();                
            }                 

           cout << endl;
           cout << "Switch info list:" <<endl;

            vector<CSwitchReportInfo>::iterator itrSw = m_vecSwitchInfo.begin();
            for(; itrSw != m_vecSwitchInfo.end(); ++itrSw)
            {
                cout << "    ";
                itrSw->Print();                
            }   

           cout << endl;
           cout << "Port info list:" <<endl;

            m_cPortInfo.Print();
            
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };

    public:
        string m_strVNAApp;
        string m_strHostName; 
        vector<CVNARegModMsg> m_vecRegistedModInfo;
        vector<CSwitchReportInfo> m_vecSwitchInfo;
        CMessageVNAPortInfoReport m_cPortInfo;
    };


    //VNA注册接口消息，用于CC/HC向VNA注册
    class CModuleRegisterToVNAMsg: public Serializable
    {
    public:
        explicit CModuleRegisterToVNAMsg() {};
        virtual ~CModuleRegisterToVNAMsg() {};
        
    public:
        SERIALIZE
        {
            SERIALIZE_BEGIN(CModuleRegisterToVNAMsg);  
            WRITE_DIGIT(m_nModuleRole);
            WRITE_STRING(m_strModuleApp);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CModuleRegisterToVNAMsg);
            READ_DIGIT(m_nModuleRole);
            READ_STRING(m_strModuleApp);
            DESERIALIZE_END();
        };

    public:
        int m_nModuleRole;
        string m_strModuleApp;
    };


    //取消MODULE向VNA注册接口消息，用于CC/HC向VNA注册
    class CModuleUnRegisterToVNAMsg: public Serializable
    {
    public:
        explicit CModuleUnRegisterToVNAMsg() {};
        virtual ~CModuleUnRegisterToVNAMsg() {};
        
    public:        
        SERIALIZE
        {
            SERIALIZE_BEGIN(MsgUnregisterToVNA);  
            WRITE_DIGIT(m_nModuleRole);
            WRITE_STRING(m_strModuleApp);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(MsgUnregisterToVNA);
            READ_DIGIT(m_nModuleRole);
            READ_STRING(m_strModuleApp);
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "MsgUnregisterToVNA:" <<endl;
            cout << "   m_nModuleRole     : " << m_nModuleRole << endl;
            cout << "   m_strModuleApp    : " << m_strModuleApp << endl; 
        };
    public:
        int m_nModuleRole;
        string m_strModuleApp;
    };

    //用于CC/HC/DHCP等业务模块向VNA发送心跳
    class CModuleHeartBeatMsg: public Serializable
    {
    public:
        explicit CModuleHeartBeatMsg() {};
        virtual ~CModuleHeartBeatMsg() {};
        
    public:            
        SERIALIZE
        {
            SERIALIZE_BEGIN(CModuleHeartBeatMsg);  
            WRITE_STRING(m_strModuleApp);
            SERIALIZE_END();
        };

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CModuleHeartBeatMsg);
            READ_STRING(m_strModuleApp);
            DESERIALIZE_END();
        };
        
    public:
        string m_strModuleApp;
    };       

    //描述一个VM的VSI信息
    class CVMVSIInfo: public Serializable
    {
    public:
        explicit CVMVSIInfo() {};
        virtual ~CVMVSIInfo() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVMVSIInfo);
            WRITE_DIGIT(m_nVMID);
            WRITE_DIGIT(m_nProjectID);
            WRITE_STRING(m_strClusterApp);
            WRITE_STRING(m_strHostApp);
            WRITE_OBJECT_VECTOR(m_vecVnicDetail);
            WRITE_OBJECT(m_cWatchDogInfo);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVMVSIInfo);
            READ_DIGIT(m_nVMID);  
            READ_DIGIT(m_nProjectID);  
            READ_STRING(m_strClusterApp);
            READ_STRING(m_strHostApp);
            READ_OBJECT_VECTOR(m_vecVnicDetail);
            READ_OBJECT(m_cWatchDogInfo);
            DESERIALIZE_END();
        };

        void SetVnicInfo(CVNetVmMem &cVnetVmMem)
        {
            m_vecVnicDetail.clear();

            vector<CVNetVnicMem> vecVnicMem;
            cVnetVmMem.GetVecVnicMem(vecVnicMem);

            vector<CVNetVnicMem>::iterator itrVnic = vecVnicMem.begin();
            for ( ; itrVnic != vecVnicMem.end(); ++itrVnic)
            {
                CVNetVnicDetail cVnicDetail;

                cVnicDetail.SetNicIndex(itrVnic->GetNicIndex());
                cVnicDetail.SetIsSriov(itrVnic->GetIsSriov()) ;   
                cVnicDetail.SetIsLoop(itrVnic->GetIsLoop()) ;  
                cVnicDetail.SetLogicNetworkUuid(itrVnic->GetLogicNetworkUuid()) ;  
                cVnicDetail.SetPortType(itrVnic->GetPortType()) ;
                cVnicDetail.SetAssIp(itrVnic->GetAssIP()) ;
                cVnicDetail.SetAssMask(itrVnic->GetAssMask()) ;
                cVnicDetail.SetAssGateway(itrVnic->GetAssGateway()) ;
                cVnicDetail.SetAssMac(itrVnic->GetAssMac());
                cVnicDetail.SetVSIIDValue(itrVnic->GetVsiIdValue()); 
                cVnicDetail.SetVSIIDFormat(itrVnic->GetVsiIdFormat()); 
                cVnicDetail.SetPGUuid(itrVnic->GetVmPgUuid()); 
                cVnicDetail.SetIPAddress(itrVnic->GetIp()) ;     
                cVnicDetail.SetNetmask(itrVnic->GetMask()); 
                cVnicDetail.SetMacAddress(itrVnic->GetMac()); 
                cVnicDetail.SetSwitchUuid(itrVnic->GetVSwitchUuid()); 
                cVnicDetail.SetVportUuid(itrVnic->GetSriovVfPortUuid());
                cVnicDetail.SetQuantumNetworkUuid(itrVnic->GetQuantumNetworkUuid());
                cVnicDetail.SetQuantumPortUuid(itrVnic->GetQuantumPortUuid());

                m_vecVnicDetail.push_back(cVnicDetail);
            }
        };

        void SetVnicInfo(vector<CVNetVnicConfig> &vecVnicCfg)
        {
            m_vecVnicDetail.clear();

            vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
            for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
            {
                CVNetVnicDetail cVnicDetail;

                cVnicDetail.SetNicIndex(itrVnic->m_nNicIndex);
                cVnicDetail.SetIsSriov(itrVnic->m_nIsSriov) ;   
                cVnicDetail.SetIsLoop(itrVnic->m_nIsLoop) ;  
                cVnicDetail.SetLogicNetworkUuid(itrVnic->m_strLogicNetworkID) ;  
                cVnicDetail.SetPortType(itrVnic->m_strAdapterModel) ; 
                cVnicDetail.SetAssIp(itrVnic->m_strIP) ;
                cVnicDetail.SetAssMask(itrVnic->m_strNetmask) ;
                cVnicDetail.SetAssGateway(itrVnic->m_strGateway) ;
                cVnicDetail.SetAssMac(itrVnic->m_strMac) ;
                cVnicDetail.SetVSIIDValue(itrVnic->m_strVSIProfileID); 

                m_vecVnicDetail.push_back(cVnicDetail);
            }
        };

        void Print()
        {
            cout << "CVMVSIInfo:" <<endl;
            cout << "   m_vm_id     : " << m_nVMID << endl;
            cout << "   m_nProjectID     : " << m_nProjectID << endl;
            
            cout << "vnic detail list:"<< endl;
            vector<CVNetVnicDetail>::iterator itrVnic = m_vecVnicDetail.begin();
            for ( ; itrVnic != m_vecVnicDetail.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            m_cWatchDogInfo.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
               
    public:
        int64   m_nVMID;         // vm id
        int64   m_nProjectID;         // m_nProjectID
	    string m_strClusterApp;
	    string m_strHostApp;
        vector<CVNetVnicDetail> m_vecVnicDetail;
        CWatchDogInfo m_cWatchDogInfo;
    };


    //VNA向VNM请求VM相关的VSI信息
    class CVMVSIInfoReqMsg: public Serializable
    {
    public:
        explicit CVMVSIInfoReqMsg() {};
        virtual ~CVMVSIInfoReqMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVMVSIInfoReqMsg);
            WRITE_DIGIT(m_nVMID);
            WRITE_OBJECT_VECTOR(m_vecVnicCfg);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVMVSIInfoReqMsg);
            READ_DIGIT(m_nVMID);
            READ_OBJECT_VECTOR(m_vecVnicCfg);
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CVMVSIInfoReqMsg:" <<endl;
            cout << "   m_vm_id     : " << m_nVMID << endl;

			
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
        
    public:
        int64 m_nVMID;
        vector<CVNetVnicConfig> m_vecVnicCfg;
    };

    //VNA向VNM请求VM相关的VSI信息的应答消息
    class CVMVSIInfoReqAckMsg: public Serializable
    {
    public:
        explicit CVMVSIInfoReqAckMsg() {};
        virtual ~CVMVSIInfoReqAckMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVMVSIInfoReqAckMsg);
            WRITE_DIGIT(m_nReturnCode);
            WRITE_OBJECT(m_cVMVSIInfo);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVMVSIInfoReqAckMsg);
            READ_DIGIT(m_nReturnCode);
            READ_OBJECT(m_cVMVSIInfo);
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CVMVSIInfoReqMsg:" <<endl;
            cout << "   m_nReturnCode     : " << m_nReturnCode << endl;
            m_cVMVSIInfo.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
        
    public:
        int32 m_nReturnCode;
        CVMVSIInfo m_cVMVSIInfo;
    };
        
    //VNM向VNA下发增加VM相关的VSI信息
    class CAddVMVSIInfoMsg: public Serializable
    {
    public:
        explicit CAddVMVSIInfoMsg() {};
        virtual ~CAddVMVSIInfoMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CAddVMVSIInfoMsg);
            WRITE_OBJECT(m_cVMVSIInfo);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CAddVMVSIInfoMsg);
            READ_OBJECT(m_cVMVSIInfo);
            DESERIALIZE_END();
        };
		
        void SetVnicInfo(CVNetVmMem &cVnetVmMem)
    	{
    		m_cVMVSIInfo.SetVnicInfo(cVnetVmMem);
    	};
		
        void Print()
        {
            cout << "CAddVMVSIInfoMsg:" <<endl;
            m_cVMVSIInfo.Print();
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
                
    public:
        CVMVSIInfo m_cVMVSIInfo;
    };


    //VNM向VNA下发删除VM相关的VSI信息
    class CDelVMVSIInfoMsg: public Serializable
    {
    public:
        explicit CDelVMVSIInfoMsg() {};
        virtual ~CDelVMVSIInfoMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CDelVMVSIInfoMsg);
            WRITE_DIGIT(m_nVMID);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CDelVMVSIInfoMsg);
            READ_DIGIT(m_nVMID);  
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CDelVMVSIInfoMsg:" <<endl;
            cout << "   m_vm_id     : " << m_nVMID << endl;
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
                
    public:
        int64   m_nVMID;         // vm id
    };

    //VNM向VNA下发所有和此VNA相关的VM相关的VSI信息
    class CVNMAddAllVMVSIInfoMsg: public Serializable
    {
    public:
        explicit CVNMAddAllVMVSIInfoMsg() {};
        virtual ~CVNMAddAllVMVSIInfoMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CVNMAddAllVMVSIInfoMsg);
            WRITE_OBJECT_VECTOR(m_vecVMVSIInfo);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CVNMAddAllVMVSIInfoMsg);
            READ_OBJECT_VECTOR(m_vecVMVSIInfo);  
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CVNMAddAllVMVSIInfoMsg:" <<endl;            
            cout << "VM vnic detail list:"<< endl;
            vector<CVMVSIInfo>::iterator itrVnic = m_vecVMVSIInfo.begin();
            for ( ; itrVnic != m_vecVMVSIInfo.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
                
    public:
        vector<CVMVSIInfo> m_vecVMVSIInfo;
    };

    //HC向VNA下发所有和此VNA相关的VM相关的VSI信息
    class CHCAddAllVMVSIInfoMsg: public Serializable
    {
    public:
        explicit CHCAddAllVMVSIInfoMsg() {};
        virtual ~CHCAddAllVMVSIInfoMsg() {};
        
    public:                        
        SERIALIZE
        {
            SERIALIZE_BEGIN(CHCAddAllVMVSIInfoMsg);
            WRITE_OBJECT_VECTOR(m_vecVMVSIInfo);
            SERIALIZE_END();
        };
        
        DESERIALIZE
        {
            DESERIALIZE_BEGIN(CHCAddAllVMVSIInfoMsg);
            READ_OBJECT_VECTOR(m_vecVMVSIInfo);  
            DESERIALIZE_END();
        };
        
        void Print()
        {
            cout << "CHCAddAllVMVSIInfoMsg:" <<endl;            
            cout << "VM vnic detail list:"<< endl;
            vector<CVMVSIInfo>::iterator itrVnic = m_vecVMVSIInfo.begin();
            for ( ; itrVnic != m_vecVMVSIInfo.end(); ++itrVnic)
            {
                itrVnic->Print();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
            cout << endl;
        };
                
    public:
        vector<CVMVSIInfo> m_vecVMVSIInfo;
    };

class CPortGroup : public Serializable
{
public:
    explicit CPortGroup()        
    {
        m_nPGType           = 0;
        m_nDefaultVlan      = 0;
        m_nPromisc          = 0;
        m_nMacVlanFilter    = 0;
        m_nVersion          = 0;
        m_nMacChange        = 0;
        m_nSwitchPortMode   = 0;   
        m_nIsCfgNetPlane    = 0;
        m_nMTU              = 0;
        m_nAccessVlanNum    = 0;
        m_nAccessIsolateNum = 0; 
        m_nTrunkNativeVlan  = 0;
        m_nIsolateType      = ISOLATE_VLAN; 
        m_nSegmentId        = 0;
        m_nIsSdn            = 0;
    };
    virtual ~CPortGroup(){};
    void Print(void)
    {
        cout << "Name               : " << m_strName << endl;   
        cout << "PgType             : " << m_nPGType<< endl; 
        cout << "Uuid               : " << m_strUUID<< endl; 
        cout << "Acl                : " << m_strACL<< endl; 
        cout << "Qos                : " << m_strQOS<< endl; 
        #if 0
        cout << "AllowPriorities    : " << cDbPG.GetAllowPriorities()<< endl;  
        cout << "RcvBandwidthLmt    : " << cDbPG.GetRcvBandwidthLmt()<< endl; 
        cout << "RcvBandwidthRsv    : " << cDbPG.GetRcvBandwidthRsv()<< endl; 
        cout << "DftVlanId          : " << cDbPG.GetDftVlanId()<< endl; 
        cout << "Promiscuous        : " << cDbPG.GetPromiscuous()<< endl; 
        cout << "MacVlanFltEnable   : " << cDbPG.GetMacVlanFltEnable()<< endl; 
        cout << "AllowTrmtMacs      : " << cDbPG.GetAllowTrmtMacs()<< endl; 
        cout << "AllowTrmtMacVlans  : " << cDbPG.GetAllowTrmtMacVlans()<< endl; 
        cout << "PlyBlkOver         : " << cDbPG.GetPlyBlkOver()<< endl; 
        cout << "PlyVlanOver        : " << cDbPG.GetPlyVlanOver()<< endl; 
        cout << "Version            : " << cDbPG.GetVersion()<< endl; 
        cout << "MgrId              : " << cDbPG.GetMgrId()<< endl; 
        cout << "TypeId             : " << cDbPG.GetTypeId()<< endl; 
        cout << "AllowMacChg        : " << cDbPG.GetAllowMacChg()<< endl; 
        #endif
        cout << "SwithportMode      : " << m_nSwitchPortMode<< endl; 
        cout << "IsCfgNetplane      : " << m_nIsCfgNetPlane<< endl; 
        cout << "NetplaneUuid       : " << m_strNetPlaneUUID<< endl; 
        cout << "Mtu                : " << m_nMTU<< endl; 
        cout << "TrunkNatvieVlanNum : " << m_nTrunkNativeVlan<< endl; 
        cout << "AccessVlanNum      : " << m_nAccessVlanNum<< endl; 
        cout << "AccessIsolateNo    : " << m_nAccessIsolateNum<< endl;                 
        cout << "m_nIsolateType     : " << m_nIsolateType<< endl; 
        cout << "SegmentId          : " << m_nSegmentId<< endl;                 

        cout << "TrunkVlanRange: "<<endl;
        vector<CTrunkVlanRange>::iterator iter = m_vecTrunkVlanRange.begin();
        for(; iter != m_vecTrunkVlanRange.end(); ++iter)
        {
            cout << "Range: "<<iter->m_nVlanBegin <<" -- "<<iter->m_nVlanEnd<<endl;
        }
        
    };
    
public:    
    string & GetName() { return m_strName;};   
    int32    GetPgType() {return m_nPGType;};
    string & GetUuid() {return m_strUUID;};
    string & GetAcl() {return m_strACL;};
    string & GetQos() {return m_strQOS;};
    string & GetAllowPriorities() {return m_strPrior;};    
    string & GetRcvBandwidthLmt() {return m_strRcvBWLimit;};    
    string & GetRcvBandwidthRsv() {return m_strRcvBWRsv;};    
    int32 GetDftVlanId() {return m_nDefaultVlan;};    
    int32 GetPromiscuous() {return m_nPromisc;};    
    int32 GetMacVlanFltEnable() {return m_nMacVlanFilter;};    
    string & GetAllowTrmtMacs() {return m_strTxMacs;};     
    string & GetAllowTrmtMacVlans() {return m_strTxMacvlans;};    
    string & GetPlyBlkOver() {return m_strPolicyBlkOver;};    
    string & GetPlyVlanOver() {return m_strPolicyVlanOver;};    
    int32 GetVersion() {return m_nVersion;};    
    string & GetMgrId() {return m_strMgrId;};    
    string & GetTypeId() {return m_strVSIType;};    
    int32 GetAllowMacChg() {return m_nMacChange;};   
    int32 GetSwithportMode() {return m_nSwitchPortMode;};    
    int32 GetIsCfgNetplane() {return m_nIsCfgNetPlane;};    
    string & GetNetplaneUuid() {return m_strNetPlaneUUID;};
    int32 GetIsSdn() {return m_nIsSdn;};
    int32 GetMtu() {return m_nMTU;};    
    int32 GetTrunkNatvieVlanNum() {return m_nTrunkNativeVlan;};    
    int32 GetAccessVlanNum() {return m_nAccessVlanNum;};    
    int32 GetAccessIsolateNo() {return m_nAccessIsolateNum;};   
    int32 GetIsolateType() {return m_nIsolateType;};   
    int32 GetSegmentId() {return m_nSegmentId;};   
    
    vector<CTrunkVlanRange> & GetTrunkVlanRange(){return m_vecTrunkVlanRange;}
    
    void SetName(const string  &value) {m_strName = value;};   
    void SetPgType(int32 value) {m_nPGType = value;};
    void SetUuid(const string  &value) {m_strUUID = value;};
    void SetAcl(const string  &value) {m_strACL = value;};
    void SetQos(const string  &value) {m_strQOS = value;};
    void SetAllowPriorities(const string  &value) {m_strPrior = value;};    
    void SetRcvBandwidthLmt(const string  &value) {m_strRcvBWLimit = value;};    
    void SetRcvBandwidthRsv(const string  &value) {m_strRcvBWRsv = value;};    
    void SetDftVlanId(int32 value) {m_nDefaultVlan = value;};    
    void SetPromiscuous(int32 value) {m_nPromisc = value;};    
    void SetMacVlanFltEnable(int32 value) {m_nMacVlanFilter = value;};    
    void SetAllowTrmtMacs(const string  &value) {m_strTxMacs = value;};     
    void SetAllowTrmtMacVlans(const string  &value) {m_strTxMacvlans = value;};    
    void SetPlyBlkOver(const string  &value) {m_strPolicyBlkOver = value;};    
    void SetPlyVlanOver(const string  &value) {m_strPolicyVlanOver = value;};    
    void SetVersion(int32 value) {m_nVersion = value;};    
    void SetMgrId(const string  &value) {m_strMgrId = value;};    
    void SetTypeId(const string  &value) {m_strVSIType = value;};    
    void SetAllowMacChg(int32 value) {m_nMacChange = value;};   
    void SetSwithportMode(int32 value) {m_nSwitchPortMode = value;};    
    void SetIsCfgNetplane(int32 value) {m_nIsCfgNetPlane = value;};    
    void SetNetplaneUuid(const string  &value) {m_strNetPlaneUUID = value;};
    void SetIsSdn(int32 value) {m_nIsSdn = value;};
    void SetMtu(int32 value) {m_nMTU = value;};    
    void SetTrunkNatvieVlanNum(int32 value) {m_nTrunkNativeVlan = value;};    
    void SetAccessVlanNum(int32 value) {m_nAccessVlanNum = value;};    
    void SetAccessIsolateNo(int32 value) {m_nAccessIsolateNum = value;}; 
    void SetIsolateType(int32 value) {m_nIsolateType = value;}; 
    void SetSegmentId(int32 value) {m_nSegmentId = value;};     
    void SetTrunkVlanRange(CTrunkVlanRange &cVlanRange)
    {
        m_vecTrunkVlanRange.push_back(cVlanRange);
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CPortGroup);
        WRITE_VALUE(m_strName);
        WRITE_VALUE(m_strUUID);
        WRITE_VALUE(m_nPGType);
        WRITE_VALUE(m_strACL);
        WRITE_VALUE(m_strQOS);
        WRITE_VALUE(m_strPrior);
        WRITE_VALUE(m_strRcvBWLimit);
        WRITE_VALUE(m_strRcvBWRsv);
        WRITE_VALUE(m_nDefaultVlan);
        WRITE_VALUE(m_nPromisc);
        WRITE_VALUE(m_nMacVlanFilter);
        WRITE_VALUE(m_strTxMacs);
        WRITE_VALUE(m_strTxMacvlans);
        WRITE_VALUE(m_strPolicyBlkOver);
        WRITE_VALUE(m_strPolicyVlanOver);
        WRITE_VALUE(m_nVersion);
        WRITE_VALUE(m_strMgrId);
        WRITE_VALUE(m_strVSIType);
        WRITE_VALUE(m_nMacChange);
        WRITE_VALUE(m_nSwitchPortMode);
        WRITE_VALUE(m_nIsCfgNetPlane);
        WRITE_VALUE(m_strNetPlaneUUID);
        WRITE_VALUE(m_nIsSdn);
        WRITE_VALUE(m_nMTU);
        WRITE_VALUE(m_nAccessVlanNum);
        WRITE_VALUE(m_nAccessIsolateNum);
        WRITE_VALUE(m_nTrunkNativeVlan);
        WRITE_VALUE(m_nIsolateType);
        WRITE_VALUE(m_nSegmentId);             
        WRITE_OBJECT_VECTOR(m_vecTrunkVlanRange);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CPortGroup);
        READ_VALUE(m_strName);
        READ_VALUE(m_strUUID);
        READ_VALUE(m_nPGType);
        READ_VALUE(m_strACL);
        READ_VALUE(m_strQOS);
        READ_VALUE(m_strPrior);
        READ_VALUE(m_strRcvBWLimit);
        READ_VALUE(m_strRcvBWRsv);
        READ_VALUE(m_nDefaultVlan);
        READ_VALUE(m_nPromisc);
        READ_VALUE(m_nMacVlanFilter);
        READ_VALUE(m_strTxMacs);
        READ_VALUE(m_strTxMacvlans);
        READ_VALUE(m_strPolicyBlkOver);
        READ_VALUE(m_strPolicyVlanOver);
        READ_VALUE(m_nVersion);
        READ_VALUE(m_strMgrId);
        READ_VALUE(m_strVSIType);
        READ_VALUE(m_nMacChange);
        READ_VALUE(m_nSwitchPortMode);
        READ_VALUE(m_nIsCfgNetPlane);
        READ_VALUE(m_strNetPlaneUUID);
        READ_VALUE(m_nIsSdn);
        READ_VALUE(m_nMTU);
        READ_VALUE(m_nAccessVlanNum);
        READ_VALUE(m_nAccessIsolateNum);
        READ_VALUE(m_nTrunkNativeVlan);
        READ_VALUE(m_nIsolateType);
        READ_VALUE(m_nSegmentId);      
        READ_OBJECT_VECTOR(m_vecTrunkVlanRange);
        DESERIALIZE_END();
    };  
    
public:
    string m_strName;
    string m_strUUID;
    string m_strdescription;
    int32  m_nPGType;
    string m_strACL;
    string m_strQOS;;
    string m_strPrior;
    string m_strRcvBWLimit;
    string m_strRcvBWRsv;
    int32  m_nDefaultVlan;
    int32  m_nPromisc;
    int32  m_nMacVlanFilter;
    string m_strTxMacs;
    string m_strTxMacvlans;
    string m_strPolicyBlkOver;
    string m_strPolicyVlanOver;
    int32  m_nVersion;
    string m_strMgrId;
    string m_strVSIType;
    int32  m_nMacChange;
    int32  m_nSwitchPortMode;  
    int32  m_nIsCfgNetPlane;
    string m_strNetPlaneUUID;
    int32  m_nIsSdn;
    int32  m_nMTU;
    int32  m_nAccessVlanNum;
    int32  m_nAccessIsolateNum; 
    int32  m_nTrunkNativeVlan;
    int32  m_nIsolateType; 
    int32  m_nSegmentId;

    vector<CTrunkVlanRange> m_vecTrunkVlanRange;
};

class CSwitchCfgMsgToVNA: public Serializable
{
    public:
        explicit CSwitchCfgMsgToVNA() 
        {
            m_nType     = 0;
            m_nMTU      = 0;
            m_nEvbMode  = 0;
            m_nMaxVnic  = 0;
            m_nBondMode = 0;
            m_nSwIfType = 0;
            m_nRetCode  = 0;
            m_nProgress = 0;
            m_nIsMixed  = 0;
            m_nIsOffline = 0;
            m_strSdnCfgUuid = "";
            m_strProto  = "";
            m_strIP     = "";
            m_nPort     = 0;
        };
        virtual ~CSwitchCfgMsgToVNA() {};
        
    public:
        void Print(void)
        {
            cout << endl;
            cout << "**********************************************************"<<endl;
            cout << "[VNA] Switch operation code: " << m_nOper << endl;
            cout << "Switch Name : " << m_strName << endl;   
            cout << "Switch Type : " << m_nType<< endl; 
            cout << "Switch Uuid : " << m_strUuid<< endl; 
            cout << "Switch Attribute: "<<endl;
            cout << " -->MTU: " << m_nMTU<< ", EVB: " <<m_nEvbMode<<", MaxVnics: "<<m_nMaxVnic<<", BondMode: "<<m_nBondMode<<endl;
            cout << " -->PG_UUID : "<<m_strPGUuid<<endl; 
            cout << " -->VNA_UUID: "<<m_strVnaUuid<<endl; 
            cout << " -->Switch Interface: "<<m_strSwIf<<", Type: "<<m_nSwIfType<<", IsOffline: "<<m_nIsOffline<<endl;
            cout << " -->IsMixed: "<<m_nIsMixed <<", Mixed Brdige: "<<m_strMixedBridge<<endl;
            cout << " -->Uplink Port: ";
            vector<string>::iterator it = m_vecPort.begin();
            for(; it != m_vecPort.end(); ++it)
            {
                cout << (*it) << " "; 
            }
            cout <<endl;
            cout << " -->Vetp-Port: "<<m_strVtepPort<<", IP: "<<m_strVtepIp<<", Mask: "<<m_strVtepMask<<endl;
            cout << " -->Request ID: "<<m_strReqId<<", progress: "<<m_nProgress<<endl;
            cout << " -->Controller Pr: "<<m_strProto<<endl;
            cout << " -->Controller Ip: "<<m_strIP<<endl;
            cout << " -->Controller Port: "<<m_nPort<<endl;
            cout << " -->Result: " << m_nRetCode <<endl;
            cout << endl;
            cout << "**********************************************************"<<endl;
        }    

        SERIALIZE 
        {
            SERIALIZE_BEGIN(CSwitchCfgMsgToVNA);
            WRITE_VALUE(m_strName);
            WRITE_VALUE(m_strUuid);
            WRITE_VALUE(m_nType);
            WRITE_VALUE(m_nMTU);
            WRITE_VALUE(m_nEvbMode);
            WRITE_VALUE(m_nMaxVnic);
            WRITE_VALUE(m_strPGUuid);
            WRITE_VALUE(m_vecPort);
            WRITE_VALUE(m_nBondMode);
            WRITE_VALUE(m_strVnaUuid);
            WRITE_OBJECT(m_cPGInfo);
            WRITE_VALUE(m_strWCTaskUuid);  
            WRITE_VALUE(m_nIsMixed);
            WRITE_VALUE(m_strMixedBridge);
            WRITE_VALUE(m_nIsOffline);
            WRITE_VALUE(m_strBondName);         
            WRITE_VALUE(m_strSwIf);
            WRITE_VALUE(m_nSwIfType);
            WRITE_VALUE(m_nOper);
            WRITE_VALUE(m_strReqId);
            WRITE_VALUE(m_nRetCode);
            WRITE_VALUE(m_strRetInfo);
            WRITE_VALUE(m_nProgress);
            WRITE_OBJECT(m_apisvr);
            WRITE_VALUE(m_strVtepPort);
            WRITE_VALUE(m_strVtepIp);
            WRITE_VALUE(m_strVtepMask);
            WRITE_VALUE(m_strSdnCfgUuid);
            WRITE_VALUE(m_strProto);
            WRITE_VALUE(m_strIP);
            WRITE_VALUE(m_nPort);
            SERIALIZE_END();
        };
    
        DESERIALIZE  
        {
            DESERIALIZE_BEGIN(CSwitchCfgMsgToVNA);
            READ_VALUE(m_strName);
            READ_VALUE(m_strUuid);
            READ_VALUE(m_nType);
            READ_VALUE(m_nMTU);
            READ_VALUE(m_nEvbMode);
            READ_VALUE(m_nMaxVnic);
            READ_VALUE(m_strPGUuid);
            READ_VALUE(m_vecPort);
            READ_VALUE(m_nBondMode);
            READ_VALUE(m_strVnaUuid);
            READ_OBJECT(m_cPGInfo);
            READ_VALUE(m_strWCTaskUuid);  
            READ_VALUE(m_nIsMixed);
            READ_VALUE(m_strMixedBridge);
            READ_VALUE(m_nIsOffline);
            READ_VALUE(m_strBondName);
            READ_VALUE(m_strSwIf);
            READ_VALUE(m_nSwIfType);
            READ_VALUE(m_nOper);
            READ_VALUE(m_strReqId);
            READ_VALUE(m_nRetCode);
            READ_VALUE(m_strRetInfo);
            READ_VALUE(m_nProgress);  
            READ_OBJECT(m_apisvr);
            READ_VALUE(m_strVtepPort);
            READ_VALUE(m_strVtepIp);
            READ_VALUE(m_strVtepMask);
            READ_VALUE(m_strSdnCfgUuid);
            READ_VALUE(m_strProto);
            READ_VALUE(m_strIP);
            READ_VALUE(m_nPort);
            DESERIALIZE_END();
        };
        
    public:
        // 基本属性
        string         m_strName;
        string         m_strUuid;
        int32          m_nType;    /* 0-physwitch，1-soft dvs, 2-emd dvs */
        int32          m_nMTU;      
        int32          m_nEvbMode;   
        int32          m_nMaxVnic;
        string         m_strPGUuid;  
        //高级属性 
        vector<string> m_vecPort;     //uplink port   
        int32          m_nBondMode;
        string         m_strVnaUuid;   //VNA UUID
        CPortGroup     m_cPGInfo;      //PG属性
        string         m_strWCTaskUuid; //通配专用UUID，为空时表示非通配消息
        int32          m_nIsMixed;   //端口混用标记 1有效
        string         m_strMixedBridge; //混用的网桥名字
        int32          m_nIsOffline;  //VNA离线标记，1表示离线，0表示在线 
        
        string         m_strBondName;  //生成的bond名字，由VNA生成
        string         m_strSwIf;      //接入交换的端口，可能是bond，可能是ethx，由VNA生成
        int32          m_nSwIfType;    //交换端口的类型，bond/physical/uplinkloop，由VNA生成
        //操作属性
        int32          m_nOper;
        string         m_strReqId;  
        int32          m_nRetCode;
        string         m_strRetInfo;
        int32          m_nProgress;
        MID            m_apisvr;   
        //vxlan属性
        string         m_strVtepPort;
        string         m_strVtepIp;
        string         m_strVtepMask;

        //OpenFlow Controller
        string         m_strSdnCfgUuid;
        string         m_strProto;
        string         m_strIP;
        int32          m_nPort;
    };


//PORT Config
class CPortCfgMsgToVNA: public Serializable
{
    public:
        explicit CPortCfgMsgToVNA() 
        {
            m_strName = "";
            m_strUuid = "";
            m_strIP = "";
            m_nIsDhcp = 0;
            m_strMask = "";
            m_nType = 0;
            m_strVnaUuid = "";
            m_strHostName = "";
            m_strReqId = "";
            m_nOper = 0;
            m_strWCTaskUuid = "";
            m_strSwitchName = "";
            m_strPgUuid = "";
            m_strUplinkPort = "";
            m_nMtu = 0;
            m_nVlan = 0;
            m_nRetCode = 0;
            m_nProgress = 0;
        };
        virtual ~CPortCfgMsgToVNA() {};
    public:
        void Print(void)
        {
            cout << endl;
            cout << "**********************************************************"<<endl;
            cout << "Port Name : " << m_strName << endl;   
            cout << "Vna  UUID : " << m_strVnaUuid<< endl;
            cout << "Port UUID : " << m_strUuid << endl;
            cout << "Port IP :   " << m_strIP<< endl; 
            cout << "DHCP    :   " << m_nIsDhcp<<endl;
            cout << "Port Mask : " << m_strMask<< endl;  
            cout << "Port Type : " << m_nType<< endl; 
            cout << "Port ReqId : " << m_strReqId<< endl; 
            cout << "Port Oper : " << m_nOper<< endl; 
            cout << "Port WC : " << m_strWCTaskUuid<< endl; 
            cout << "Port Switch :" << m_strSwitchName <<endl;
            cout << "Port Pg :" << m_strPgUuid <<endl;
            cout << "Port (uplink) :" << m_strUplinkPort <<endl;
            cout << "Port MTU :" << m_nMtu <<endl;
            cout << "Port VLAN :" << m_nVlan <<endl;			
            cout << "Port RetCode : " << m_nRetCode<< endl; 
            cout << "Port Progress : " << m_nProgress<< endl; 
            cout << "**********************************************************"<<endl;
        }    

        SERIALIZE 
        {
            SERIALIZE_BEGIN(CPortCfgMsgToVNA);
            WRITE_VALUE(m_strName);
            WRITE_VALUE(m_strUuid);
            WRITE_VALUE(m_strIP);
            WRITE_VALUE(m_nIsDhcp);
            WRITE_VALUE(m_strMask);
            WRITE_VALUE(m_nType);
            WRITE_VALUE(m_strVnaUuid);
            WRITE_VALUE(m_strReqId);
            WRITE_VALUE(m_nOper);
            WRITE_OBJECT(m_apisvr);
            WRITE_VALUE(m_strWCTaskUuid);
            WRITE_VALUE(m_strSwitchName);
            WRITE_VALUE(m_strPgUuid);
            WRITE_VALUE(m_strUplinkPort);
            WRITE_VALUE(m_nMtu);
            WRITE_VALUE(m_nVlan);
            WRITE_VALUE(m_nRetCode);
            WRITE_VALUE(m_nProgress);
            SERIALIZE_END();
        };
    
        DESERIALIZE  
        {
            DESERIALIZE_BEGIN(CPortCfgMsgToVNA);
            READ_VALUE(m_strName);
            READ_VALUE(m_strUuid);
            READ_VALUE(m_strIP);
            READ_VALUE(m_nIsDhcp);
            READ_VALUE(m_strMask);
            READ_VALUE(m_nType);
            READ_VALUE(m_strVnaUuid);
            READ_VALUE(m_strReqId);
            READ_VALUE(m_nOper);
            READ_OBJECT(m_apisvr);
            READ_VALUE(m_strWCTaskUuid);
            READ_VALUE(m_strSwitchName);
            READ_VALUE(m_strPgUuid);
            READ_VALUE(m_strUplinkPort);
            READ_VALUE(m_nMtu);
            READ_VALUE(m_nVlan);
            READ_VALUE(m_nRetCode);
            READ_VALUE(m_nProgress);        
            DESERIALIZE_END();
        };
        
    public:
        string         m_strName;
        string         m_strUuid;
        string         m_strIP;
        int32          m_nIsDhcp;
        string         m_strMask;
        int32          m_nType;
        string         m_strVnaUuid;
        string         m_strHostName;
        string         m_strReqId;  
        int32          m_nOper;
        MID            m_apisvr;   
        string         m_strWCTaskUuid;
        string         m_strSwitchName;
        string         m_strPgUuid;
        string         m_strUplinkPort;
        uint32          m_nMtu;
        uint32          m_nVlan;	
        int32          m_nRetCode;
        int32          m_nProgress;
    };

    class CVxlanMcGroup: public Serializable
    {
        public:
            explicit CVxlanMcGroup() 
            {
                m_strNetplaneId  = "";
                m_strSegmentId   = "";
                m_vecLocalVtepIp.clear();
                m_vecVtepIp.clear();
                
            };
            virtual ~CVxlanMcGroup() 
            {
                m_strNetplaneId  = "";
                m_strSegmentId   = "";
                m_vecLocalVtepIp.clear();
                m_vecVtepIp.clear();
            };
            
        public:
            SERIALIZE 
            {
                SERIALIZE_BEGIN(CVxlanMcGroup);
                WRITE_VALUE(m_strNetplaneId);
                WRITE_VALUE(m_strSegmentId);
                WRITE_VALUE(m_vecLocalVtepIp);
                WRITE_VALUE(m_vecVtepIp);
                SERIALIZE_END();
            };
        
            DESERIALIZE  
            {
                DESERIALIZE_BEGIN(CVxlanMcGroup);
                READ_VALUE(m_strNetplaneId);
                READ_VALUE(m_strSegmentId);
                READ_VALUE(m_vecLocalVtepIp);
                READ_VALUE(m_vecVtepIp);
                DESERIALIZE_END();
            };           
        
            bool operator < (const CVxlanMcGroup& VxlanMcGroup) const
            {
                if (m_strNetplaneId < VxlanMcGroup.m_strNetplaneId)
                {
                    return true;
                }

                if (m_strSegmentId < VxlanMcGroup.m_strSegmentId)
                {
                    return true;
                }                
  
                return false;     
            }

            void operator = (CVxlanMcGroup& VxlanMcGroup)
            {
                m_strNetplaneId = VxlanMcGroup.m_strNetplaneId;
                m_strSegmentId  = VxlanMcGroup.m_strSegmentId;

                
                m_vecLocalVtepIp.clear();
                m_vecVtepIp.clear();
                
                vector<string>::iterator it = VxlanMcGroup.m_vecLocalVtepIp.begin();               
                for (; it!=VxlanMcGroup.m_vecLocalVtepIp.end(); ++it)
                {
                    m_vecLocalVtepIp.push_back(*it);  
                }

                it = VxlanMcGroup.m_vecVtepIp.begin();   
                for (; it!=VxlanMcGroup.m_vecVtepIp.end(); ++it)
                {
                    m_vecVtepIp.push_back(*it);  
                }
                
            }
            
            bool operator == (CVxlanMcGroup& VxlanMcGroup) 
            {           
                if (!(m_strNetplaneId == VxlanMcGroup.m_strNetplaneId))
                {
                    return false;
                }

                if (!(m_strSegmentId == VxlanMcGroup.m_strSegmentId))
                {
                    return false;
                }


                sort(m_vecLocalVtepIp.begin(), m_vecLocalVtepIp.end());
                sort(VxlanMcGroup.m_vecLocalVtepIp.begin(), VxlanMcGroup.m_vecLocalVtepIp.end());
                if (!(m_vecLocalVtepIp == VxlanMcGroup.m_vecLocalVtepIp))
                {
                    return false;
                }

                sort(m_vecVtepIp.begin(), m_vecVtepIp.end());
                sort(VxlanMcGroup.m_vecVtepIp.begin(), VxlanMcGroup.m_vecVtepIp.end());
                if (!(m_vecVtepIp == VxlanMcGroup.m_vecVtepIp))
                {
                    return false;
                }
               
                return true;    
            }
            
        public:
            string         m_strNetplaneId;
            string         m_strSegmentId;
            vector<string> m_vecLocalVtepIp;            
            vector<string> m_vecVtepIp;
            
    };

    class CVxlanMcGroupTable: public Serializable
    {
        public:
            explicit CVxlanMcGroupTable() {}; 
            virtual ~CVxlanMcGroupTable() {};
            
        public:
            void Print(void)
            {
                cout << "VNA Vxlan Multicast Group Info:                                                             " << endl;        
                cout << "--------------------------------------------------------------------------------------------" << endl;
                cout << "NetPlane    Segment       Vteps ([...] is local vtep)                                       " << endl;
                cout << "--------------------------------------------------------------------------------------------" << endl;

                vector<CVxlanMcGroup>::iterator it = m_vecVxlanMcGroup.begin();
                for(; it!=m_vecVxlanMcGroup.end(); ++it)
                {  
                    cout << left << setw(12) << it->m_strNetplaneId;
                    cout << left << setw(14) << it->m_strSegmentId;
                    
                    vector<string>::iterator iter = it->m_vecLocalVtepIp.begin();
                    int i=1;
                    for(; iter != it->m_vecLocalVtepIp.end(); ++i,++iter)
                    {
                        string slocal = "[" + (*iter) + "]";
                        cout << left << setw(17) << slocal;
                        if (i >= 4)
                        {
                            i = 0;
                            string spaces = "                          ";
                            cout << endl << spaces;
                        }            
                    }
                    
                    iter = it->m_vecVtepIp.begin();
                    for(; iter != it->m_vecVtepIp.end(); ++i,++iter)
                    {
                        cout << left << setw(17) << (*iter);
                        if (i >= 4)
                        {
                            i = 0;
                            string spaces = "                          ";
                            cout << endl << spaces;
                        }            
                    }
                    cout << endl;
                    cout << "--------------------------------------------------------------------------------------------" << endl;
                }
                cout << "--------------------------------------------------------------------------------------------" << endl;
                
            }

            SERIALIZE 
            {
                SERIALIZE_BEGIN(CVxlanMcGroupToVNA);
                WRITE_OBJECT_VECTOR(m_vecVxlanMcGroup)
                SERIALIZE_END();
            };
        
            DESERIALIZE  
            {
                DESERIALIZE_BEGIN(CVxlanMcGroupToVNA);
                READ_OBJECT_VECTOR(m_vecVxlanMcGroup)
                DESERIALIZE_END();
            };

            void operator = (CVxlanMcGroupTable& VxlanMcGroupTable)
            {
                m_vecVxlanMcGroup.clear();
                
                vector<CVxlanMcGroup>::iterator it = VxlanMcGroupTable.m_vecVxlanMcGroup.begin();               
                for (; it!=VxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++it)
                {
                    m_vecVxlanMcGroup.push_back(*it);  
                }
            }
            
            bool operator == (CVxlanMcGroupTable& VxlanMcGroupTable)
            {
                sort(m_vecVxlanMcGroup.begin(), m_vecVxlanMcGroup.end());
                sort(VxlanMcGroupTable.m_vecVxlanMcGroup.begin(), VxlanMcGroupTable.m_vecVxlanMcGroup.end());
                if (m_vecVxlanMcGroup.size()!=VxlanMcGroupTable.m_vecVxlanMcGroup.size())
                {
                    return false;
                }

                if (m_vecVxlanMcGroup.empty())
                {
                    return true;
                }

                vector<CVxlanMcGroup>::iterator it1 = m_vecVxlanMcGroup.begin();
                vector<CVxlanMcGroup>::iterator it2 = VxlanMcGroupTable.m_vecVxlanMcGroup.begin();

                for (; it1!=m_vecVxlanMcGroup.end(); ++it1,++it2)
                {
                    if (!(*it1==*it2))
                    {
                        return false;
                    }
                }
               
                return true;    
            }
            
            
        public:
            vector<CVxlanMcGroup>  m_vecVxlanMcGroup;
            
    };
    


    class CVxlanMcGroupToVNA: public Serializable
    {
        public:
            explicit CVxlanMcGroupToVNA() {}; 
            virtual ~CVxlanMcGroupToVNA() {};
            
        public:
            void Print(void)
            {
                m_cVxlanMcGroupTable.Print();
            }

            SERIALIZE 
            {
                SERIALIZE_BEGIN(CVxlanMcGroupToVNA);
                WRITE_OBJECT(m_cVxlanMcGroupTable)
                SERIALIZE_END();
            };
        
            DESERIALIZE  
            {
                DESERIALIZE_BEGIN(CVxlanMcGroupToVNA);
                READ_OBJECT(m_cVxlanMcGroupTable)
                DESERIALIZE_END();
            };
            
        public:
            CVxlanMcGroupTable  m_cVxlanMcGroupTable;
            
    };

    // 通知NEW VNA
    class CNewVnaNotify: public Serializable
    {
        public:
            explicit CNewVnaNotify() {}; 
            virtual ~CNewVnaNotify() {};

        public:
            void Print(void)
            {
                cout << "CNewVnaNotify vna uuid :" << m_strNewVnaUuid << endl;
            }

            SERIALIZE 
            {
                SERIALIZE_BEGIN(CNewVnaNotify);
                WRITE_STRING(m_strNewVnaUuid);
                SERIALIZE_END();
            };
        
            DESERIALIZE  
            {
                DESERIALIZE_BEGIN(CNewVnaNotify);
                READ_STRING(m_strNewVnaUuid);
                DESERIALIZE_END();
            };

        public:
            string   m_strNewVnaUuid;
            
    };

    class CUpdateCfgToVNA: public Serializable
    {
        public:
            explicit CUpdateCfgToVNA() {}; 
            virtual ~CUpdateCfgToVNA() {};
            
        public:
            void Print(void)
            {
                cout << "CUpdateCfgToVNA vna uuid :" << m_strVnaUuid << endl;
            }

            SERIALIZE 
            {
                SERIALIZE_BEGIN(CUpdateCfgToVNA);
                WRITE_STRING(m_strVnaUuid);
                SERIALIZE_END();
            };
        
            DESERIALIZE  
            {
                DESERIALIZE_BEGIN(CUpdateCfgToVNA);
                READ_STRING(m_strVnaUuid);
                DESERIALIZE_END();
            };

        public:
            string   m_strVnaUuid;
            
    };
}



#endif // VNET_MSG_DATA_INCLUDE_H__

