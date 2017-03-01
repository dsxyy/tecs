/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ssh_trust_main.cpp
* �ļ���ʶ��
* ����ժҪ��ssh ��֤���̵�main����
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��7��26��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
******************************************************************************/
#include "tecs_config.h"
#include "log_agent.h"
#include "ssh_trust.h"

using namespace zte_tecs;
//namespace zte_tecs
//{
#define VALID_CHAR(c)   \
    ((((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'z')) || \
     (((c) >= 'A') && ((c) <= 'Z')) || ((c) == '_') || ((c) == '-'))

static const char *filename_without_path(const char *path)
{
    if (path == NULL)
    {
        return NULL;
    }
    
    /* �ҵ�б�ܳ��ֵ����λ�ã�����Ҳ����򷵻�NULL */
    const char *slash = strrchr(path, '/');

    if (!slash || (slash == path && !slash[1]))
    {
        return (char*)path;
    }
    return slash + 1;
}

static string get_proc_name ()
{
    TecsConfig *config = TecsConfig::Get();

    stringstream nstream;
    int instance = config->GetBackupInst();
    //ʵ���ţ��ڲ���������ʱΪ��������0���
    if (DEFAULT_MS_INST_NOT_CFG == config->GetBackupInst())
    {
        nstream << filename_without_path(config->get_exe_name().c_str());
    }
    else
    {
        nstream << filename_without_path(config->get_exe_name().c_str())<< "_" << instance;
    }    

    string name;
    name = nstream.str();
    int   size   =   name.size();
    //ȥ��"."���ţ�����"_"��������������У��Ϸ���ʧ��    
    for(int pos=0; pos < size; ++pos) 
    {
        if (VALID_CHAR(name.at(pos)))
        {
            continue;
        }
        name.replace(pos,1,"_");
    }
    
    return name;
}

int main(int argc, char** argv)
{    
    STATUS ret;

    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    // ʹ����־��ӡ�������ò���
    EnableLogOptions();

    ret = config->Parse(argc, argv);
    //���������в����Լ������ļ�
    if(SUCCESS == ret)
    {
        if (config->IsHelp())
        {
            return 0;
        }        
    }	
	
    //��ʼ������
    if(ERROR == ret)
    {
        if (!config->IsHelp())
        {
            SkyAssert(0);		  		
            exit(-1);
        }
    }
    else if(ERROR_OPTION_NOT_EXIST == ret)
    {		
        exit(-1);
    }

    //���ý������ƣ������ļ�����ʵ����
    config->SetProcess(get_proc_name().c_str());
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(SShTrust): call tecs_init failed.");
    }

    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "SShTrust call LogInit failed.");
    }
    AddKeyMu(MU_SSH_TRUST);

    // ��������shell
    Shell("tecs_sshTrust-> ");

    SShTrust *pst = SShTrust::GetInstance();
    if(!pst)
    {
        SkyAssert(0);
        SkyExit(-1,"SShTrust GetInstance failed!\n");
    }
    ret = pst->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"SShTrust Init failed!\n");
    }
    //�ȴ�SIGINT��SIGTERM�źŵ������˳�
    wait_signal_exit(); 
    return 0;
}
//}
