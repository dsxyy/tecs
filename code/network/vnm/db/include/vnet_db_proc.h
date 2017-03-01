
/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_proc.h
* 文件标识：
* 内容摘要：CVNetDb类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/

#if !defined(_VNET_DB_PROC_INCLUDE_H_)
#define _VNET_DB_PROC_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_error.h"

namespace zte_tecs
{

// 存贮过程操作(add,modify,del)结果结构体
class CDbProcOperateResult
{
public:
    CDbProcOperateResult(){m_nRet = 0; m_oid = -1; m_strUuid = "";};
    virtual ~CDbProcOperateResult(){};

    int32 GetRet() {return m_nRet;};
    int64 GetOid() {return m_oid;};   
    string & GetUuid() {return m_strUuid;};
    void  SetRet(int32 ret) {m_nRet = ret;};
    void  SetOid(int32 id) {m_oid = id;};
    void  SetUuid(const char* uuid) {m_strUuid = uuid;};
    
private:
    int32 m_nRet;  // = 0 success; = other failed.
    int64 m_oid;   // talbe id; if ret = 0, valid.
    string m_strUuid;// talbe uuid; if ret = 0, (netplane,port,pg,lgnetwork,switch) valid.
};

typedef  enum tagDbProcRtnType{
    DB_PROC_RTN_RECORD = 0,
    DB_PROC_RTN_CURSOR,        
}DB_PROC_RTN_TYPES;

// proc db callback base class 
class CVNetDbIProcedureCallback
{
public:    
    CVNetDbIProcedureCallback() {};
    virtual ~CVNetDbIProcedureCallback() {};
    virtual int32 DbProcCallback(int32 type, CADORecordset *, void* )=0;
};

class CVNetDbIProcedure
{
public:
    CVNetDbIProcedure() {};
    virtual ~CVNetDbIProcedure() {};
    virtual int32 Handle(DB_PROC_RTN_TYPES type,
        const char * fun_name,
        vector<CADOParameter> & inParam,
        CVNetDbIProcedureCallback * cb, 
        int32 callback_type,
        void *cb_arg) =0;
    
};

class CVNetDbProcedurePostgresql:public CVNetDbIProcedure
{
public:
    virtual int32 Handle(DB_PROC_RTN_TYPES type,const char * fun_name,
        vector<CADOParameter> & inParam,CVNetDbIProcedureCallback * cb,int32 callback_type, void *cb_arg);
private:
    virtual int32 HandleRecord(const char * fun_name,
        vector<CADOParameter> & inParam,CVNetDbIProcedureCallback* cb, int32 callback_type,void *cb_arg);
    virtual int32 HandleCursor(const char * fun_name,
        vector<CADOParameter> & inParam,CVNetDbIProcedureCallback* cb,int32 callback_type, void *cb_arg);
};

class CVNetDbProcedureFactory
{
public:
    explicit CVNetDbProcedureFactory();
    virtual ~CVNetDbProcedureFactory();
    static CVNetDbProcedureFactory *GetInstance()
    {
        if(NULL == s_instance)
        {
            s_instance = new CVNetDbProcedureFactory(); 
            /*if( VNET_DB_SUCCESS != Init())
            {
                if( s_instance )
                {
                    delete s_instance;
                    s_instance = NULL;
                }
            }*/
        }        
        return s_instance;
    };
    CVNetDbIProcedure * GetIProcedure()
    {
        return m_pDbIProc;
    }
    
    int32 Init();
private: 
    DISALLOW_COPY_AND_ASSIGN(CVNetDbProcedureFactory);
    
    static CVNetDbProcedureFactory *s_instance; 
    CVNetDbIProcedure * m_pDbIProc;
};
} // namespace zte_tecs

#endif // _VNET_DB_PROC_INCLUDE_H_ 

