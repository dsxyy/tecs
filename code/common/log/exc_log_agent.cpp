/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�exc_log_agent.cpp
* �ļ���ʶ��
* ����ժҪ��CC�ϵ��쳣��־����ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ��˺첨
* ������ڣ�2011��9��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/09/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ��˺첨
*     �޸����ݣ�����
*******************************************************************************/
#include "exc_log_agent.h"
#include "log_agent.h"
using namespace zte_tecs;
namespace zte_tecs
{

ExcLogAgent *ExcLogAgent::_instance = NULL;

ExcLogAgent::ExcLogAgent()
{
    _mu = NULL;
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
STATUS ExcLogAgent::StartMu(const string& name)
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

ExcLogAgent::~ExcLogAgent()
{
    delete _mu;
};

STATUS ExcLogAgent::Receive(const MessageFrame& message)
{
    return _mu->Receive(message);
};

void ExcLogAgent::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
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
        //�����յ�����Ϣid���д���
        switch (message.option.id())
        {
        case EV_EXC_LOG:
        {
            SysLogMessage log_msg;
            struct stat file_state;
            int ret;
            log_msg.deserialize(message.data);
            //�ж��쳣��־Ŀ¼�Ƿ���ڣ�����������򴴽�
            string file;
            file = _exc_log_path+"/"+log_msg._host_name+".txt";
            ret  = stat(file.c_str(),&file_state);
            if (ret == 0)
            {
                if (file_state.st_size >=  EXC_LOG_FILE_SIZE)
                {
                    string file_bak;
                    file_bak = file+".bak";
                    unlink(file.c_str());
                    rename(file.c_str(),file.c_str());
                }
            }

            //д���ļ�
            ofstream ofile;
            ofile.open(file.c_str(),ios::app|ios::out); 
            if (!ofile.is_open())
            {
                //ʧ�����˳�
                cerr << "Error opening out file!errno =" << errno << endl;
                return ; 
            }
            ofile << "<" << log_msg._log_level_count << ">" \
                  << log_msg._log_time << " " \
                  << log_msg._pro_name << ": " \
                  << log_msg._log_content<< endl;
            ofile.close();
            break;
        }
        default:
            return;
        }
        break;
    }

    default:
        break;
    }

    return;
}

}


