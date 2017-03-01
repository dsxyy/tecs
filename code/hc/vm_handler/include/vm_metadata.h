/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_metadata.h
* �ļ���ʶ�� 
* ����ժҪ�������Ԫ���ݷ���ʵ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ��Ӵ�ɽ 
* ������ڣ� 
*******************************************************************************/
#ifndef VM_METADATA_H
#define VM_METADATA_H

#include "vm_messages.h"
#include "vm_instance.h"
#include "log_agent.h"

namespace zte_tecs
{
    /**
    @brief ��������:�ռ������Ԫ������Ϣ����xml��ʽ��֯��

    @li @b ����б�
    @verbatim
    VmInstance *pinstance   ָ��ǰʵ����ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
    ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
    ��
    @endverbatim

    @li @b �ӿ����ƣ��ú����ռ���Ԫ������ͨ��iso���ݸ������ʹ�ã���Ҫ���ݵ���Ϣ�����ڴ˺�������ӡ�
    @li @b ����˵������
    */
    string CreateVmMetadata(VmInstance *pinstance);

}
#endif

