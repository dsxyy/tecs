/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：netplane_mgr.h
* 文件标识：
* 内容摘要：CNetplaneMgr类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
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

