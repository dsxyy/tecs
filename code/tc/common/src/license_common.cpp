/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：license_common.cpp
* 文件标识：见配置管理计划书
* 内容摘要：证书管理器模块
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年10月11日
*
* 修改记录1：
*     修改日期：2012/10/11
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#include "license_common.h"
#include "log_agent.h"
#include "db_config.h"
#include "pool_sql.h"
#include "vm_messages.h"
#include "mid.h"
#include "event.h"

static int license_common_debug_print = 0;  
DEFINE_DEBUG_VAR(license_common_debug_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(license_common_debug_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
        
int64 GetObjectCount(SqlCallbackable& sqlcb, LicensedObject object)
{
    int64 value = 0;
    string table;
    string column;
    string where;
    STATUS ret = ERROR;
    
    switch(object)
    {
        case LICOBJ_CREATED_VMS:
            table = "vmcfg_pool";
            column = "count(*)";
            where = "";
            ret = sqlcb.SelectColumn(table,column,where,value);
            if(SQLDB_ERROR == ret)
            {
                OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
                    table.c_str(),column.c_str(),where.c_str());
                return -1;
            }
            return value;
            break;

        case LICOBJ_DEPLOYED_VMS:
            table = "vmcfg_pool";
            column = "count(*)";
            where = "deployed_cluster IS NOT NULL";
            ret = sqlcb.SelectColumn(table,column,where,value);
            if(SQLDB_ERROR == ret)
            {
                OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
                    table.c_str(),column.c_str(),where.c_str());
                return -1;
            }
            return value;
            break;

        case LICOBJ_IMAGES:
            table = "image_pool";
            column = "count(*)";
            where = "";
            ret = sqlcb.SelectColumn(table,column,where,value);
            if(SQLDB_ERROR == ret)
            {
                OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
                    table.c_str(),column.c_str(),where.c_str());
                return -1;
            }
            return value;
            break;

        case LICOBJ_USERS:
            table = "user_pool";
            column = "count(*)";
            where = "";
            ret = sqlcb.SelectColumn(table,column,where,value);
            if(SQLDB_ERROR == ret)
            {
                OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
                    table.c_str(),column.c_str(),where.c_str());
                return -1;
            }
            return value;
            break;

        case LICOBJ_CLUSTERS:
            table = "cluster_pool";
            column = "count(*)";
            where = "";
            ret = sqlcb.SelectColumn(table,column,where,value);
            if(SQLDB_ERROR == ret)
            {
                OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
                    table.c_str(),column.c_str(),where.c_str());
                return -1;
            }
            return value;
            break;

        default:
            SkyAssert(false);
            return -1;
            break;
    }
    
    SkyAssert(false);
    return -1;
}

STATUS GetRunningStat(RunningStat& stat)
{
    SqlCallbackable sqlcb(GetDB());
    int64 value = 0;

    value = GetObjectCount(sqlcb,LICOBJ_CREATED_VMS);
    if(value < 0)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    stat.created_vms = value;

    value = GetObjectCount(sqlcb,LICOBJ_DEPLOYED_VMS);
    if(value < 0)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    stat.deployed_vms = value;

    value = GetObjectCount(sqlcb,LICOBJ_IMAGES);
    if(value < 0)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    stat.images = value;

    value = GetObjectCount(sqlcb,LICOBJ_USERS);
    if(value < 0)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    stat.users = value;

    value = GetObjectCount(sqlcb,LICOBJ_CLUSTERS);
    if(value < 0)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    stat.clusters = value;
    return SUCCESS;
}

void StopAllVM()
{
    SqlCallbackable sqlcb(GetDB());
    vector<int64> vids;
    string table("vmcfg_pool");
    string column("oid");
    string where("deployed_cluster IS NOT NULL");
    STATUS ret = sqlcb.SelectColumn(table,column,where,vids);
    if(SQLDB_ERROR == ret)
    {
        OutPut(SYS_ERROR,"failed to select table %s column %s with where: %s!\n",
            table.c_str(),column.c_str(),where.c_str());
        return;
    }

    if(vids.empty())
    {
        OutPut(SYS_DEBUG,"No vm to stop!\n");
        return;
    }

    MessageUnit temp_mu(TempUnitName("LicenseManagerStopVm"));
    temp_mu.Register();

    column = "deployed_cluster";
    vector<int64>::iterator it;
    for(it = vids.begin(); it != vids.end(); it ++)
    {
        OutPut(SYS_DEBUG,"stopping vm %lld ...\n",*it);
        
        where  = " oid = " + to_string(*it,dec);
        string  cluster;
        ret = sqlcb.SelectColumn(table,column,where,cluster);
        if((SQLDB_OK != ret)
            ||cluster.empty())
        {
            continue;
        }
        /* 3. 下发消息给CC */
        VmOperationReq  req_msg(*it,static_cast<int>(STOP), temp_mu.GetSelfMID());    
        MID             req_mid(cluster, PROC_CC, MU_VM_MANAGER);
        MessageOption   req_option(req_mid, EV_VM_OP, 0,0);
        
        temp_mu.Send(req_msg, req_option);
    }
}

int LicenseRemainingDays()
{
    return license_days_remained();
}

int ValidateLicense(LicensedObject object, int64 increment)
{
    RunningStat stat;
    SqlCallbackable sqlcb(GetDB());
    int64 count = GetObjectCount(sqlcb,object);
    if(count < 0)
    {
        return LICENSE_FAIL;
    }

    switch(object)
    {
        case LICOBJ_CREATED_VMS:
            stat.created_vms += count + increment;
            break;
            
        case LICOBJ_DEPLOYED_VMS:
            stat.deployed_vms += count + increment;
            break;

        case LICOBJ_IMAGES:
            stat.images += count + increment;
            break;
            
        case LICOBJ_USERS:
            stat.users += count + increment;
            break;

        case LICOBJ_CLUSTERS:
            stat.clusters += count + increment;
            break;
            
        default:
            SkyAssert(false);
            return LICENSE_FAIL;
    }

    ostringstream error;
    return license_valid(stat,error);
}

void DbgValidateLicense(int object, int64 increment)
{
    int ret = ValidateLicense((LicensedObject)object,increment);
    if(LICENSE_OK == ret)
    {
        printf("license is ok!\n");
    }
    else
    {
        printf("license will be invalid!!! ret = %d\n",ret);
    }   
}
DEFINE_DEBUG_FUNC(DbgValidateLicense);

void DbgShowRunningStat()
{
    RunningStat stat;
    if(GetRunningStat(stat) != SUCCESS)
    {
        OutPut(SYS_EMERGENCY,"failed to get running stat!\n");
        SkyExit(-1,"failed to get running stat!\n");
    }
    cout << "created_vms: " << stat.created_vms << endl;
    cout << "deployed_vms: " << stat.deployed_vms << endl;
    cout << "users: " << stat.users << endl;
    cout << "clusters: " << stat.clusters << endl;
    cout << "images: " << stat.images << endl;
}
DEFINE_DEBUG_FUNC(DbgShowRunningStat);

