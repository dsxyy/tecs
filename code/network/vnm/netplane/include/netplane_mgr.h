/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�netplane_mgr.h
* �ļ���ʶ��
* ����ժҪ��CNetplaneMgr��Ķ����ļ�
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

#if !defined(NETPLANE_MGR_INCLUDE_H__)
#define NETPLANE_MGR_INCLUDE_H__

#define  NETPLANE_MAX_NUM      10000  

namespace zte_tecs
{
    class CNetplane;
    class CNetplaneMgr
    {
    public:
        explicit CNetplaneMgr();
        virtual ~CNetplaneMgr();

    public:
        static CNetplaneMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CNetplaneMgr();
            }
            
            return s_pInstance;
        };

        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);
        
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        void DbgShowAllData(void);     

    public:
        int32 GetAllNetplane(vector<CNetplane> &vecNetplane);
        int32 ApplyNetplaneId(int32 &nID);        
        int32 GetNetplaneMTU(const string &strNetplaneID, int32 &nMTU);
        int32 GetNetplaneId(const string &strNetplaneUuid, int32 &nId);
        int32 IsValidId(const string &strNetplaneUuid);        
        int32 ConvertIdToNickName(int32 nId, string &strNickName);        

    protected:
        int32 ProcNetplaneMsg(const MessageFrame &message);

    private:
        int32 AddNetplane(CNetplaneMsg &cMsg, string &strRetInfo);
        int32 DelNetplane(const CNetplaneMsg &cMsg, string &strRetInfo);
        int32 ModNetplane(const CNetplaneMsg &cMsg, string &strRetInfo);

    private:
        static CNetplaneMgr *s_pInstance;

        MessageUnit *m_pMU;
        map<int32, BOOL> m_mapNetplaneIdPool;
        BOOL m_bOpenDbgInf;
        
        DISALLOW_COPY_AND_ASSIGN(CNetplaneMgr);
    };
}// namespace zte_tecs

#endif

