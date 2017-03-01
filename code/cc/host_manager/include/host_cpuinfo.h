/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_cpu_info.h
* 文件标识：见配置管理计划书
* 内容摘要：host_cpu_info类的声明文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月21日
*
* 修改记录1：
*     修改日期：2011/8/21
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HM_HOST_CPUINFO_H
#define HM_HOST_CPUINFO_H

#include "callbackable.h"
#include "sqldb.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: 主机类定义
@li @b 其它说明：无
*/
class HostCpuInfo: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    enum
    {
        INVALID_CPU_INFO_ID = -1
    };

    static HostCpuInfo *GetInstance()
    {
        return _instance;
    }

    static HostCpuInfo* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new HostCpuInfo(db); 
        }
        
        return _instance;
    };
    virtual ~HostCpuInfo() { _instance = NULL; }
    
    int64 GetIdByInfo(const map<string, string> &info); 
    int32 GetInfoById(int64 cpu_info_id, map<string, string> &info);
    int64 Insert(const map<string, string> &info);
    int32 Drop(const map<string, string> &info);
    int32 Drop(int64 cpu_info_id);
    int32 Search(vector<int64> &cpu_info_ids, const string &where = "");
    void  Dump(int64 cpu_info_id, string &info);
    void  Print(int64 cpu_info_id);
    bool  IsExist(int64 cpu_id);
    int64 GetCpuBenchByCpuId(const int64 &cpu_info_id);
    int64 UpdateCpubenchByCpuId(const map<string, string> &info,const int64 &cpu_info_id);
    int64 UpdateCpubenchByCpuId(const int64& cpu_bench,
                                const int64& cpu_info_id);
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:    

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* 各成员在数据库表中的列位置 */
    enum ColNames
    {
        C_ID            = 0,
        PROCESSOR_NUM   = 1,
        PHYSICAL_ID_NUM = 2,
        CPU_CORES       = 3,
        VENDOR_ID       = 4,
        CPU_FAMILY      = 5,
        MODEL           = 6,
        MODEL_NAME      = 7,
        STEPPING        = 8,
        CACHE_SIZE      = 9,
        FPU             = 10,
        FPU_EXCEPTION   = 11,
        CPUID_LEVEL     = 12,
        WP              = 13,
        FLAGS           = 14,
        CPU_BENCH       = 15,
        LIMIT           = 16
    };    

    int64               _last_cpu_info_id;
    static HostCpuInfo  *_instance;      
    static const char   *_table;        // 数据库中存放HostCpuInfo对象的表名
    static const char   *_db_names;     // 查找HostCpuInfo资源表时用到的字段名
    static SqlDB        *_db;       

    string WhereByInfo(const map<string, string> &info);
    int32 InitCallback(void *nil, SqlColumn *columns);
    int32 GetIdCallback(void *nil, SqlColumn *columns);    
    int32 GetInfoCallback(void *nil, SqlColumn *columns);
    int32 SearchCallback(void *nil, SqlColumn *columns);

    HostCpuInfo() {}
    HostCpuInfo(SqlDB *pDb);

    DISALLOW_COPY_AND_ASSIGN(HostCpuInfo);
};

}  /* end namespace zte_tecs */

#endif /* end HM_HOST_H */

