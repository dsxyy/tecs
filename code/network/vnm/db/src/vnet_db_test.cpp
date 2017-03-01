
#include "vnet_db_in.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane.h"
//#include "stdio.h"
#if 0
#include "sqldb.h"
#include "postgresql_db.h"
#include "sqlite_db.h"
#include "sample_type.h"
#endif 
extern CADODatabase *GetAdoDB();

#include "db_config.h"
//extern STATUS DBInitAdo(const DBConfig&) ;


namespace zte_tecs
{

extern CADODatabase *g_pDb;

void db_query_netplanesummary()
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
    if( NULL == pNetplane )
    {
        cout << "CDBOperateNetplane is NULL" << endl;
        return ;
    }

    vector<CDBObjectBase> outVInfo;
    int ret = pNetplane->QueryNetplaneSummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QueryNetplaneSummary failed." << ret << endl;
        return;
    }
    cout << "QueryNetplaneSummary success." << endl;
    vector<CDBObjectBase>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_netplanesummary);

void db_query_netplane(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
    if( NULL == pNetplane )
    {
        cout << "CDBOperateNetplane is NULL" << endl;
        return ;
    }

    CDbNetplane info;
    info.SetUuid(uuid);
    int ret = pNetplane->QueryNetplane(info);
    if( ret != 0 )
    {
        cout << "QueryNetplane failed." << ret << endl;
        return;
    }
    cout << "QueryNetplane success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_netplane);

void db_add_netplane(char* name, char *desc,int32 mtu)
{
    if( NULL == name || NULL == desc )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
    if( NULL == pNetplane )
    {
        cout << "CDBOperateNetplane is NULL" << endl;
        return ;
    }

    CDbNetplane info;
    info.SetName(name);
    info.SetDesc(desc);
    info.SetMtu(mtu);
    int ret = pNetplane->AddNetplane(info);
    if( ret != 0 )
    {
        cout << "AddNetplane failed." << ret << endl;
        return;
    }
    cout << "AddNetplane success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_netplane);

void db_mod_netplane(char * uuid,char* name, char *desc,int32 mtu)
{
    if(uuid == NULL || NULL == name || NULL == desc )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
    if( NULL == pNetplane )
    {
        cout << "CDBOperateNetplane is NULL" << endl;
        return ;
    }

    CDbNetplane info;
    info.SetUuid(uuid);
    info.SetName(name);
    info.SetDesc(desc);
    info.SetMtu(mtu);
    int ret = pNetplane->ModifyNetplane(info);
    if( ret != 0 )
    {
        cout << "ModifyNetplane failed." << ret << endl;
        return;
    }
    cout << "ModifyNetplane success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_netplane);

void db_del_netplane(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateNetplane * pNetplane = pMgr->GetINetplane();
    if( NULL == pNetplane )
    {
        cout << "CDBOperateNetplane is NULL" << endl;
        return ;
    }

    int ret = pNetplane->DelNetplane(uuid);
    if( ret != 0 )
    {
        cout << "DelNetplane failed." << ret << endl;
        return;
    }
    cout << "DelNetplane success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_netplane);




extern CADODatabase * GetVnmDb();
void simple_db()
{
#if 0
    CADODatabase db("",0,"postgresql","password","test");        
    if ( !db.Open() )    
    {        
        printf("open database failed\n");        
        exit(-1);    
    }    
#endif     
    /* сн╠Й */    
    CADOParameter pParamIn1(CADOParameter::paramInt, CADOParameter::paramInput, 1);
    printf("============\n");    
    //CADOCommand pCmd4(&db, "reffunc", CADOCommand::typeCmdStoredProcCursor);    
    //CADOCommand pCmd4(g_pDb, "reffunc", CADOCommand::typeCmdStoredProcCursor);        
    //pCmd4.AddParameter(&pParamIn1);    

    //cout << "npsum" << endl;
    CADOCommand pCmd4(GetVnmDb(), "np", CADOCommand::typeCmdStoredProcCursor);        
    
    pCmd4.AddParameter(&pParamIn1);  
    
    //CADORecordset* prs4 = new CADORecordset(&db);    
    CADORecordset* prs4 = new CADORecordset(GetVnmDb());         
    if(prs4->Execute(&pCmd4))    
    {           
        printf("success.g_pDb\n");
        while ( !prs4->IsEOF() )        
        {        
            printf("a:%s  b:%s c:%s d:%s e:%s\n", 
                prs4->GetFieldValue(0),              
                prs4->GetFieldValue(1),             
                prs4->GetFieldValue(2),             
                prs4->GetFieldValue(3),             
                prs4->GetFieldValue(4));                 
            prs4->MoveNext();           
        }    
    }    
    else
    {
        printf("failed.\n");
    }
    delete prs4;         
    printf("============\n");
}DEFINE_DEBUG_FUNC(simple_db);

#include "db_config.h"
void ado_db(char* url, char * db_name, int32 port, char* user, char * pwd)
{
    DBConfig config;
    config.server_url = url;
    config.server_port = port;
    config.user = user;
    config.passwd = pwd; 
    config.database = db_name;        
    
    int32 ret = DBInitAdo(config);
    if( ret != 0 )
    {
        cout << "DbInitAdo failed." << ret << endl;
    }
    else
    {
        cout << "DbInitAdo success" << endl;
        // cout << "ado db : " <<  GetAdoDB(pthread_t t) << endl;
    }

}DEFINE_DEBUG_FUNC(ado_db);

}




