#ifndef TECS_VM_DISK_SHARE_H
#define TECS_VM_DISK_SHARE_H
#include "pool_sql.h"
#include "vm_disk_url.h"

namespace zte_tecs {

/**
@brief 功能描述: VmDiskShare实体类\n
@li @b 其它说明: 无
*/
class VmDiskShare
{
public:
    string _request_id;
    int64 _hid;
    int64 _vid;
    string _target;
    int64 _cache_id;
    string _volume_uuid;
    FileUrl _disk_url;
    int _state;
    string _detail;
    int _last_op;

    VmDiskShare() 
    {
        _hid = INVALID_HID;
        _vid = INVALID_VID;
        _cache_id = INVALID_OID;
        _state = VDSS_UNKNOWN;
        _last_op = VDSO_UNKNOWN;
        _detail = "OK";
    };
};

/**
@brief 功能描述: VmDiskShare数据库操作类\n
@li @b 其它说明: 无
*/
class VmDiskSharePool : public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static VmDiskSharePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static VmDiskSharePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new VmDiskSharePool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const VmDiskShare  &vds);
    int Drop(const VmDiskShare  &vds);
    int UpdateVolumeUuidAndState(const VmDiskShare &vds);
    int UpdateUrlAndState(const VmDiskShare &vds);
    int UpdateState(const VmDiskShare &vds);
    int Select(vector<VmDiskShare> &vec_vds, const string &where);
    /* 继续添加数据库访问函数 */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */
    
    int SelectHostIpAndIscsiByHid(int64 hid, string &ip, string &iscsi);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/	
private:
     enum ColNames 
     {
         REQUEST_ID = 0,
         HID = 1,
         VID = 2,
         TARGET = 3,
         VOLUME_UUID = 4,
         DISK_URL = 5,
         CACHE_ID = 6,
         STATE = 7,
         DETAIL = 8,
         LAST_OP = 9,
         LIMIT = 10
     };

    VmDiskSharePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        
    };
    static VmDiskSharePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(VmDiskSharePool);
};
int GetHostIpAndIscsiByHid(int64 hid, string &ip, string &iscsi);
int GetSnapshotByCacheId(int64 id, vector<VmDiskShare> &vec_vds);
int GetShareDiskByVmInfo(int64 hid, int64 vid, const string &target, VmDiskShare &vds);
int GetShareDiskByVidAndTarget(int64 vid, const string &target, VmDiskShare &vds);
int GetShareDiskByRequestId(const string &request_id,VmDiskShare &vds);
int AllocateShareDisk(const VmDiskShare &vds);
int UpdateShareDiskVolumeUuid(const VmDiskShare &vds);
int UpdateShareDiskUrl(const VmDiskShare &vds);
int UpdateShareDiskState(const VmDiskShare &vds);
int DeleteShareDiskFromDb(const VmDiskShare &vds);
STATUS CreateVmShareDisk(MessageUnit *mu, 
                              const VmDiskConfig &disk,
                              const VmDiskShare &vds,
                              const ImageStoreOption &option);
STATUS CreateVmShareDisk(MessageUnit *mu, 
                              const VmDiskUrlReq& req, 
                              const VmDiskConfig &disk, 
                              VmDiskShare &vds, 
                              const ImageStoreOption &option);
STATUS AuthVmShareDisk(MessageUnit *mu, const VmDiskUrlReq& req,const VmDiskShare &vds);
STATUS PrepareVmShareDisk(MessageUnit *mu, const VmDiskUrlReq& req, const ImageStoreOption &option);
STATUS ReleaseVmShareDisk(MessageUnit *mu, const VmDiskUrlReq &req);
STATUS DealVmShareDiskAck(MessageUnit *mu);

}

#endif


