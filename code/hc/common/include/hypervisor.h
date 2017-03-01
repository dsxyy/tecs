/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�hypervisor.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�������⻯��أ�hc����ģ�鹫�õĺ����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��8��10��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/8/10
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HH_HYPERVISOR_H
#define HH_HYPERVISOR_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{
inline bool IsXenKernel()
{
    if (0 != access("/proc/xen", F_OK))
    {
        return false; //��xen�ں�
    }
    
    return true;
}

inline bool IsKvmKernel()
{
    if (0 != access("/dev/kvm", F_OK))
    {
        return false;
    }
    
    return true;
}
} 
#endif 


