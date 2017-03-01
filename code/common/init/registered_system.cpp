/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�identity.cpp
* �ļ���ʶ��
* ����ժҪ��GetRegisteredSystem�ӿڵ�ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��9��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/9/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#include "registered_system.h"

static string registered_system_name;

/******************************************************************************/
STATUS GetRegisteredSystem(string &reg_system)
{
    reg_system = registered_system_name;

    if (reg_system.empty())
    {
        return ERROR;
    }
    return SUCCESS;
}

/******************************************************************************/
void SetRegisteredSystem(const string &reg_system)
{
    // �޸ı�
    if (registered_system_name == reg_system)
    {
        return;
    }

    // ���޵���
    if (registered_system_name.empty())
    {
        registered_system_name = reg_system;
        return;
    }

    // ���е���
    if (reg_system.empty())
    {
        exit(-1);
        return;
    }

    // ���е��У��쳣
    SkyAssert(0);
    exit(-1);
    return;
}

/******************************************************************************/
void WaitRegisteredSystem(void)
{
    cerr << "Waite register into reg_system......";
    string reg_system;
    for(;;)
    {
        if (SUCCESS == GetRegisteredSystem(reg_system))
        {
            break;
        }
        else
        {
            sleep(1);
            cerr << ".";
        }
    }
    cerr << endl << "Have registered into reg_system: " 
         << reg_system << endl;
}

void DbgShowRegisteredSystem()
{
    cout << "registered system = " << registered_system_name << endl;
}
DEFINE_DEBUG_CMD(regto,DbgShowRegisteredSystem);


