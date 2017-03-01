/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host_cpu_info.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��host_cpu_info��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_HOST_CPUINFO_H
#define HM_HOST_CPUINFO_H

#include "callbackable.h"
#include "sqldb.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

/**
@brief ��������: �����ඨ��
@li @b ����˵������
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
    /* ����Ա�����ݿ���е���λ�� */
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
    static const char   *_table;        // ���ݿ��д��HostCpuInfo����ı���
    static const char   *_db_names;     // ����HostCpuInfo��Դ��ʱ�õ����ֶ���
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

