/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�instance_pool.h
* �ļ���ʶ��
* ����ժҪ��InstancePool��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��8��3��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/3
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HC_INSTANCEPOOL_H
#define HC_INSTANCEPOOL_H
#include "vm_pub.h"
class VmInstance;

/*********************************************************************************
 *                       �����ʵ����                                            *
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

    //��������
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
    //���캯��
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




