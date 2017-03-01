/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_metadata.cpp
* �ļ���ʶ�� 
* ����ժҪ�������Ԫ����ָ�������ʵ����ص�һЩ���ݣ���vid,vm_name,�û����ݵȣ�
*           �����ʵ��������ЩԪ����ȷ���Լ��Ĺ��ܶ�λ��
* ��ǰ�汾��1.0
* ��    �ߣ��Ӵ�ɽ 
* ������ڣ� 
*******************************************************************************/
#include "vm_metadata.h"

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
string CreateVmMetadata(VmInstance *pinstance)
{
    XmlParser xml;
    string    filecontext("");
    string     vmversion;

    if((NULL == pinstance))
    {
        OutPut(SYS_DEBUG, "CreateVmMetadata: pinstance is null!\n");
        return filecontext;
    }
    
    filecontext = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    
    xml.Create("content");
    
    //���ð汾�� 
    xml.Write("version",to_string<uint32>(pinstance->GetVmVersion(),dec));

    //���������ID
    xml.Write("vid",to_string<int64>(pinstance->GetInstanceId(),dec));

    if (!pinstance->vm_cfg._nics.empty())
    {
        //�ռ�������ź�������Ϣ
        xml.Write("network", "");
        xml.Enter();
        xml.Write("interfaces", "");
        xml.Enter();
        vector<VmNicConfig>::iterator itr = pinstance->vm_cfg._nics.begin();
        for(; itr != pinstance->vm_cfg._nics.end(); ++itr)
        {
            xml.Write("item", ""); 
            xml.Enter(); 
            xml.Write("index", to_string<uint32>(itr->_nic_index,dec)); 
            xml.Write("mac", itr->_mac);
            xml.Write("ipaddr", itr->_ip);
            xml.Write("netmask", itr->_netmask);
            xml.Write("gateway", itr->_gateway);
            xml.Exit();
        }
        xml.Exit();
        xml.Exit();
    }

    //�����豸
    xml.Write("special_device", "");
    xml.Enter();
    vector<VmDeviceConfig>::iterator device;
    string is_used_pdh = "0";
    for (device = pinstance->vm_cfg._devices.begin(); device != pinstance->vm_cfg._devices.end(); device++ )
    {
        if (1 == device->_type)
        {
            is_used_pdh = "1";
            break;
        }
    }
    xml.Write("is_used_pdh", is_used_pdh);
    xml.Exit();

    filecontext += xml.DocToString();

    return filecontext;
}

}//namespace zte_tecs

