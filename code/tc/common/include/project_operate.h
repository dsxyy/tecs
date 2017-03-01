/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�project_operate.h
* �ļ���ʶ��
* ����ժҪ��ProjectOperate��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#ifndef PROJECT_OPERATE_H        
#define PROJECT_OPERATE_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "project_pool.h"
#include "project_api.h"


namespace zte_tecs
{


class ProjectOperate
{
public:

    static ProjectOperate *GetInstance( )
    {
        SkyAssert(NULL != _op_inst);   
        SkyAssert(NULL != _ppool); 
        
        return _op_inst;
    };

    static ProjectOperate *CreateInstance()
    {        
        if(NULL == _op_inst)
        {
            _op_inst = new ProjectOperate( );
            _ppool   = ProjectPool::GetInstance();
            SkyAssert(NULL != _ppool);
        }       
        
        return _op_inst;
    };

    ~ProjectOperate(){};

    STATUS DealCreate(const ApiProjectAllocateData  &req, 
                                 int64          uid,
                                 int64         &pid,
                                 ostringstream &err_oss);
    STATUS DealModify(const ApiProjectModifyData  &req, 
                      int64          uid, 
                      ostringstream &oss);

    STATUS DealDelete(int64  op_uid, 
                                      int64  owner, 
                                      const string &project_name, 
                                      ostringstream &err_oss);
                                       
    STATUS DealQuery(const ApiProjectQuery  &req, 
                        int64                    op_uid,
                        int64                    appointed_uid,
                        vector<ApiProjectInfo>  &vec_prj,
                        int64                   &max_count,
                        int64                   &next_index,
                        ostringstream           &err_oss);                       
    
    STATUS DealProjectVmQuery(const ApiProjectVmQuery  &req, 
                        int64                      op_uid,
                        int64                      appointed_uid,
                        vector<ApiProjectVmInfo>  &vec_prj,
                        int64                     &max_count,
                        int64                     &next_index,
                        ostringstream             &err_oss);  
                        
    bool Authorize(int64 opt_uid, Project *buf, int oprate);

    STATUS Str2ProjOp(const string   &action_name,
                          ProjectOperation    &type,
                          ostringstream  &err_oss);

    STATUS DealStatisticsStaticData(const ApiProjectQuery   &req, 
                                    int64                   op_uid,
                                    int64                   appointed_uid,
                                    vector<ApiProjectStatisticsStaticData>  &vec_prj,
                                    int64                   &max_count,
                                    int64                   &next_index,
                                    ostringstream           &err_oss);   
 
    STATUS DealProjectClusterQuery(ApiProjectDataQuery &req,
                                       int64    &uid,                                                  
                                       string  &cluster_name,
                                       int64 &max_count,
                                       int64 &next_index,
                                       ostringstream &err_oss);     

   
                                 
private:
    static  ProjectOperate * _op_inst;
    static  ProjectPool    * _ppool;

    ProjectOperate( ) { };

    DISALLOW_COPY_AND_ASSIGN(ProjectOperate);
 
};



}

#endif /* PROJECT_OPERATE_H */