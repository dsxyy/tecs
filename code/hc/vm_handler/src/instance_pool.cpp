/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：instance_pool.cpp
* 文件标识：
* 内容摘要：InstancePool类的实现文件
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
#include "instance_pool.h"
#include "vm_instance.h"

/*********************************************************************************
 *                       全局变量                                                *
 ********************************************************************************/
InstancePool* InstancePool::self = NULL;

/****************************************************************************************
* 函数名称：LockInstance_pool
* 功能描述：实例池加锁
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
void InstancePool::LockInstance_pool(void)
{
    Lock();
}

/****************************************************************************************
* 函数名称：UnlockInstance_pool
* 功能描述：实例池解锁
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
void InstancePool::UnlockInstance_pool(void)
{
    Unlock();
}

/****************************************************************************************
* 函数名称：GetInstancePool
* 功能描述：获得实例池句柄
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
vector<VmInstance *>*  InstancePool::GetInstancePool(void)
{
    return &_instance_pool;
}

/****************************************************************************************
* 函数名称：AddInstance
* 功能描述：往实例池增加一个实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
void InstancePool::AddInstance(VmInstance* inst)
{
    MutexLock lock(_pool_mutex);
    
    _instance_pool.push_back(inst);
}

/****************************************************************************************
* 函数名称：DelInstance
* 功能描述：从实例池删除一个实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
bool InstancePool::DelInstance(int64 instanceid)
{
    vector<VmInstance*>::iterator it;
    MutexLock lock(_pool_mutex);

    if (_instance_pool.empty())
    {
        return false;
    }
    
    for(it = _instance_pool.begin(); it != _instance_pool.end();)
    {
        if((*it)->GetInstanceId() ==  instanceid)
        {
            VmInstance *tmpInstance = (*it);
            it = _instance_pool.erase(it);
            delete(tmpInstance);
            return true;
        }
        else
       {
            ++it;
       }
    }
    
    return false;
}

/****************************************************************************************
* 函数名称：GetInstanceByID
* 功能描述：根据实例id获取对应实例句柄
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
VmInstance *InstancePool::GetInstanceByID(int64 instanceid)
{
    vector<VmInstance*>::iterator it;
    MutexLock lock(_pool_mutex);
    
    if (_instance_pool.empty())
    {
        return NULL;
    }
    
    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        if((*it)->GetInstanceId() == instanceid)
        {
            return *it;
        }
    }
    return NULL;
}

/****************************************************************************************
* 函数名称：GetInstanceByDomID
* 功能描述：根据domid获取对应实例句柄
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
VmInstance* InstancePool::GetInstanceByDomID(int domid)
{
    vector<VmInstance*>::iterator it;
    MutexLock lock(_pool_mutex);

    if (_instance_pool.empty())
    {
        return NULL;
    }
    
    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        if((*it)->GetDomainId() == domid)
        {
            return *it;
        }
    }
    return NULL;
}

/****************************************************************************************
* 函数名称：GetInstanceByDomID
* 功能描述：根据domid获取对应实例句柄
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
VmInstance* InstancePool::GetInstanceByDomName(const string name)
{
    vector<VmInstance*>::iterator it;
    MutexLock lock(_pool_mutex);

    if (_instance_pool.empty())
    {
        return NULL;
    }
    
    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        if((*it)->GetInstanceName() == name)
        {
            return *it;
        }
    }
    return NULL;
}

/****************************************************************************************
* 函数名称：ShowAllInstance
* 功能描述：查看实例池中的所有实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/    
void InstancePool::ShowAllInstance()
{
    vector<VmInstance*>::iterator it;

    if(_instance_pool.empty())
    {
        cout << "No instance." << endl;
        return;
    }
    
    MutexLock lock(_pool_mutex);

    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        (*it)->PrintInstanceInfo(false);
    }

    return;
}

/****************************************************************************************
* 函数名称：ShowInstanceById
* 功能描述：查看实例池中的指定实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/    
void InstancePool::ShowInstanceById(int64 instanceid)
{
    vector<VmInstance*>::iterator it;

    if(_instance_pool.empty())
    {
        cout << "No instance." << endl;
        return;
    }
    
    MutexLock lock(_pool_mutex);

    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        if((*it)->GetInstanceId() == instanceid)
        {
            (*it)->PrintInstanceInfo(true);
        }   
    }

    return;
}

/****************************************************************************************
* 函数名称：ShowInstanceByDomId
* 功能描述：查看实例池中的指定实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/   
void InstancePool::ShowInstanceByDomId(int domid)
{
    vector<VmInstance*>::iterator it;

    if(_instance_pool.empty())
    {
        cout << "No instance." << endl;
        return;
    }
    
    MutexLock lock(_pool_mutex);

    for(it = _instance_pool.begin(); it != _instance_pool.end(); ++it)
    {
        if((*it)->GetDomainId() == domid)
        {
            (*it)->PrintInstanceInfo(true);
        }   
    }

    return;
}



/**********************************************************************************
 *                           调试函数                                             *
 *********************************************************************************/

/****************************************************************************************
* 函数名称：VMX_DbgShowAllInstance
* 功能描述：查看实例池中的所有实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/   
void VMX_DbgShowAllInstance()
{
   InstancePool * pInstPool = InstancePool::GetSelf();
   if (pInstPool == NULL)
   {
       cout << "InstancePool is NULL!!" << endl;
       return;
   }
   pInstPool->ShowAllInstance();
}

/****************************************************************************************
* 函数名称：VMX_DbgDelInstanceByID
* 功能描述：删除实例池中的指定实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/   
void VMX_DbgDelInstanceByID(int64 instanceid)
{
   InstancePool * pInstPool = InstancePool::GetSelf();
   if (pInstPool == NULL)
   {
       cout << "InstancePool is NULL!!" << endl;
       return;
   }
   pInstPool->DelInstance(instanceid);
}

/****************************************************************************************
* 函数名称：VMX_DbgShowInstanceById
* 功能描述：查看实例池中的指定实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/  
void VMX_DbgShowInstanceById(int64 instanceid)
{
   InstancePool * pInstPool = InstancePool::GetSelf();
   if (pInstPool == NULL)
   {
       cout << "InstancePool is NULL!!" << endl;
       return;
   }
   pInstPool->ShowInstanceById(instanceid);
}

/****************************************************************************************
* 函数名称：VMX_DbgShowInstanceByDomId
* 功能描述：查看实例池中的指定实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/  
void VMX_DbgShowInstanceByDomId(int domid)
{
   InstancePool * pInstPool = InstancePool::GetSelf();
   if (pInstPool == NULL)
   {
       cout << "InstancePool is NULL!!" << endl;
       return;
   }
   pInstPool->ShowInstanceByDomId(domid);
}

DEFINE_DEBUG_FUNC(VMX_DbgShowAllInstance);
DEFINE_DEBUG_CMD(instances,VMX_DbgShowAllInstance);
DEFINE_DEBUG_FUNC(VMX_DbgDelInstanceByID);
DEFINE_DEBUG_FUNC(VMX_DbgShowInstanceById);
DEFINE_DEBUG_FUNC(VMX_DbgShowInstanceByDomId);




