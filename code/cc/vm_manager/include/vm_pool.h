#ifndef TECS_CC_VM_POOL_H
#define TECS_CC_VM_POOL_H

#include "pool_sql.h"
#include "vm.h"
#include "vm_messages.h"

namespace zte_tecs
{

class VmWorkFlow;
    
/**
 @brief VirtualMachinePool 类
 @li @b 其它说明：无
 */ 
class VirtualMachinePool: public PoolSQL
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static VirtualMachinePool* GetInstance()
    {
        return instance;
    };

    static VirtualMachinePool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new VirtualMachinePool(db); 
        }
        
        return instance;
    };
    
    virtual ~VirtualMachinePool();
    STATUS Init();
    
    //数据库中生成一个新的VirtualMachine对象
    int64 Allocate(int64 *oid,int64 vid);
    int64 Allocate(int64 *oid,VirtualMachine &vm);
    int GetAllVms(vector<int64>& vids,int limit);
    int GetVmsNotOnHost(vector<int64>& vids,int limit);
    
    int GetVmsByHost(vector<int64>& vids,int64 hid,int limit);
    int GetVmsByHostAndNext(vector<int64>& vids,int64 hid,int limit);    
    int GetVmsByLcmState(vector<int64>& vids,LcmState state,int limit);
    int GetVmsByLastOpResult(vector<int64>& vids,STATUS lastopresult,int limit);
    int64 GetHid(int64 vid);
    int GetLcmState(int64 vid,LcmState& state);
    int GetHAPolicy(int64 vid,VmHAPolicy& policy);
    int GetRunningInfo(int64 vid,VmRunningInfo& info);
    int GetNicConfig(int64 vid,vector<VmNicConfig>& nics);
    int IsVmExist(int64 vid);
    int32 GetVmsByCond(vector<int64> &vids, const string &cond);
    int IsWorkFlowExist(const string &name, int64 label1, int64 label2, int64 label3);
    int GetVmWorkFlowId(int64 vid, string &wf_id);
    int GetVmWorkFlowId(int64 vid, const string &op, string &wf_id);

    //获取一个VirtualMachine对象，如果缓存中没有，从数据库读取
    VirtualMachine *Get(int64 vid, bool lock)
    {
        //首先将虚拟机的vid转换成数据库表中的oid
        int64 oid = vid2oid(vid);
        if(INVALID_OID == oid)
            return NULL;

        VirtualMachine *pvm = static_cast<VirtualMachine *>(PoolSQL::Get(oid,lock));
        return pvm;
    };        

    //将虚拟机信息更新到数据库中
    int Update(VirtualMachine *pvm)
    {
        return pvm->Update(_db);
    };

    //将虚拟机信息从数据库中删除
    int Drop(VirtualMachine *pvm)
    {
        int rc = PoolSQL::Drop(pvm);
        if (rc == 0)
        {
            _known_vms.erase(pvm->_vid);
        }
        return rc;
    };

    //将虚拟机信息从数据库中删除
    int Drop(int64 vid)
    {
        VirtualMachine *pvm = Get(vid,true);
        int rc = Drop(pvm);
        Put(pvm,true);
        return rc;
    };
    
    int Dump(ostringstream& oss, const string& where);
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    VirtualMachinePool(SqlDB *db);
    DISALLOW_COPY_AND_ASSIGN(VirtualMachinePool);
    static VirtualMachinePool *instance;
    
    int64 vid2oid(int64 vid)
    {
        map<int64,int64>::const_iterator it;
        it = _known_vms.find(vid);
        if(it == _known_vms.end())
            return INVALID_OID;
        else
            return it->second;
    }

    //初始化oid的回调函数
    int InitCallback(void *nil, SqlColumn *columns);
    
    //dump的回调函数
    int DumpCallback(void *oss, SqlColumn *columns);

    //查找虚拟机的回调函数
    int SearchVmsCallback(void *nil, SqlColumn *columns);

    //创建资源池对象
    PoolObjectSQL *Create()
    {
        return new VirtualMachine;
    };
       
    //vid--oid mapping relation
    map<int64,int64> _known_vms;
};

class VmDevicePool: public Callbackable
{
public:
    static VmDevicePool* GetInstance(SqlDB *db = NULL)
    {   
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmDevicePool(db);
        }
        
        return _instance;
    };
    
    int InsertVmDevices(const vector<VmDeviceConfig> &devices, int64 id);
    int InsertVmDevicesCancel(const vector<VmDeviceConfig> &devices, int64 vid, int64 hid);

private:
    enum ColNames 
    {
        ID              = 0,
        TYPE            = 1,
        COUNT           = 2,
        LIMIT           = 3
    };
    
    VmDevicePool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmDevicePool);
    static VmDevicePool *_instance;
    SqlDB              *db;

    static const char *  _table_name;
    static const char *  _cancel_table_name;
    
    static const char *  _col_names;    
    static const char *  _cancel_col_names;    
};

class VmUsbPool: public SqlCallbackable
{
public:
    static VmUsbPool* GetInstance(SqlDB *db = NULL)
    {   
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmUsbPool(db);
        }
        
        return _instance;
    };

    int GetUsbInfoByVid(vector<int64> &vids, vector<VmQueryUsbInfo> &usb_list);
    bool IsUsbExist(int64 vid, int vendor_id, int product_id);
    int InsertVmUsbInfo(int64 vid, const VmUsbInfo &info);
    int DeleteVmUsbInfo(int64 vid, const VmUsbInfo &info);
    int SearchUsbInfoCallback(void *nil, SqlColumn *columns);
    int GetVidbyUsb(int64 &vid, int vendor_id, int product_id);

private:
    enum ColNames 
    {
        VID                  = 0,
        VENDOR_ID     = 1,
        PRODUCT_ID   = 2,
        NAME              = 3
    };
    
    VmUsbPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmUsbPool);
    
    static VmUsbPool *_instance;
    SqlDB              *db;
    static const char *  _table_name;
    static const char * _col_names;
};

}

#endif

