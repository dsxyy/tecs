#ifndef TECS_CC_VSTORAGE_MANAGER_H
#define TECS_CC_VSTORAGE_MANAGER_H
#include "sky.h"
#include "event.h"
#include "mid.h"
#include "vm_messages.h"
#include "registered_system.h"
#include "pool_sql.h"
#include "vm_disk_url.h"


class ShareStorageInfo
{
public:
    ShareStorageInfo()
    {
        _iTotalCapacity = 0;
        _iFreeCapacity = 0;
        _iAllocMaxSize = 0;
    };
    
    ~ShareStorageInfo()    {};    

    int64   _iTotalCapacity; /*总大小*/    
    int64   _iFreeCapacity;  /*剩余总大小*/
    int64   _iAllocMaxSize;  /*能够申请的最大容量*/

};

namespace zte_tecs
{

class DiskAgent : public MessageHandler
{
public:
    static DiskAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new DiskAgent();
        }
        return _instance;
    }
    STATUS Init()
    {
        return StartMu(MU_DISK_AGENT);
    }
    virtual ~DiskAgent() { _instance = NULL; }
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数

private:
    MessageUnit         *_mu;               // 消息单元
    static DiskAgent    *_instance;
    DiskAgent()
    {

        _mu = NULL;
    }
    STATUS StartMu(const string& name);
    STATUS PrepareVmDiskUrl(const VmDiskUrlReq &req);
    STATUS ReleaseVmDiskUrl(const VmDiskUrlReq &req);

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(DiskAgent); 

};

STATUS EnableVstorageOptions();
STATUS PrepareVmStorage(MessageUnit *m,
                             int64 hid,
                             const VMConfig& config,
                             int &share_image_num,
                             int &share_disk_num,
                             ostringstream& error);
STATUS ReleaseVmStorage(MessageUnit *m,
                             int64 hid,
                             const VMConfig& config,
                             int &share_image_num,
                             int &share_disk_num,
                             ostringstream& error);
VOID PrepareVmStorgeAck(const vector<VmDiskConfig> &disks,VMConfig& config);
int GetSpaceRatio();
void GetImgUsage(string &usage);
bool IsUseSnapshot();
int64 CalcDiskRealSize(const VmDiskConfig &disk);
bool IsVMStorageSupportMigrate(const VMConfig& config);
void GetVstorageOption(ImageStoreOption& storage_option);
STATUS VStorageInit();
}
#endif

