/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�proc_admin.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ProcAdmin�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��11��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/11/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HA_PROC_ADMIN_H
#define HA_PROC_ADMIN_H
#include "sky.h"

namespace zte_tecs 
{

class ProcAdmin : public MessageHandlerTpl<MessageUnit>
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ProcAdmin *GetInstance();
    STATUS Init();
    
    virtual ~ProcAdmin() { _instance = NULL; }
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    STATUS DoStartUp();
    void DoRegisterSystem(const MessageFrame &message);

    static ProcAdmin *_instance;  
    ProcAdmin() {}

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(ProcAdmin);   
};

} // namespace zte_tecs
#endif // #ifndef HA_PROC_ADMIN_H

