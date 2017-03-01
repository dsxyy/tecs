
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

#if !defined(_VNET_DB_INTERFACE_INCLUDE_H_)
#define _VNET_DB_INTERFACE_INCLUDE_H_

#include <string>
#include <vector>
#include "pub.h"

using namespace std;

namespace zte_tecs
{

class CDbResourceAllocNum
{
public: 
    CDbResourceAllocNum() 
    {
        m_nRet = m_nNum = 0;
     };
    virtual ~CDbResourceAllocNum() {};
    
    int32 GetRet(){return m_nRet;}; 
    int32 GetNum(){return m_nNum;}; 

    void SetRet(int32 value) {m_nRet = value;};   
    void SetNum(int32 value) {m_nNum = value;};   
private:
    int32 m_nRet;
    int32 m_nNum;
};


class CDBObjectBase
{
public:
    CDBObjectBase(){m_strUuid = m_strName = "";/*m_strState = 0;*/};
    virtual ~CDBObjectBase() {};

    string & GetUuid(){return m_strUuid;};
    string & GetName(){return m_strName;};   
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetName(const char* name) {m_strName = name;};    

    void DbgShow()
    {
        cout << "|" <<setw(32) << m_strUuid << 
                    "|" << setw(32) << m_strName << endl;
    }
private:    
    //int32  m_strState;
    string m_strUuid;
    string m_strName;
};

#if 0
class IDBOperate
{
public:
    virtual int32 QuerySummary(vector<CDBObjectBase *> &) = 0;
    virtual int32 Query(CDBObjectBase * ) = 0;   
    virtual int32 Add(CDBObjectBase *) = 0;
    virtual int32 Modify(CDBObjectBase *) = 0;
    virtual int32 Del(string uuid) = 0;    
};
#endif 

typedef enum tagDbOperatorType
{
    EN_DB_OPERATE_NETPLANE = 0,
    EN_DB_OPERATE_PORGGROUP,
    EN_DB_OPERATE_LOGICNETWORK,
    EN_DB_OPERATE_VNA,
    EN_DB_OPERATE_PORT,    
    EN_DB_OPERATE_KERNEL,
    EN_DB_OPERATE_SWITCH,   
    EN_DB_OPERATE_RESOURCE_MAC, 
    EN_DB_OPERATE_RESOURCE_IP,
    EN_DB_OPERATE_RESOURCE_VLAN,
    EN_DB_OPERATE_SCHEDULE_DEPLOY,
    EN_DB_OPERATE_PORT_PROFILE,
}EN_DB_OPERATE_TYPE;

} // namespace zte_tecs

#endif // _VNET_DB_INTERFACE_INCLUDE_H_ 


