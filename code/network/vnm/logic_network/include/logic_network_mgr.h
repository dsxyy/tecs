/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�logic_network_mgr.h
* �ļ���ʶ��
* ����ժҪ��CLogicNetworkMgr��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(LOGIC_NETWORK_MGR_INCLUDE_H__)
#define LOGIC_NETWORK_MGR_INCLUDE_H__

namespace zte_tecs
{
    class CLogicNetworkMgr
    {
    public:
        explicit CLogicNetworkMgr();
        virtual ~CLogicNetworkMgr();

    public:
        static CLogicNetworkMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CLogicNetworkMgr();
            }
            
            return s_pInstance;
        };

        int32 GetPortGroupUUID(const string &cstrLogiNetUUID, string &strPortGroupUUID);
        int32 GetLogicNetwork(const string &cstrUUID, CLogicNetwork &cLogicNetwork);
        int32 GetLogiNetByNetplane(const string &cstrNetplaneUUID, vector<CLogicNetwork> &vecLogiNet);
        int32 GetNetplaneByLogiNetID(const string &cstrLogiNetUUID, string &strNetplaneUUID);
        int32 CheckAllVlanTrunkOfSRIOV(vector<CVNetVnicConfig> &vecVNetVnic);
        int32 CreateQuantumNetwork(CVNetVmMem &cVnetVmMem);
        int32 CreateQuantumNetwork(const CLogicNetworkMsg &cMsg, const string &cstrLnUuid);
        int32 DeleteQuantumNetwork(CVNetVmMem &cVnetVmMem);
        int32 DeleteQuantumNetwork(const string &cstrLNUuid);
        int32 RESTfulCreateNetwork(const string &cstrQuantumSvrIP, const string &cstrNetworkName, const int64 cnProjectID, string &QuantumNetworkUuid);
        int32 RESTfulDeleteNetwork(const string &cstrQuantumSvrIP, const string &QuantumNetworkUuid);

    public: 
        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);

        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        void DbgShowData(const string &cstrPortGroupUUID);
        
    protected:
        int32 ProcLogicNetworkMsg(const MessageFrame &message);
        int32 ProcLogicNetworkName2UuidMsg(const MessageFrame &message);

    private:
        int32 AddLogicNetwork(CLogicNetworkMsg &cMsg, string &strRetInfo);
        int32 DelLogicNetwork(const CLogicNetworkMsg &cMsg, string &strRetInfo);
        int32 ModLogicNetwork(const CLogicNetworkMsg &cMsg, string &strRetInfo);

        int32 GetLogicNetworkByName(const string &strLnName,string & strUuid, string &strRetInfo);

    private:
        static CLogicNetworkMgr *s_pInstance;

        MessageUnit *m_pMU;
        BOOL m_bOpenDbgInf;
        
        DISALLOW_COPY_AND_ASSIGN(CLogicNetworkMgr);
    };
}// namespace zte_tecs

#endif

