/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostManager��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ��뿥
* ������ڣ�2012��9��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/9/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "tecs_config.h"
#include "alarm_agent.h"
#include "storage_adaptor_impl.h"
#include "storage_cli.h"
namespace zte_tecs
{

#define TIMER_GET_SAINFO            EV_TIMER_1

static string storage_cfg_file;

STATUS EnableSaStoreOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("sa_cfg_file",storage_cfg_file,
                         "storage adaptor config file path.");
    return SUCCESS;
}

StorageAdaptorInitiator *StorageAdaptorInitiator::_instance = NULL;

/******************************************************************************/
bool StorageAdaptorInitiator::CheckCfg(sa_cfg_array &cfg_array)
{
    vector<sa_cfg>::iterator it;
    it=cfg_array._cfg_array.begin();
    int  count=0;
    string name;
    map<string,int> namemap;
    map<string,int>::iterator iter; 
    map<string,int> controlmap;
   
    for (;it!=cfg_array._cfg_array.end();it++)
    {
        if(it->_type==LOCALSAN)
        {
            count++;
        }          
        iter= namemap.find(it->_name);
        if( iter != namemap.end() ) 
        {
            OutPut(SYS_DEBUG, "cfg name repeated\n");
            return false;
        }
        namemap[it->_name]++;
        
        iter= controlmap.find(it->_control_ip);
        if( iter != controlmap.end() ) 
        {
            OutPut(SYS_DEBUG, "control ip repeated\n");
            return false;
        }
        controlmap[it->_control_ip]++;
        
    }  
    if(count > 1)
    {
        OutPut(SYS_DEBUG, "local san num should <=1\n");
        return false;
    }
    else
    {
        return true;
    }
}

STATUS StorageAdaptorInitiator::Init()
{
    string         tmp;
    sa_cfg_array   cfg_array;
    sa_cfg         cfg;
    int            ret;
    bool           bRet;

    //step1  ���������ļ���������storage_cfg����
    ifstream fin(storage_cfg_file.c_str(),ios_base::in);
    if (!fin)
    {
        OutPut(SYS_ERROR,"open sacfg file err\n");
        return ERROR;
    }

    getline(fin,tmp,'\0');
    fin.close();
    cfg_array.deserialize(tmp);
    if (cfg_array._cfg_array.size()==0)
    {
        OutPut(SYS_ERROR,"SA don't get valid config and can't work correctly !!!\n");
        OutPut(SYS_ERROR,"You should change /opt/tecs/sa/etc/sa.xml according your requirements \n");
        OutPut(SYS_ERROR,"You can use the command of DbgWriteSacfg(\"/home/test.xml\") to see the cfg example \n");
        return ERROR;
    }

    //setp2  У������
    bRet=CheckCfg(cfg_array);
    if(!bRet)
    {
        OutPut(SYS_ERROR,"CheckCfg  err\n");
        return ERROR;
    }

    vector<sa_cfg>::iterator it;
    it=cfg_array._cfg_array.begin();
    for (;it!=cfg_array._cfg_array.end();it++)
    {
        cfg=*it;
        _sa_cfg.push_back(cfg);
    }  
    
    //setp3  ���� adaptor
    Storage_Adaptor *pAdaptor;
    vector<sa_cfg>::iterator itcfg;   
    for (itcfg = _sa_cfg.begin(); itcfg!= _sa_cfg.end(); itcfg++)
    {
        pAdaptor=new Storage_Adaptor(*itcfg);
        if (pAdaptor==NULL)
        {
            OutPut(SYS_ERROR, "Create Storage_Adaptor %s failed!\n",itcfg->_name.c_str());
            return ERROR_NO_MEMORY;
        }

        ret=pAdaptor->Init();
        if(ret!=SUCCESS)
        {
            SkyAssert(0);
            OutPut(SYS_ERROR, "StorageAdaptorInitiator: Storage_Adaptor init failed. ret:%d\n",ret);
            return ret;
        }
        _adaptor_array.push_back(pAdaptor);        
    }
    return SUCCESS;
}

void DbgWriteSacfg(char *file)
{
    string         tmp;
    string         data_ip;
    sa_cfg_array cfg_array;
    sa_cfg         cfg;
    cfg._type="LOCALSAN";
    cfg._control_ip="10.1.1.1";
    cfg._description="test1";
    cfg._name="disk_localsan";
    data_ip="20.1.1.1";
    cfg._data_ip.push_back(data_ip);
    data_ip="30.1.1.1";
    cfg._data_ip.push_back(data_ip);
    data_ip="40.1.1.1";
    cfg._data_ip.push_back(data_ip);

    cfg_array._cfg_array.push_back(cfg);
    cfg._data_ip.clear();
    cfg._type="IPSAN";
    cfg._control_ip="10.1.1.2";
    cfg._user="admin";
    cfg._password="tecs123";
    cfg._name="disk_ipsan1";
    cfg._description="test2";
    cfg_array._cfg_array.push_back(cfg);

    cfg._type="IPSAN";
    cfg._control_ip="10.1.1.3";
    cfg._user="admin";
    cfg._password="tecs123";
    cfg._name="disk_ipsan2";
    cfg._description="test3";
    cfg_array._cfg_array.push_back(cfg);


    tmp=cfg_array.serialize();
    if(file==NULL)
    {
        printf("pls input the filename(including path) \n");
    }
    ofstream fout(file,ios_base::out);
    if (!fout.is_open())
    {
        printf("not open\n");
        return ;
    }

    fout <<tmp;
    fout.close();
    printf("generate  %s OK\n",file);
}
DEFINE_DEBUG_FUNC(DbgWriteSacfg);

void DbgShowSacfg()
{
    string         tmp;
    sa_cfg_array   cfg_array;
    sa_cfg         cfg;
    ifstream fin(storage_cfg_file.c_str(),ios_base::in);
    if (!fin)
    {
        printf("open sacfg file err\n");
        return ;
    }

    getline(fin,tmp,'\0');
    printf("cfg is below----------------\n");
    printf("%s",tmp.c_str());
    fin.close();
}
DEFINE_DEBUG_FUNC(DbgShowSacfg);

}

