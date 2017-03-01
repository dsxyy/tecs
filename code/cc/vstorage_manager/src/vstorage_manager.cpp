/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vstorage_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������洢����ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��4��11��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/04/11
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "vstorage_manager.h"
#include "log_agent.h"
#include "tecs_config.h"
#include "image_url.h"
#include "vm_disk_url.h"
#include "image_agent.h"
#include "volume.h"
#include "host_pool.h"
#include "vm_disk_share.h"

namespace zte_tecs
{

static string img_src_type = IMAGE_SOURCE_TYPE_FILE;
static int img_spc_exp = 20;
static string img_usage = IMAGE_USE_SNAPSHOT;
static string image_agent_mode = "nfs";
static string share_img_usage = IMAGE_USE_SNAPSHOT;

/******************************************************************************/
DiskAgent *DiskAgent::_instance = NULL;
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
* 2012/5         V1.0        yanwei         ����
***************************************************************/
STATUS DiskAgent::StartMu(const string& name)
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

/******************************************************************************/
void DiskAgent::MessageEntry(const MessageFrame &message)
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
        switch (message.option.id())
        {
            case EV_VM_DISK_URL_PREPARE_REQ:
            {
                VmDiskUrlReq req;
                if(req.deserialize(message.data))
                {
                    ImageStoreOption option;
                    GetVstorageOption(option);
                    PrepareVmShareDisk(_mu,req,option);
                }
                else
                {
                    OutPut(SYS_ERROR,"failed to deserialize VmDiskUrlReq message:\n%s\n",
                        message.data.c_str());
                }
                break;
            }
            case EV_VM_DISK_URL_RELEASE_REQ:
            {
                VmDiskUrlReq req;
                if(req.deserialize(message.data))
                {
                    ReleaseVmShareDisk(_mu,req);
                }
                else
                {
                    OutPut(SYS_ERROR,"failed to deserialize VmDiskUrlReq message:\n%s\n",
                        message.data.c_str());
                }
                break;
            }
            #if 0
            case EV_STORAGE_SC_ACK:
            {
                DealVmShareDiskAck(_mu);
                break;
            }
            #endif
            default:
            {
                cout << "DiskAgent::MessageEntry: receive an unkown message!"
                     << endl;
                OutPut(SYS_NOTICE, "Unknown message: %d.\n", message.option.id());
                break;
            }
        }
        break;
    }

    default:
        break;
    }
}
/******************************************************************************/
STATUS EnableVstorageOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("img_src_type", img_src_type,"Image souce type , default = file(file or block).");
    config->EnableCustom("img_spc_exp", img_spc_exp,"Image space expand , default = 20 percent.");
    config->EnableCustom("img_usage", img_usage,"Image Usage , default = snapshot(snapshot , no-snapshot).");
    config->EnableCustom("image_agent_mode", image_agent_mode,"Image agent mode , default = none(none , nfs , or httpd).");
    config->EnableCustom("share_img_usage", share_img_usage,"Share Image Usage , default = snapshot(snapshot , no-snapshot).");
    return SUCCESS;
}

/******************************************************************************/
int GetSpaceRatio()
{
    if (img_src_type != IMAGE_SOURCE_TYPE_FILE)
    {
        return 0;
    }
    else
    {
        return img_spc_exp;
    }
}
/******************************************************************************/
void GetImgUsage(string &usage)
{
    usage = img_usage;
}
/******************************************************************************/
bool IsUseSnapshot()
{
    return (IMAGE_USE_SNAPSHOT == img_usage);
}
/******************************************************************************/
/* �����û����ã�����image�ļ���hc��ʵ��ռ�ÿռ��С
��Ҫ4m���� */
int64 CalcDiskRealSize(const VmDiskConfig &disk)
{
    int64 origi_size = disk._size;
    if (disk._id != INVALID_FILEID)
    {
        origi_size = disk._disk_size;
    }
    if (origi_size == 0)
    {//�����ʱ�򣬻��disk_size��Ϊ0
        return 0;
    }
    /* id=-1��ʱ��image��hc��ֻ����һ�ݣ����ļ���С����; ��=-1��ʱ������base+snapshot,�ļ���С���������� */
    if (disk._id != INVALID_FILEID)
    {
        origi_size = origi_size * (100 + GetSpaceRatio()) / 100;
    }
    else
    {
        if (VM_STORAGE_TO_SHARE == disk._position)
        {//�����̵�����ֶδ�С��0
            return 0;
        }
    }
	/* ����4M������size */
    return STORAGE_SIZE_ALIGNED(origi_size);
}
/******************************************************************************/
bool CheckVstorageOptions()
{
    if ((IMAGE_SOURCE_TYPE_FILE != img_src_type) && (IMAGE_SOURCE_TYPE_BLOCK !=  img_src_type))
    {
        return false;
    }
    if ((IMAGE_USE_SNAPSHOT != img_usage) && (IMAGE_USE_NOSNAPSHOT != img_usage))
    {
        return false;
    }
    if ((IMAGE_USE_SNAPSHOT != share_img_usage) && (IMAGE_USE_NOSNAPSHOT != share_img_usage))
    {
        return false;
    }
    if ( img_spc_exp < 0)
    {
        return false;
    }
    return true;
}
/******************************************************************************/
void GetVstorageOption(ImageStoreOption& storage_option)
{
    storage_option._img_srctype = img_src_type;
    storage_option._img_spcexp = img_spc_exp;
    storage_option._img_usage = img_usage;
    storage_option._share_img_usage = share_img_usage;
}
/**********************************************************************
* �������ƣ�VMManager::IsVMDiskSupportMigrate
* �����������жϸ�����Ĵ��̺;����Ƿ�֧��Ǩ��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2012/03/05   V1.0    ҦԶ        ����
************************************************************************/
bool IsVMStorageSupportMigrate(const VMConfig& config)
{
    vector<VmDiskConfig>::const_iterator disk;

    //ϵͳ��
    if (config._machine._position == VM_STORAGE_TO_LOCAL)
    {
        return false;
    }

    for ( disk = config._disks.begin(); disk != config._disks.end(); disk ++ )
    {
        if (VM_STORAGE_TO_LOCAL == disk->_position)
        {
            return false;
        }
    }

    return true;
}

