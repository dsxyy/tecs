/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_metadata.cpp
* 文件标识： 
* 内容摘要：虚拟机元数据指和虚拟机实例相关的一些数据，如vid,vm_name,用户数据等，
*           虚拟机实例根据这些元数据确定自己的功能定位。
* 当前版本：1.0
* 作    者：钟春山 
* 完成日期： 
*******************************************************************************/
#include "vm_metadata.h"

namespace zte_tecs
{

/**
@brief 功能描述:收集虚拟机元数据信息，按xml格式组织。

@li @b 入参列表：
@verbatim
VmInstance *pinstance   指向当前实例的指针
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：该函数收集的元数据是通过iso传递给虚拟机使用，需要传递的信息可以在此函数中添加。
@li @b 其它说明：无
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
    
    //设置版本号 
    xml.Write("version",to_string<uint32>(pinstance->GetVmVersion(),dec));

    //设置虚拟机ID
    xml.Write("vid",to_string<int64>(pinstance->GetInstanceId(),dec));

    if (!pinstance->vm_cfg._nics.empty())
    {
        //收集网口序号和网口信息
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

    //特殊设备
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

