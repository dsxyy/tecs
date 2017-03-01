/*********************************************************************
* 版权所有 (C)2007, 深圳市中兴通讯股份有限公司。
*
* 文件名称: pci_passthrought.cpp
* 文件标识:
* 内容摘要: pci透传设备管理
* 其它说明:
* 当前版本: V1.0
* 作    者: 姚远 ——3G平台
* 完成日期:
*
* 修改记录1:
*     修改日期: 2012-12-06
*     版 本 号: V1.0
*     修 改 人: 姚远
*     修改内容: 创建
*
**********************************************************************/
#include "pci_passthrought.h"
#include "tecs_config.h"

namespace zte_tecs
{

/* 申请占用指定pci设备 */
STATUS PCIPassthrough::Request(int64 vid, int type)
{
    STATUS ret = ERROR;

    if (type == _type)
    {
        if ((_vid == 0) || (_vid == vid))
        {
            ret = SUCCESS;
            _vid = vid;
            //todo:back操作在脚本被调用的时候自动执行
        }
    }

    return ret;
}

/* 释放占用的pci设备 */
void PCIPassthrough::Release(int64 vid)
{
    if (_vid == vid)
    {
        _vid = 0;
        //todo: 是否需要对pci做释放操作，未知
    }
}


/******************************************************************************************/
const char *pci_xml_path = "/var/run/tecs/pci_passthrought.xml";
//const char *pci_sh = "/opt/tecs/hc/scripts/host_pci.sh";

PCIPthManager *PCIPthManager::_instance = NULL;

void PCIPthManager::Startup()
{
    ReadPciDevices();
}

/* 从pci_passthrought.xml读pci设备表 */
void PCIPthManager::ReadPciDevices()
{
    //检查文件是否存在，第一次上电文件没创建，不能返回失败
    if(0 != access(pci_xml_path, 0))
    {
        TecsConfig *config = TecsConfig::Get();
        string pci_sh = config->get_scripts_path() + "/host_pci.sh";
        system(pci_sh.c_str());
    }

    string pci_xml;
    ifstream fin(pci_xml_path);
    string line;
    while (getline(fin, line))
    {
        pci_xml += line + "\n";
    }

    if ((!pci_xml.empty()) && (!deserialize(pci_xml)))
    {//为空会导致解序列化失败
        cout << "PCIPthManager deserialize failed!\n";
        SkyAssert(false);
        SkyExit(-1, "deserialize pci_passthrought.xml failed.");
    }
}

/* 将pci设备表的使用情况保存入pci_passthrought.xml */
void PCIPthManager::SavePciDevices()
{
    string pci_xml = serialize();
    ofstream fout(pci_xml_path);
    fout<< pci_xml<< endl;
}

/* 提供给虚拟机，申请使用某类资源 */
const vector<PCIPassthrough> &PCIPthManager::Request(int64 vid, const VmDeviceConfig &vm_device, vector<PCIPassthrough> &pci_devices)
{
    int need_apply = vm_device._count;
    SkyAssert(need_apply > 0);

    vector<PCIPassthrough>::iterator it;
    for (it = _devices.begin(); it != _devices.end(), need_apply > 0; it ++)
    {
        if (SUCCESS == it->Request(vid, vm_device._type))
        {
            need_apply --;
            pci_devices.push_back(*it);
        }
    }

    SkyAssert(need_apply == 0);

    return pci_devices;
}

const vector<PCIPassthrough> &PCIPthManager::Request(int64 vid, const vector<VmDeviceConfig> &vm_devices, vector<PCIPassthrough> &pci_devices)
{
    vector<VmDeviceConfig>::const_iterator it;
    for (it = vm_devices.begin(); it != vm_devices.end(); it ++)
    {
        Request(vid, *it, pci_devices);
    }

    SavePciDevices();   //保存入xml文件

    return pci_devices;
}

void PCIPthManager::Release(int64 vid)
{
    vector<PCIPassthrough>::iterator it;
    for (it = _devices.begin(); it != _devices.end(); it ++)
    {
        it->Release(vid);
    }

    SavePciDevices();   //保存入xml文件
}

/* 整理出hc上的特殊设备信息，以便上报给cc */
class CountSDevice {
public:
    CountSDevice(int type) : _type(type) {}
    bool operator()(const PCIPassthrough &pth)
    {
        return pth._type == _type;
    }
    
    int _type;
};

void PCIPthManager::ReportInfo(vector<PciPthDevInfo> &sdevices)
{
    vector<PCIPassthrough> all_sdevices(_devices);
    while (!all_sdevices.empty())
    {
        vector<PCIPassthrough>::iterator it = remove_if(all_sdevices.begin(), all_sdevices.end(), CountSDevice(all_sdevices[0]._type));
        sdevices.push_back(PciPthDevInfo(all_sdevices[0]._type, all_sdevices.end() - it, "PCIPassthrough"));
        all_sdevices.erase(it, all_sdevices.end());
    }
}

void PCIPthManager::Test()
{
    PCIPassthrough dev1;
    dev1._type = 1;
    dev1._vid = 2;
    PciBus bus1;
    bus1._domain = "0x0000";
    bus1._bus = "0x88";
    bus1._slot = "0x00";
    bus1._function = "0x0";
    PciBus bus2 = bus1;
    bus1._bus = "86";
    dev1._pci_buses.push_back(bus1);
    dev1._pci_buses.push_back(bus2);

    PCIPassthrough dev2 = dev1;
    dev2._type = 3;
    dev2._vid = 4;

    SavePciDevices();

    _devices.push_back(dev1);
    //_devices.push_back(dev2);

    SavePciDevices();
}

/******************************************************************************/
/*************************调试函数，待增加，查看pci和子卡的信息****************/
/*******************************************************************************/

void DbgTestPciPassthrought()
{
    PCIPthManager *pci_ma = PCIPthManager::GetInstance();
    pci_ma->Test();
}

DEFINE_DEBUG_FUNC(DbgTestPciPassthrought);

} // namespace zte_tecs