STATUS DealVmStorageReq(MessageUnit *m,
                         int64 hid,
                         const VMConfig& config,
                         int op_type,
                         int &share_image_num,
                         int &share_disk_num,
                         ostringstream& error)
{
    uint32 msg_id;
    STATUS ret = ERROR;
    if ("none" == image_agent_mode)
    {
        OutPut(SYS_NOTICE,"image agent is not needed!\n");
        return ERROR;
    }
    HostPool *host_pool = HostPool::GetInstance();
    VmDiskUrlReq req;
    req._hid = hid;
    req._vid = config._vid;
    req._host_ip = host_pool->GetMediaAddressById(hid);
    if (req._host_ip.empty())
    {
        req._host_ip = host_pool->GetHostIpById(hid);
    }
    req._iscsi_initiator = host_pool->GetIscsinameById(hid);
    share_image_num = 0;
    share_disk_num = 0;

    OutPut(SYS_ERROR,"DealVmStorageReq for hid:%lld,vid:%lld,host_ip:%s,iscsi:%s\n",req._hid,req._vid,req._host_ip.c_str(),req._iscsi_initiator.c_str());
    if ((config._machine._id != INVALID_OID) && (IMAGE_POSITION_SHARE == config._machine._position))
    {
        req._disks.push_back(config._machine);
        share_image_num++;
    }

    if ((config._kernel._id != INVALID_OID) && (IMAGE_POSITION_SHARE == config._kernel._position))
    {
        req._disks.push_back(config._kernel);
        share_image_num++;
    }

    if ((config._initrd._id != INVALID_OID) && (IMAGE_POSITION_SHARE == config._initrd._position))
    {
        req._disks.push_back(config._initrd);
        share_image_num++;
    }

    vector<VmDiskConfig>::const_iterator it;
    for (it = config._disks.begin(); it != config._disks.end(); it++)
    {
        if ((it->_id != INVALID_OID) && (IMAGE_POSITION_SHARE == it->_position))
        {
            req._disks.push_back(*it);
            share_image_num++;
        }
    }
    if (0 != req._disks.size())
    {
        if (DEPLOY == op_type)
        {
            msg_id = EV_VM_IMAGE_URL_PREPARE_REQ;
        }
        else if (CANCEL == op_type)
        {
            msg_id = EV_VM_IMAGE_URL_RELEASE_REQ;
        }
        else
        {
            return ERROR;
        }
        MessageOption option(MID(PROC_IMAGE_AGENT,MU_IMAGE_AGENT), msg_id,0,0);
        if (SUCCESS != m->Send(req, option))
        {
            error << "vm " << config._vid << " get image url send req  failed! ret = " << ret;
            OutPut(SYS_DEBUG,"%s\n",error.str().c_str());
            return ret;
        }
        OutPut(SYS_ERROR,"DealVmStorageReq image share num:%d\n",share_image_num);
    }

    //������
    req._disks.clear();
    for (it = config._disks.begin(); it != config._disks.end(); it++)
    {
        if ((it->_id == INVALID_OID) && (STORAGE_POSITION_SHARE == it->_position))
        {
            req._disks.push_back(*it);
            share_disk_num++;
        }
    }

    if (0 != req._disks.size())
    {
        if (DEPLOY == op_type)
        {
            msg_id = EV_VM_DISK_URL_PREPARE_REQ;
        }
        else if (CANCEL == op_type)
        {
            msg_id = EV_VM_DISK_URL_RELEASE_REQ;
        }
        else
        {
            return ERROR;
        }
        MessageOption option(MID(PROC_CC,MU_DISK_AGENT), msg_id,0,0);
        if (SUCCESS != m->Send(req, option))
        {
            error << "vm " << config._vid << " get disk url send req failed! ret = " << ret;
            OutPut(SYS_DEBUG,"%s\n",error.str().c_str());
            return ret;
        }
        OutPut(SYS_ERROR,"DealVmStorageReq disk share num:%d\n",share_disk_num);
    }

    return SUCCESS;
}

