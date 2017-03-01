/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：instance_pool.h
* 文件标识：
* 内容摘要：InstancePool类的定义文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年8月3日
*
* 修改记录1：
*     修改日期：2011/8/3
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
*******************************************************************************/
#ifndef HC_INSTANCEPOOL_H
#define HC_INSTANCEPOOL_H
#include "vm_pub.h"
class VmInstance;

/*********************************************************************************
 *                       虚拟机实例池                                            *
 ********************************************************************************/
class InstancePool
{
public:
    static InstancePool *GetSelf()
    {
        if(NULL == self)
        {
            self = new InstancePool();
        }

        return self;
    };

    //析构函数
    ~InstancePool()
    {
    };

    bool DelInstance(int64 instanceid);
    void AddInstance(VmInstance* inst);
    VmInstance *GetInstanceByID(int64 instanceid);
    VmInstance* GetInstanceByDomID(int domid);
    VmInstance* GetInstanceByDomName(const string name);
    void SetDomIDByID(int64 instanceid, int domid);
    vector<VmInstance *>* GetInstancePool(void);
    void ShowAllInstance();
    void ShowInstanceById(int64 instanceid);
    void ShowInstanceByDomId(int domid);
    void LockInstance_pool(void);
    void UnlockInstance_pool(void);

private:
    //构造函数
    InstancePool()
    {
        _pool_mutex.Init();
    };
    
    static InstancePool  *self; 
    vector<VmInstance *> _instance_pool;
    Mutex                _pool_mutex;

    void Lock()
    {
        _pool_mutex.Lock();
    };

    void Unlock()
    {
        _pool_mutex.Unlock();
    };
};
#endif // #ifndef VH_INSTANCEPOOL_H




