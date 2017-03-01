/*********************************************************************
* ��Ȩ���� (C)2007, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ�����: pci_passthrought.h
* �ļ���ʶ:
* ����ժҪ: pci͸���豸����
* ����˵��:
* ��ǰ�汾: V1.0
* ��    ��: ҦԶ ����3Gƽ̨
* �������:
*
* �޸ļ�¼1:
*     �޸�����: 2012-12-06
*     �� �� ��: V1.0
*     �� �� ��: ҦԶ
*     �޸�����: ����
*
**********************************************************************/
#ifndef PCI_PASSTHROUGHT_H
#define PCI_PASSTHROUGHT_H
#include "sky.h"
#include "vm_messages.h"
#include "msg_host_manager_with_host_agent.h"

namespace zte_tecs
{

class PCIPassthrough : public Serializable {
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(PCIPassthrough);
        WRITE_VALUE(_type);
        WRITE_VALUE(_vid);
        WRITE_OBJECT_ARRAY(_pci_buses);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(PCIPassthrough);
        READ_VALUE(_type);
        READ_VALUE(_vid);
        READ_OBJECT_ARRAY(_pci_buses);
        DESERIALIZE_END();
    };

    STATUS Request(int64 vid, int type);
    void Release(int64 vid);

    int             _type;
    int64           _vid;
    vector<PciBus>  _pci_buses;
};

class PCIPthManager : public Serializable {
public:
    static PCIPthManager* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new PCIPthManager();
        }

        SkyAssert(_instance);
        return _instance;
    };

    void Startup();
    void Test();
    const vector<PCIPassthrough> &Request(int64 vid, const vector<VmDeviceConfig> &devices, vector<PCIPassthrough> &pci_devices);
    void Release(int64 vid);
    void ReportInfo(vector<PciPthDevInfo> &sdevices);

private:
    const vector<PCIPassthrough> &Request(int64 vid, const VmDeviceConfig &devices, vector<PCIPassthrough> &pci_devices);

    SERIALIZE
    {
        SERIALIZE_BEGIN(PCIPthManager);
        WRITE_OBJECT_ARRAY(_devices);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(PCIPthManager);
        READ_OBJECT_ARRAY(_devices);
        DESERIALIZE_END();
    };

    void ReadPciDevices();
    void SavePciDevices();

    static PCIPthManager       *_instance;
    vector<PCIPassthrough>      _devices;
};

} // namespace zte_tecs
#endif // #ifndef HA_HOST_AGENT_H

