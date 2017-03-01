

/******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_db_netplane.h
* �ļ���ʶ��
* ����ժҪ��CVNetDb��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��28��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/28
*     �� �� �ţ�V1.0
*     �� �� �ˣ�gong.xiefeng
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(_VNET_DB_NETPLANE_SEGMENTPOOL_INCLUDE_H_)
#define _VNET_DB_NETPLANE_SEGMENTPOOL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

// logicnetwork db
class CDbNetplaneSegmentPool
{
public: 
    CDbNetplaneSegmentPool() 
    {
        m_strUuid =  "";
        m_nSegmentNum = 0;
    };
    virtual ~CDbNetplaneSegmentPool() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int32  GetSegmentNum(){return m_nSegmentNum;};
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetSegmentNum(int32 value) {m_nSegmentNum = value;};
    
    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl;  
        cout << "vlan_Num: " << m_nSegmentNum << endl;   
    }
private:    
    string m_strUuid;  // netplane uuid
    int32  m_nSegmentNum;
};


class CDbVnaNpSegmentId
{
public: 
    CDbVnaNpSegmentId() 
    {
        m_strVnaUuid = m_strNpUuid= m_strIp=m_strMask= "";
        m_nSegmentNum = 0;
    };
    virtual ~CDbVnaNpSegmentId() {};
     
    string & GetVnaUuid(){return m_strVnaUuid;}; 
    string & GetNpUuid(){return m_strNpUuid;};
    int32  GetSegmentNum(){return m_nSegmentNum;};
    string & GetIp(){return m_strIp;};
    string & GetMask(){return m_strMask;};
     
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetNpUuid(const char* value) {m_strNpUuid = value;};
    void SetSegmentNum(int32 value) {m_nSegmentNum = value;};
    void SetIp(const char* value) {m_strIp = value;};
    void SetMask(const char* value) {m_strMask = value;};
    
    void DbgShow()
    {       
        cout << "VnaUuid    : " << m_strVnaUuid << endl;  
        cout << "NpUuid     : " << m_strNpUuid << endl; 
        cout << "Segment_Num: " << m_nSegmentNum << endl;   
        cout << "Ip         : " << m_strIp << endl; 
        cout << "Mask       : " << m_strMask << endl;         
    }
    
    BOOL operator<( const CDbVnaNpSegmentId &oDbVnaNpSegmentId ) const
    {
        return m_nSegmentNum < oDbVnaNpSegmentId.m_nSegmentNum;
    }

    BOOL operator==( const CDbVnaNpSegmentId &oDbVnaNpSegmentId ) const
    {
        return m_nSegmentNum == oDbVnaNpSegmentId.m_nSegmentNum;
    }
    
private:    
    string m_strVnaUuid;  // vna uuid
    string m_strNpUuid;   // np uuid    
    int32  m_nSegmentNum;
    string m_strIp;
    string m_strMask;
};

class CVNetDbIProcedure;
class CDBOperateNetplaneSegmentPool : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneSegmentPool(CVNetDbIProcedure * pb);  
    virtual ~CDBOperateNetplaneSegmentPool();  

    int32 Query(const char* uuid, vector<CDbNetplaneSegmentPool> & outVInfo);   
    int32 QueryShareNetSegment(const char* NetplaneUUID, vector<int32>& outVInfo);
    int32 QueryPrivateNetIsolateNo(const char* NetplaneUUID, vector<int32>& outVInfo);    
#if 0    
    int32 QueryShareNetAccessVlan(const char* NetplaneUUID, vector<int32>& outVInfo);    
    int32 QueryShareNetTrunkVlan(const char* NetplaneUUID, vector<CDbNetplaneTrunkVlan>& outVInfo);
#endif     
    int32 Add(CDbNetplaneSegmentPool & info);  
    int32 Del(CDbNetplaneSegmentPool & info);  

    int32 CheckAdd(CDbNetplaneSegmentPool & info);  
    int32 CheckDel(CDbNetplaneSegmentPool & info);  

    // ��ȡvna netplane segmentid ��Ϣ
    // vna_uuid = "",������vna���й���
    // np_uuid = "", ������np���й���
    // seg = -1 , ������seg���й���
    int32 QuerySegIdIplistByVnaNpSeg(const char* vna_uuid, const char* np_uuid,int32 segNum, vector<CDbVnaNpSegmentId> & outVInfo);
    
    int32 DbProcCallback(int32 type, CADORecordset *, void* );  

    void DbgShow();      
private:
    int32 QueryPrivateNetIsolateNoCallback(CADORecordset * prs,void * nil);  
    int32 QueryCallback(CADORecordset * prs,void * nil);   
    int32 OperateCallback(CADORecordset * prs,void * nil);  
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  

    int32 QuerySegIdIplistCallback(CADORecordset * prs,void * nil);

    int32 QuerySegIdIplistShareByVnaNpSeg(const char* vna_uuid, const char* np_uuid,int32 segNum,vector<CDbVnaNpSegmentId> & outVInfo);
    int32 QuerySegIdIplistPrivateByVnaNpSeg(const char* vna_uuid, const char* np_uuid,int32 segNum, vector<CDbVnaNpSegmentId> & outVInfo);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneSegmentPool);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_VLANPOOL_INCLUDE_H_ 