STATUS PrepareVmStorage(MessageUnit *m,
                             int64 hid,
                             const VMConfig& config,
                             int &share_image_num,
                             int &share_disk_num,
                             ostringstream& error)
{
    return DealVmStorageReq(m,hid,config,DEPLOY,share_image_num,share_disk_num,error);
}

STATUS ReleaseVmStorage(MessageUnit *m,
                             int64 hid,
                             const VMConfig& config,
                             int &share_image_num,
                             int &share_disk_num,
                             ostringstream& error)
{
    return DealVmStorageReq(m,hid,config,CANCEL,share_image_num,share_disk_num,error);
}

VOID PrepareVmStorgeAck(const vector<VmDiskConfig> &disks,VMConfig& config)
{
    vector<VmDiskConfig>::const_iterator it;
    vector<VmDiskConfig>::iterator itcfg;

    for (it = disks.begin(); it != disks.end(); it++)
    {
        if ((INVALID_FILEID != it->_id) && (it->_id == config._kernel._id))
        {
            config._kernel._share_url = it->_share_url;
        }
        else if ((INVALID_FILEID != it->_id) && (it->_id == config._initrd._id))
        {
            config._initrd._share_url = it->_share_url;
        }
        else if ((INVALID_FILEID != it->_id) && (it->_id == config._machine._id))
        {
            config._machine._share_url = it->_share_url;
        }
        else
        {
            for (itcfg = config._disks.begin(); itcfg != config._disks.end(); itcfg++)
            {
                if ((it->_id == itcfg->_id) && (it->_target == itcfg->_target))
                {
                    itcfg->_share_url = it->_share_url;
                }
            }
        }
    }
}
STATUS VStorageInit()
{
    if (false == CheckVstorageOptions())
    {
        OutPut(SYS_EMERGENCY,"vstorage option invalid!!!\n");
        return ERROR_INVALID_ARGUMENT;
    }

    DiskAgent *da = DiskAgent::GetInstance();
    if (!da)
    {
        OutPut(SYS_EMERGENCY,"vstorage DiskAgent::GetInstance failed!!!\n");
        return ERROR;
    }
    if (SUCCESS != da->Init())
    {
        OutPut(SYS_EMERGENCY,"vstorage DiskAgent::Init failed!!!\n");
        return ERROR;
    }
    return SUCCESS;
}

void DbgSetVstorageOption(const char* imgsrctype,int imgspcexp,const char* imgusage,const char* shareimgusage)
{
    if(imgsrctype == NULL ||imgusage==NULL)
        return;
    string type=imgsrctype;
    string usage=imgusage;
    string shareusage= shareimgusage;
    if ((IMAGE_SOURCE_TYPE_BLOCK  != type) && (IMAGE_SOURCE_TYPE_FILE != type))
    {
        cout<<"image source type should be"<< IMAGE_SOURCE_TYPE_FILE<<" or "<<IMAGE_SOURCE_TYPE_BLOCK<<endl;
        return;
    }
    if ((IMAGE_USE_SNAPSHOT  != usage) && (IMAGE_USE_NOSNAPSHOT != usage))
    {
        cout<<"image usage should be"<< IMAGE_USE_SNAPSHOT<<" or "<<IMAGE_USE_NOSNAPSHOT<<endl;
        return;
    }
    if ((IMAGE_USE_SNAPSHOT  != shareusage) && (IMAGE_USE_NOSNAPSHOT != shareusage))
    {
        cout<<"share image usage should be"<< IMAGE_USE_SNAPSHOT<<" or "<<IMAGE_USE_NOSNAPSHOT<<endl;
        return;
    }
    if ( imgspcexp < 0)
    {
        cout<<"image space expand should be [0~X)"<<endl;
        return ;
    }
    img_src_type = type;
    img_spc_exp = imgspcexp;
    img_usage = usage;
    share_img_usage = shareimgusage;
}
#if 0
void DbgSetImageType(const char* psrctype,int spcexp,const char* usage)
{
    if(psrctype == NULL ||usage==NULL)
        return;
    string imgsrctype = psrctype;
    string imgusage = usage;
    int imgspcexp = spcexp;
    DbgSetVstorageOption(imgsrctype,imgspcexp,imgusage);
    cout<<"img_src_type:"<<img_src_type<<",imgusage:"<<imgusage<<endl;
}
#endif
void DbgShowVstorageOption(void)
{
    cout<<"img_src_type:"<<img_src_type<<",img_spc_exp:"<<img_spc_exp<<",img_usage:"<<img_usage<<",share_img_usage:"<<share_img_usage<<endl;
}
DEFINE_DEBUG_FUNC(DbgSetVstorageOption);
DEFINE_DEBUG_FUNC(DbgShowVstorageOption);

}

