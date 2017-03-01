/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�image_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CoredumpFileManager���ʵ���ļ�
* ��ǰ�汾��2.0
* ��    �ߣ�lichun
* ������ڣ�2012��10��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/10/19
*    �� �� �ţ�V1.0
*    �� �� �ˣ�lichun
*    �޸����ݣ���image_pool��Ϊ�ǻ���ģʽ
*******************************************************************************/
#include "sky.h"
#include "file_manager.h"
#include "tecs_config.h"
#include "workflow.h"
#include "log_agent.h"

// ����coredump�ļ�����λ��
static string  coredump_save_dir = "/var/lib/tecs/coredump/";
static string  file_manager_server_url = "";
static string nfs_server_ip_temp = "";

namespace zte_tecs
{

STATUS EnableCoredumpFileOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("coredump_save_dir", coredump_save_dir, "save vm coredump file dir when vm crash,default:/var/lib/tecs/coredump/.");
    config->EnableCustom("file_manager_server_url", file_manager_server_url, "server addr for save vm coredump file , eg:129.x.x.x or dns url");
    config->EnableCustom("nfs_server_url", nfs_server_ip_temp,"Image manager's nfs server url for clusters and hosts to download image.");
    return SUCCESS;
}

FileManager* FileManager::instance = NULL;
/******************************************************************************/
/* FileManager :: Constructor/Destructor                                   */
/******************************************************************************/
FileManager::FileManager()
{
    _mu = NULL;
    _timer_id = INVALID_TIMER_ID;
};

/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��  
* �� �� ֵ�� ��
* ����˵���� 
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS FileManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0); 
        return ret;
    }
    
    _mu->Print();
    return SUCCESS;
}

/******************************************************************************/
void FileManager:: MessageEntry(const MessageFrame &message)
{
    switch (_mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
            case EV_POWER_ON:
            {
                if (INVALID_TIMER_ID == (_timer_id = _mu->CreateTimer()))
                {
                    SkyAssert(false);
                    OutPut(SYS_ERROR, "%s power on failed for createtimer!\n",_mu->name().c_str());
                }
                TimeOut timeout;  
                timeout.type     = LOOP_TIMER;
                timeout.duration = 30*1000; // 30s����һ��ɨ��
                timeout.msgid    = EV_FILE_SCAN_TIMER;
                if (SUCCESS != _mu->SetTimer(_timer_id,timeout))
                {
                    SkyAssert(false);
                    OutPut(SYS_ERROR, "%s power on failed for SetTimer!\n",_mu->name().c_str());
                }

                // ����û�û�������ļ������������ַ�������image��ַ
                if (file_manager_server_url == "")
                {
                    file_manager_server_url = nfs_server_ip_temp;
                }

                // ��ַ�ɿ��Լ��
                if (file_manager_server_url == "")
                {
                    SkyExit(-1, "FileManager::MessageEntry:file_manager_server_url  need config.");
                }

                // ���ݶ�ȡ�����ã��������ļ���
                if (!Mkdir(coredump_save_dir))
                {
                    SkyExit(-1, "FileManager::MessageEntry: mkdir save coredump dir fail.");
                }

                // ���ݶ�ȡ�����ã�����nfs��·��
                ExportDirToNfs(coredump_save_dir);

                // ����nfs����
                RestartNfs();
                
                _mu->set_state(S_Work);
                OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
                break;
            }
            default:
                break;
            }
            break;
        }

        case S_Work:
        {
            switch (message.option.id())
            {
                case EV_FILE_GET_URL_REQ:
                {
                    WorkReq recv_msg;
                    recv_msg.deserialize(message.data);
                    SendGetUrlAck(message.option.sender(), recv_msg.action_id);    
                    break;
                }
                case EV_FILE_SCAN_TIMER:
                {
                    // ִ��coredumpѹ������ű�
                    TecsConfig *config = TecsConfig::Get();
                    string cmd = config->get_scripts_path() + "/cordump_file_zip.sh " + coredump_save_dir;
                    system(cmd.c_str());
                    break;
                }
                    
                default:
                    OutPut(SYS_WARNING,"FileManager rcv unexpected meg !, id = %u\n",message.option.id() );
                    break;
            }
            break;
        }

        default:
            break;
    }
}

/******************************************************************************/
bool FileManager:: Mkdir(const string &path)
{
    string cmd="mkdir -p " + path;

    if (0 != system(cmd.c_str()))
    {
        OutPut(SYS_ALERT, "FileManager::mkdir %s fail! ", path.c_str());
        return false;
    }
    else
    {
        return true;
    } 
}

/******************************************************************************/
bool FileManager:: ExportDirToNfs(const string &path)
{
    string cmd="[ `cat /etc/exports | grep -c " + path + "` -eq 0 ] && echo \"" + path + " *(rw,no_root_squash) \">> /etc/exports";

    system(cmd.c_str());
    return true;
}

/******************************************************************************/
void FileManager:: RestartNfs()
{
    string cmd="service nfs restart";

    if (0 != system(cmd.c_str()))
    {
        OutPut(SYS_WARNING, "FileManager::RestartNfs %s fail! ");
    }
}

/******************************************************************************/
void FileManager:: SendGetUrlAck(const MID &receiver, const string &action_id)
{
    UploadUrl AckMsg;

    if (NULL == _mu)
    {
        OutPut(SYS_ALERT, 
               "In SendGetUrlAck, send ack fail, mu is null\n");
        SkyAssert(false);
        return;
    }

    AckMsg.ip = file_manager_server_url;
    AckMsg.path = coredump_save_dir;
    AckMsg.access_type = "nfs";
    AckMsg.action_id = action_id;
    AckMsg.state = SUCCESS;
    AckMsg.progress = 100;

    MessageOption option(receiver, EV_FILE_GET_URL_ACK, 0, 0);
    STATUS ret = _mu->Send(AckMsg, option);

    if(SUCCESS != ret)
    {
        OutPut(SYS_ERROR, 
               "In SendGetUrlAck, send ack fail, return: %d ", ret);
    }
    else
    {
        OutPut(SYS_INFORMATIONAL, "In SendGetUrlAck, send ack success \n");
    }
}

}


