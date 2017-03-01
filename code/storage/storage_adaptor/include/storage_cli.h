/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�storage_cli.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��
* ��ǰ�汾��1.0
* ��    �ߣ��뿥
* ������ڣ�2012��9��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��9��19��
*     �� �� �ţ�V1.0
*     �� �� �ˣ��뿥
*     �޸����ݣ�����
*******************************************************************************/
#ifndef STORAGE_ADAPTOR_INITIATOR_H
#define STORAGE_ADAPTOR_INITIATOR_H
#include "sky.h"
#include "mid.h"
#include "log_agent.h"
#include "storage_adaptor.h"

namespace zte_tecs 
{

class StorageAdaptorInitiator 
{
public:
    static StorageAdaptorInitiator *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new StorageAdaptorInitiator();
        }
        return _instance;
    }

    STATUS Init();       
    ~StorageAdaptorInitiator() 
    { 
        _instance = NULL; 
    }
    StorageAdaptorInitiator(){};  
    bool  CheckCfg(sa_cfg_array &cfg_array);
    
private:   
    bool            _have_startup;  // �Ƿ��������ı�־���������³�ʼ��
    string          _sc_name;       // ��SA������tecsϵͳ����    
    vector<sa_cfg>  _sa_cfg;
    TIMER_ID        _timer;    
    vector<Storage_Adaptor *>       _adaptor_array;

    static StorageAdaptorInitiator  *_instance; 
 
    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorInitiator);   
};

STATUS EnableSaStoreOptions();
}

#endif // #ifndef HM_HOST_MANAGER_H

