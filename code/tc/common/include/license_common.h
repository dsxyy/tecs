/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：license_common.h
* 文件标识：见配置管理计划书
* 内容摘要：证书管理器模块头文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年10月11日
*
* 修改记录1：
*     修改日期：2012/10/11
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#include "sky.h"
#include "pool_sql.h"
#include "license.h"

enum LicensedObject
{
    LICOBJ_CREATED_VMS = 0,
    LICOBJ_DEPLOYED_VMS = 1,
    LICOBJ_IMAGES = 2,
    LICOBJ_USERS = 3,
    LICOBJ_CLUSTERS = 4
};

int64 GetObjectCount(SqlCallbackable& sqlcb, LicensedObject object);
STATUS GetRunningStat(RunningStat& stat);
int LicenseRemainingDays();
void StopAllVM();
STATUS ValidateLicense(LicensedObject object, int64 increment);


