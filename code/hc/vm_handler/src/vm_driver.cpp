/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_driver.cpp
* �ļ���ʶ��
* ����ժҪ��VmDriver���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixch
* ������ڣ�2011��7��25��
*
*******************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string> 
#include <iostream>
#include <stdio.h>
#include "vm_driver.h"
#include "libvirt_driver.h"
//#include "xen_driver.h"
#include "simulate_driver.h"
#include "kvm_driver.h"


/************************************************************************************
 *                    ���⻯������������                                                    *
 ***********************************************************************************/
class VmDriverFactory
{
public:
    VmDriverFactory(){};
    ~VmDriverFactory(){};

    static VmDriverBase* GetImpl(VmmType flag)
    {
        VmDriverBase* impl = NULL;
        switch (flag)
        {
            case XEN:         /* now xen libvirt use same driver: libvirt */
//                impl = new XenDriver();
//                break;
        case KVM:
            case LIBVIRT:
            impl = new LibvirtDriver(flag);
                break;
        /*case KVM:
		  impl = new KvmDriver();
            break;*/
            case VMWARE:
                break;
            case SIMULATE:
                impl = new SimulateDriver();
                break;
            default:
                break;
        }
        return impl;
    };
};

/*****************************************************************************
 *                               ȫ�ֱ���                                    *
 ****************************************************************************/
VmDriver* VmDriver::instance = NULL;

/*   modify by wangtl  */
void VmDriver::Init()
{
    VmmType hvType = UNKNOWN_DRIVER_TYPE;
    if (_is_init)
    {
        return;
    }

    if (SimulateDriver::SimulateIsOk())
    {
        hvType = SIMULATE;
    }
    else
    {
        hvType = VmDriverBase::GetDriverType();
    }
    if (UNKNOWN_DRIVER_TYPE <= hvType)
    {
        SkyExit(-1,"VmDriver Impl Get fail!\n");
        return;
    }

     _is_init = true;

    impl = VmDriverFactory::GetImpl(hvType);
    if (impl == NULL)
    {
        SkyExit(-1,"VmDriver Impl Get fail!\n");
	 return;
    }
    
    if (SUCCESS != impl->Init())
    {
        SkyExit(-1,"VmDriver Init fail!\n");
	 return;
    }

    return;
}


